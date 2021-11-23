#include "StdAfx.h"
#include "joinery_solver_rhino7PlugIn.h"
#include "compas.h"
#include "connection_zones.h"
#include <exception>

#include <chrono>
using namespace std::chrono;

#pragma region command_get_polylines command

class command_get_polylines : public CRhinoCommand
{
public:

	command_get_polylines() = default;
	~command_get_polylines() = default;

	UUID CommandUUID() override
	{
		static const GUID joinery_solver_rhino7Command_UUID =
		{ 0x3118197D, 0x9A40, 0x498D, { 0x84, 0x19, 0x73, 0xD8, 0x71, 0xF0, 0x68, 0x51 } };
		return joinery_solver_rhino7Command_UUID;
	}

	const wchar_t* EnglishCommandName() override { return L"joinery_solver"; }
	CRhinoCommand::result RunCommand(const CRhinoCommandContext& context) override;
};



static class command_get_polylines thejoinery_solver_rhino7Command;

static int RhinoFindOrCreateLayer(CRhinoDoc& doc, const wchar_t* layer_name) {
	if (nullptr == layer_name || 0 == layer_name[0])
		return ON_UNSET_INT_INDEX;

	int layer_index = doc.m_layer_table.FindLayerFromFullPathName(layer_name, ON_UNSET_INT_INDEX);
	if (layer_index == ON_UNSET_INT_INDEX) {
		ON_Layer layer;
		layer.SetName(layer_name);
		layer.SetColor(ON_Color(255, 0, 0));
		layer.SetPlotColor(ON_Color(255, 0, 0));
		layer_index = doc.m_layer_table.AddLayer(layer);
	}

	return layer_index;
}

CRhinoCommand::result command_get_polylines::RunCommand(const CRhinoCommandContext& context)
{

	/////////////////////////////////////////////////////////////////////
	//Get Polylines and Convert to CGAL Polylines
	/////////////////////////////////////////////////////////////////////
	CRhinoGetObject go;
	go.SetCommandPrompt(L"Select pairs of polylines");
	go.SetGeometryFilter(CRhinoGetObject::curve_object);
	go.GetObjects(1, 0);
	if (go.CommandResult() != success) return go.CommandResult();

	std::vector<CGAL_Polyline> input_polyline_pairs;
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

	if (input_polyline_pairs.size() == 0) return failure;

	//RhinoApp().Print(L"Input %d \n", input_polyline_pairs.size());
	//for (int i = 0; i < input_polyline_pairs.size(); i++) {
	//	for (size_t j = 0; j < input_polyline_pairs[i].size(); j++)
	//	{
	//		RhinoApp().Print(L"Input %f %f %f \n", input_polyline_pairs[i][j].hx(), input_polyline_pairs[i][j].hy(), input_polyline_pairs[i][j].hz());
	//	}
	//	RhinoApp().Print(L"\n %d ", input_polyline_pairs[i].size());
	//}
	//RhinoApp().Print(L"Input %d \n", input_polyline_pairs.size());
	//return success;

	/////////////////////////////////////////////////////////////////////
	//Call Main Method
	/////////////////////////////////////////////////////////////////////
	RhinoApp().Print("============================================================================== CPP + \n");
	auto start = high_resolution_clock::now();
	//input
	std::vector<std::vector<IK::Vector_3>> input_insertion_vectors;
	std::vector<std::vector<int>> input_joint_types;
	std::vector<std::vector<int>> input_three_valence_element_indices_and_instruction;
	//output
	std::vector<std::vector<CGAL_Polyline>> output_polyline_groups;
	//parameters
	int search_type = 1;
	double division_distance = 300;
	double shift = 0.6;

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
		}	catch (...) {
			RhinoApp().Print("Caught exception : Some other exception");
		}
	
	//}	__except (EXCEPTION_EXECUTE_HANDLER) {
		//SEH handling
		//printf("Executing SEH __except block\r\n");
	//}
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<milliseconds>(stop - start);
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
			CRhinoCurveObject* curve_object = context.m_doc.AddCurveObject(pline,&attributes);
			group_members.Append(curve_object);
		}

		int group_index = context.m_doc.m_group_table.AddGroup(ON_Group(), group_members);
		if (group_index < 0) CRhinoCommand::failure;
	}
	context.m_doc.Redraw();



	return CRhinoCommand::success;
}

#pragma endregion

