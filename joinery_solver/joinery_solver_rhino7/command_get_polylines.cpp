#include "StdAfx.h"
#include "RhinoUI.h"
#include "joinery_solver_rhino7PlugIn.h"
#include "compas.h"
#include "connection_zones.h"
#include <exception>

#include <chrono>

#pragma region command_get_polylines command







bool UI(const CRhinoCommandContext& context, std::vector<CGAL_Polyline>& input_polyline_pairs, int& search_type , double& division_distance, double& shift) {

	/////////////////////////////////////////////////////////////////////
	//Get Polylines and Convert to CGAL Polylines |
	/////////////////////////////////////////////////////////////////////
	CRhinoGetObject go;
	go.SetCommandPrompt(L"Select pairs of polylines");
	go.SetGeometryFilter(CRhinoGetObject::curve_object);
	go.GetObjects(1, 0);
	if (go.CommandResult() != CRhinoCommand::success) return false;


	input_polyline_pairs.reserve(go.ObjectCount() - go.ObjectCount() % 2);

	for (int i = 0; i < go.ObjectCount() - go.ObjectCount() % 2; i++) {

		//Convert Curve to Polyline
		const CRhinoObjRef& o = go.Object(i);
		const  ON_Curve* crv = o.Curve();
		if (!crv) continue;
		ON_SimpleArray<ON_3dPoint> points;
		crv->IsPolyline(&points);
		if (points.Count() < 3) continue;

		//Construct CGAL_Polyline
		CGAL_Polyline pline;
		pline.reserve(points.Count());
		for (size_t i = 0; i < points.Count(); i++)
			pline.emplace_back(points[i].x, points[i].y, points[i].z);
		input_polyline_pairs.emplace_back(pline);
	}

	if (input_polyline_pairs.size() == 0) return false;


	/////////////////////////////////////////////////////////////////////
	//search_type | division_distance | shift
	/////////////////////////////////////////////////////////////////////
	search_type = 2;
	division_distance = 1000;
	shift = 0.5;

	/////////////////////////////////////////////////////////////////////
	//Command Line Menu
	/////////////////////////////////////////////////////////////////////
	CRhinoGetOption menu;
	menu.SetCommandPrompt(L"Parameter for the Joinery Solver");
	menu.AcceptNothing();

	/////////////////////////////////////////////////////////////////////
	//Iterate parameters until user stops
	/////////////////////////////////////////////////////////////////////
	for (;;) {

		menu.ClearCommandOptions();

		//int nval_option_index = menu.AddCommandOptionInteger(RHCMDOPTNAME(L"Integer"), &search_type, L"0 - face_to_face, 1 - plane_to_face, 2 - all", 1, 99);


		int search_type_value_index = menu.AddCommandOptionInteger(RHCMDOPTNAME(L"search_type"), &search_type, L"0 - face_to_face, 1 - plane_to_face, 2 - all", 0, 2);
		int division_distance_value_index = menu.AddCommandOptionNumber(RHCMDOPTNAME(L"division_distance"), &division_distance, L"connection length divided by division_distance", FALSE, 0.001, 100000.0);
		int shift_value_index = menu.AddCommandOptionNumber(RHCMDOPTNAME(L"shift"), &shift, L"joint shift value applied for all joints", FALSE, 0.0, 1.0);

		CRhinoGet::result res = menu.GetOption();

		if (res == CRhinoGet::nothing)
			break;
		if (res == CRhinoGet::cancel)
			return false;
		if (res != CRhinoGet::option)
			return false;

		const CRhinoCommandOption* option = menu.Option();
		if (nullptr == option)
			return false;

		int option_index = option->m_option_index;

		if (option_index == search_type_value_index)
			continue; // nothing to do

		if (option_index == division_distance_value_index)
			continue; // nothing to do

		if (option_index == shift_value_index)
			continue; // nothing to do


	}



	/////////////////////////////////////////////////////////////////////
	//Output
	/////////////////////////////////////////////////////////////////////
	return true;



}









class command_get_polylines : public CRhinoCommand {
public:

