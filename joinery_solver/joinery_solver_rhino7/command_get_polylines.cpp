#include "StdAfx.h"
#include "RhinoUI.h"
#include "joinery_solver_rhino7PlugIn.h"
#include "compas.h"
#include "connection_zones.h"
#include <exception>

#include <chrono>

#pragma region command_get_polylines command







bool UI(const CRhinoCommandContext& context, std::vector<CGAL_Polyline>& input_polyline_pairs, int& search_type , double& division_distance, double& shift, int& output_type, std::vector<double>& joint_parameters) {

	/////////////////////////////////////////////////////////////////////
	//Get Polylines and Convert to CGAL Polylines |
	/////////////////////////////////////////////////////////////////////
	CRhinoGetObject go;
	go.SetCommandPrompt(L"Select pairs of polylines");
	go.SetGeometryFilter(CRhinoGetObject::curve_object);
	go.GetObjects(1, 0);
	if (go.CommandResult() != CRhinoCommand::success) return false;

	go.SetGeometryFilter(CRhinoGetObject::curve_object);


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

	if (input_polyline_pairs.size() == 0 || input_polyline_pairs.size() % 2 == 1) return false;


	/////////////////////////////////////////////////////////////////////
	//search_type | division_distance | shift
	/////////////////////////////////////////////////////////////////////
	search_type = 2;
	division_distance = 1000;
	shift = 0.5;
	output_type = 4;
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


		int search_type_value_index = menu.AddCommandOptionInteger(RHCMDOPTNAME(L"search_type"), &search_type, L"0 - face_to_face, 1 - plane_to_face, 2 - all", 0, 2);
		int division_distance_value_index = menu.AddCommandOptionNumber(RHCMDOPTNAME(L"division_distance"), &division_distance, L"connection length divided by division_distance", FALSE, 0.001, 100000.0);
		int shift_value_index = menu.AddCommandOptionNumber(RHCMDOPTNAME(L"shift"), &shift, L"joint shift value applied for all joints", FALSE, 0.0, 1.0);
		int output_type_value_index = menu.AddCommandOptionInteger(RHCMDOPTNAME(L"output_type"), &output_type, L"0 - joint area, 1 joint lines, 2 - joint volumes, 3 - joint geometry, 4 - joint merged with two outlines", 0, 4);

		
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

		if (option_index == output_type_value_index)
			continue; // nothing to do

	}

	/////////////////////////////////////////////////////////////////////
	//Joint Parameters 
	/////////////////////////////////////////////////////////////////////


	//face_to_face side - to - side parallel in - plane | joinery_library 1 - 9 | type 12
//face_to_face side - to - side parallel out - of - plane | joinery_library 10 - 19 | type 11
//face_to_face top - to - side | joinery_library 20 - 29 | type 20
//plane_to_face cross | joinery_library 30 - 39 | type 30
//face_to_face top_to_top | joinery_library 40 - 49 |
//face_to_face side - to - side | joinery_library non - parallel 50 - 59 |
	//int list_index = 3;
	// joint_parameters = { 1000, 0.5, 1,  1000, 0.5, 10 ,  1000, 0.5, 20 ,  1000, 0.5, 30 ,  1000, 0.5, 40 ,  1000, 0.5, 50 };


	CRhinoGetOption menu_joint_parameters;
	menu_joint_parameters.SetCommandPrompt(L"Individual joint parameters");
	menu_joint_parameters.AcceptNothing();

	for (;;) {

		menu_joint_parameters.ClearCommandOptions();

	std::array<int, 18> ids;
		ids[0] = menu_joint_parameters.AddCommandOptionNumber(RHCMDOPTNAME(L"SSI_Dist"), &joint_parameters[0], nullptr, FALSE, 0.001, 100000.0);
		ids[1] = menu_joint_parameters.AddCommandOptionNumber(RHCMDOPTNAME(L"SSI_Shift"), &joint_parameters[1], nullptr, FALSE, 0.0, 1.0);
		ids[2] = menu_joint_parameters.AddCommandOptionNumber(RHCMDOPTNAME(L"SSI_Type"), &joint_parameters[2], nullptr, FALSE, 1, 9);

		ids[3] = menu_joint_parameters.AddCommandOptionNumber(RHCMDOPTNAME(L"_____SSO_Dist"), &joint_parameters[3], nullptr, FALSE, 0.001, 100000.0);
		ids[4] = menu_joint_parameters.AddCommandOptionNumber(RHCMDOPTNAME(L"SSO_Shift"), &joint_parameters[4],nullptr, FALSE, 0.0, 1.0);
		ids[5] = menu_joint_parameters.AddCommandOptionNumber(RHCMDOPTNAME(L"SSO_Type"), &joint_parameters[5], nullptr, FALSE, 10, 19);

		ids[6] = menu_joint_parameters.AddCommandOptionNumber(RHCMDOPTNAME(L"_____TS_Dist"), &joint_parameters[6],nullptr, FALSE, 0.001, 100000.0);
		ids[7] = menu_joint_parameters.AddCommandOptionNumber(RHCMDOPTNAME(L"TS_Shift"), &joint_parameters[7],nullptr, FALSE, 0.0, 1.0);
		ids[8] = menu_joint_parameters.AddCommandOptionNumber(RHCMDOPTNAME(L"TS_Type"), &joint_parameters[8],nullptr, FALSE, 20, 29);

		ids[9] = menu_joint_parameters.AddCommandOptionNumber(RHCMDOPTNAME(L"_____C_Dist"), &joint_parameters[9], nullptr, FALSE, 0.001, 100000.0);
		ids[10] = menu_joint_parameters.AddCommandOptionNumber(RHCMDOPTNAME(L"C_Shift"), &joint_parameters[10], nullptr, FALSE, 0.0, 1.0);
		ids[11] = menu_joint_parameters.AddCommandOptionNumber(RHCMDOPTNAME(L"C_Type"), &joint_parameters[11], nullptr, FALSE, 30, 39);

		ids[12] = menu_joint_parameters.AddCommandOptionNumber(RHCMDOPTNAME(L"_____TT_Dist"), &joint_parameters[12],nullptr, FALSE, 0.001, 100000.0);
		ids[13] = menu_joint_parameters.AddCommandOptionNumber(RHCMDOPTNAME(L"TT_Shift"), &joint_parameters[13],nullptr, FALSE, 0.0, 1.0);
		ids[14] = menu_joint_parameters.AddCommandOptionNumber(RHCMDOPTNAME(L"TT_Type"), &joint_parameters[14], nullptr, FALSE, 40, 49);

		ids[15] = menu_joint_parameters.AddCommandOptionNumber(RHCMDOPTNAME(L"_____SSR_Dist"), &joint_parameters[15],nullptr, FALSE, 0.001, 100000.0);
		ids[16] = menu_joint_parameters.AddCommandOptionNumber(RHCMDOPTNAME(L"SSR_Shift"), &joint_parameters[16],nullptr, FALSE, 0.0, 1.0);
		ids[17] = menu_joint_parameters.AddCommandOptionNumber(RHCMDOPTNAME(L"SSR_Type"), &joint_parameters[17], nullptr, FALSE, 50, 59);


		CRhinoGet::result res = menu_joint_parameters.GetOption();

		if (res == CRhinoGet::nothing)
			break;
		if (res == CRhinoGet::cancel)
			return false;
		if (res != CRhinoGet::option)
			return false;

		const CRhinoCommandOption* option = menu_joint_parameters.Option();
		if (nullptr == option)
			return false;
	}


	for (int i = 0; i < joint_parameters.size(); i++) {
		RhinoApp().Print(L" %f ", joint_parameters[i]);
	}
	RhinoApp().Print(L" \n");
	

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
	int output_type = 4;
	//side-to-side parallel in-plane |  side-to-side parallel | side-to-side out-of-plane |  top-to-side | cross | top-to-top |  side-to-side non-parallel
	std::vector<double> joint_parameters =  { 1000, 0.5, 1,  1000, 0.5, 10 ,  1000, 0.5, 20 ,  1000, 0.5, 30 ,  1000, 0.5, 40 ,  1000, 0.5, 50 };
	if (!UI(context, input_polyline_pairs, search_type, division_distance, shift, output_type, joint_parameters)) return CRhinoCommand::failure;


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



	//__try {
	try {
		get_connection_zones(
			input_polyline_pairs,
			input_insertion_vectors,
			input_joint_types,
			input_three_valence_element_indices_and_instruction,
			output_polyline_groups,

			joint_parameters,
			search_type,
			division_distance,
			shift,
			output_type
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