	command_get_polylines() = default;
	~command_get_polylines() = default;

	UUID CommandUUID() override {
		static const GUID joinery_solver_rhino7Command_UUID =
		{ 0x3118197D, 0x9A40, 0x498D, { 0x84, 0x19, 0x73, 0xD8, 0x71, 0xF0, 0x68, 0x51 } };
		return joinery_solver_rhino7Command_UUID;
	}

	const wchar_t* EnglishCommandName() override { return L"joinery_solver"; }
	CRhinoCommand::result RunCommand(const CRhinoCommandContext& context) override;
};

static class command_get_polylines thejoinery_solver_rhino7Command;



CRhinoCommand::result command_get_polylines::RunCommand(const CRhinoCommandContext& context) {

	/////////////////////////////////////////////////////////////////////
	//Input
	/////////////////////////////////////////////////////////////////////
	std::vector<CGAL_Polyline> input_polyline_pairs;
	int search_type = 2;
	double division_distance = 1000;
	double shift = 0.5;
	if (!UI(context, input_polyline_pairs, search_type, division_distance, shift)) return CRhinoCommand::failure;


	/////////////////////////////////////////////////////////////////////
	//Main Method
	/////////////////////////////////////////////////////////////////////
	RhinoApp().Print("============================================================================== CPP + \n");
	auto start = std::chrono::high_resolution_clock::now();
	//input
	std::vector<std::vector<IK::Vector_3>> input_insertion_vectors;
	std::vector<std::vector<int>> input_joint_types;
	std::vector<std::vector<int>> input_three_valence_element_indices_and_instruction;
	//output
	std::vector<std::vector<CGAL_Polyline>> output_polyline_groups;



	//get_connection_zones_test();
	//__try {
	try {
		get_connection_zones(
			input_polyline_pairs,
			input_insertion_vectors,
			input_joint_types,
			input_three_valence_element_indices_and_instruction,
			output_polyline_groups,
			search_type,
			division_distance,
			shift
		);

	} catch (char const* exception) {
		RhinoApp().Print(exception);
	} catch (...) {
		RhinoApp().Print("Caught exception : Some other exception");
	}

	//}	__except (EXCEPTION_EXECUTE_HANDLER) {
		//SEH handling
		//printf("Executing SEH __except block\r\n");
	//}
	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	RhinoApp().Print("==================================== %d ms ==================================== \n", duration);
	RhinoApp().Print("============================================================================== CPP - \n");

	/////////////////////////////////////////////////////////////////////
	//Output
	/////////////////////////////////////////////////////////////////////

	RhinoApp().Print(L"Input %d \n", input_polyline_pairs.size());
	RhinoApp().Print(L"Output %d \n", output_polyline_groups.size());

	//Create layer
	const wchar_t* layer_name = L"joinery_solver";
	int layer_index = RhinoFindOrCreateLayer(context.m_doc, layer_name);
	if (layer_index == ON_UNSET_INT_INDEX)
		return CRhinoCommand::failure;

	ON_3dmObjectAttributes attributes;
	context.m_doc.GetDefaultObjectAttributes(attributes);
	attributes.m_layer_index = layer_index;

	for (auto& output : output_polyline_groups) {

		ON_SimpleArray<const CRhinoObject*> group_members(output.size());
		for (int i = 0; i < output.size(); i++) {

			ON_3dPointArray points;
			points.Reserve(output[i].size());
			for (size_t j = 0; j < output[i].size(); j++)
				points.Append(ON_3dPoint(output[i][j].hx(), output[i][j].hy(), output[i][j].hz()));

			ON_Polyline pline(points);
			CRhinoCurveObject* curve_object = context.m_doc.AddCurveObject(pline, &attributes);
			group_members.Append(curve_object);
		}

		int group_index = context.m_doc.m_group_table.AddGroup(ON_Group(), group_members);
		if (group_index < 0) CRhinoCommand::failure;
	}
	context.m_doc.Redraw();



	return CRhinoCommand::success;
}

#pragma endregion

