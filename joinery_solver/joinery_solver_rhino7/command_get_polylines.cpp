#include "StdAfx.h"
#include "RhinoUI.h"
#include "joinery_solver_rhino7PlugIn.h"
#include "compas.h"
#include "connection_zones.h"
#include <exception>
#include "string"
#include <chrono>

#pragma region command_get_polylines command



class CGetTextDotObject : public CRhinoGetObject {
	bool CustomGeometryFilter(const CRhinoObject* object, const ON_Geometry* geometry, ON_COMPONENT_INDEX component_index) const;
};

bool CGetTextDotObject::CustomGeometryFilter(const CRhinoObject* object, const ON_Geometry* geometry, ON_COMPONENT_INDEX component_index) const {
	UNREFERENCED_PARAMETER(object);
	UNREFERENCED_PARAMETER(component_index);
	bool rc = false;
	if (geometry) {
		const ON_TextDot* p = ON_TextDot::Cast(geometry);
		if (p)
			rc = true;
	}
	return rc;
}




bool UI(const CRhinoCommandContext& context, std::vector<CGAL_Polyline>& input_polyline_pairs, int& search_type, double& division_distance, double& shift, int& output_type,
	std::vector<double>& joint_parameters, std::vector<std::vector<IK::Vector_3>>& input_insertion_vectors, std::vector<std::vector<int>>& input_joint_types) {

	double tol = context.m_doc.AbsoluteTolerance() * 10;
#pragma region GetPolylines



	/////////////////////////////////////////////////////////////////////
	//Get Polylines and Convert to CGAL Polylines |
	/////////////////////////////////////////////////////////////////////
	CRhinoGetObject go;
	go.SetCommandPrompt(L"Select pairs of polylines");
	go.SetGeometryFilter(CRhinoGetObject::curve_object);
	go.GetObjects(1, 0);
	if (go.CommandResult() != CRhinoCommand::success) return false;



	std::vector<CGAL_Polyline> input_polyline_pairs_temp;
	input_polyline_pairs_temp.reserve(go.ObjectCount() );
	input_polyline_pairs.reserve(go.ObjectCount() );

	for (int i = 0; i < go.ObjectCount() ; i++) {

		//Convert Curve to Polyline
		const CRhinoObjRef& o = go.Object(i);
		const  ON_Curve* crv = o.Curve();
		if (!crv) continue;
		ON_SimpleArray<ON_3dPoint> points;
		crv->IsPolyline(&points);
		
		if (points.Count() < 3) continue;
		if (points.First()->DistanceTo(*points.Last()) > tol) continue;

		//Construct CGAL_Polyline
		CGAL_Polyline pline;
		pline.reserve(points.Count());
		for (size_t i = 0; i < points.Count(); i++)
			pline.emplace_back(points[i].x, points[i].y, points[i].z);
		input_polyline_pairs_temp.emplace_back(pline);
	}



	if (input_polyline_pairs_temp.size() == 0 || input_polyline_pairs_temp.size() % 2 == 1) return false;


	//Re-test pairs by measuring point distance 
	for (int i = 0; i < input_polyline_pairs_temp.size()- input_polyline_pairs_temp.size()%2; i+=2) {

		if (input_polyline_pairs_temp[i].size() != input_polyline_pairs_temp[i + 1].size())continue;
	
		double dist0 = CGAL::squared_distance(input_polyline_pairs_temp[i][0], input_polyline_pairs_temp[i + 1][0]);
		double dist1 = CGAL::squared_distance(input_polyline_pairs_temp[i][1], input_polyline_pairs_temp[i + 1][1]);
		double dist2 = CGAL::squared_distance(input_polyline_pairs_temp[i][2], input_polyline_pairs_temp[i + 1][2]);
		double check0 = std::abs(dist0 - dist1);
		double check1 = std::abs(dist0 - dist2);

		//RhinoApp().Print(L" Distance difference between polylines %f, %f \n", check0, check1);
		if(true){
		//if (check0 < GlobalToleranceSquare * 10 && check1 < GlobalToleranceSquare * 10) {
			input_polyline_pairs.emplace_back(input_polyline_pairs_temp[i]);
			input_polyline_pairs.emplace_back(input_polyline_pairs_temp[i+1]);
		}
	}

	if (input_polyline_pairs.size() == 0 || input_polyline_pairs.size() % 2 == 1) return false;
	


	//Create a container to store full empty insertion vectors
	int n = input_polyline_pairs.size() * 0.5;

	input_insertion_vectors = std::vector<std::vector<IK::Vector_3>>(n);
	input_joint_types = std::vector<std::vector<int>>(n);

	for (int i = 0; i < input_insertion_vectors.size(); i++) {

		input_insertion_vectors[i].reserve(input_polyline_pairs[i * 2].size() + 1);
		input_joint_types[i].reserve(input_polyline_pairs[i * 2].size() + 1);
		for (int j = 0; j < input_polyline_pairs[i * 2].size() + 1; j++) {
			input_insertion_vectors[i].emplace_back(0, 0, 0);
			input_joint_types[i].emplace_back(-1);
		}
	}

#pragma endregion

#pragma region GetInsertionDirection
	/////////////////////////////////////////////////////////////////////
	//Get Insertion direction
	/////////////////////////////////////////////////////////////////////
	int collision_count = 0;
	CRhinoGetObject go2;
	go2.SetCommandPrompt(L"Select Lines for Insertion Vectors");
	go2.SetGeometryFilter(CRhinoGetObject::curve_object);
	go2.EnablePreSelect(FALSE);
	go2.EnableDeselectAllBeforePostSelect(FALSE);
	go2.GetObjects(1, 0);



	std::vector<ON_Line> insertion_lines;
	insertion_lines.reserve(go2.ObjectCount());
	if (go2.CommandResult() == CRhinoCommand::success) {
		for (int i = 0; i < go2.ObjectCount(); i++) {
			//Convert Curve to Polyline
			const CRhinoObjRef& o = go2.Object(i);
			const  ON_Curve* crv = o.Curve();
			if (!crv) continue;
			ON_SimpleArray<ON_3dPoint> points;
			crv->IsPolyline(&points);
			if (points.Count() != 2) continue;
			//Construct Line

			insertion_lines.emplace_back(points[0], points[1]);
		}

		//if (insertion_lines.size() == 0) return false;
		RhinoApp().Print(L" %i  \n", insertion_lines.size());
	}

	//Create RTree
	RTree<int, double, 3> tree;

	//Insert AABB
	RhinoApp().Print(L" Number of Elements: %i \n", n);
	for (int i = 0; i < n; i++) {

		//Create copy of a polyline and transform points
		CGAL_Polyline twoPolylines;
		twoPolylines.resize(input_polyline_pairs[i*2].size() + input_polyline_pairs[i*2 + 1].size());
		std::copy(input_polyline_pairs[i*2].begin(), input_polyline_pairs[i*2].end(), twoPolylines.begin());
		std::copy(input_polyline_pairs[i*2 + 1].begin(), input_polyline_pairs[i*2 + 1].end(), twoPolylines.begin() + input_polyline_pairs[i].size());


		CGAL::Bbox_3 AABB = CGAL::bbox_3(twoPolylines.begin(), twoPolylines.end(), IK());

		CGAL_Polyline AABB_Min_Max = {
			IK::Point_3(AABB.xmin() - 1 * GlobalTolerance, AABB.ymin() - 1 * GlobalTolerance, AABB.zmin() - 1 * GlobalTolerance),
			IK::Point_3(AABB.xmax() + 1 * GlobalTolerance, AABB.ymax() + 1 * GlobalTolerance, AABB.zmax() + 1 * GlobalTolerance),
		};

		AABB = CGAL::bbox_3(AABB_Min_Max.begin(), AABB_Min_Max.end(), IK());

		double min[3] = { AABB.xmin(), AABB.ymin(), AABB.zmin() };
		double max[3] = { AABB.xmax(), AABB.ymax(), AABB.zmax() };
		tree.Insert(min, max, i);
	}




	collision_count = 0;

	for (int i = 0; i < insertion_lines.size(); i++) {//AABB.size()

		//Get lines points
		IK::Point_3 p0(insertion_lines[i].from.x, insertion_lines[i].from.y, insertion_lines[i].from.z);
		IK::Point_3 p1(insertion_lines[i].to.x, insertion_lines[i].to.y, insertion_lines[i].to.z);
		IK::Vector_3 v = IK::Vector_3(p0 - p1);

		//std::vector<int> result;
		auto callback = [i, &insertion_lines, &input_polyline_pairs, &input_insertion_vectors, &collision_count, &p0, &p1, &v](int foundValue) -> bool
		{



			int edge = 0;
			double closest_distance = std::abs(CGAL_PolylineUtil::closest_distance(p0, input_polyline_pairs[foundValue * 2 + 0], edge));
			//RhinoApp().Print(L"Element %i Closest Distance 0: %f \n", foundValue,  closest_distance);
			double flag = closest_distance < GlobalToleranceSquare * 100;
			if (flag) {
				input_insertion_vectors[foundValue][edge + 2] = v;
				//RhinoApp().Print(L" Element %i edge %i \n", foundValue, edge+2);
				collision_count++;
				return true;
			}

			edge = 0;
			closest_distance = std::abs(CGAL_PolylineUtil::closest_distance(p1, input_polyline_pairs[foundValue * 2 + 1], edge));
			//RhinoApp().Print(L"Element %i Closest Distance 0: %f \n", foundValue,  closest_distance);
			flag = closest_distance < GlobalToleranceSquare * 100;
			if (flag) {
				input_insertion_vectors[foundValue][edge + 2] = v;
				//RhinoApp().Print(L" Element %i edge %i \n", foundValue, edge+2);
				collision_count++;
				return true;
			}




			return false;
		};

		double min[3] = { insertion_lines[i].from.x-tol, insertion_lines[i].from.y - tol, insertion_lines[i].from.z - tol };
		double max[3] = { insertion_lines[i].to.x+tol, insertion_lines[i].to.y + tol, insertion_lines[i].to.z + tol };
		int nhits = tree.Search(min, max, callback);//callback in this method call callback above

	}

	RhinoApp().Print(L" found insertion vectors: %i \n", collision_count);
	if (collision_count == 0) input_insertion_vectors.clear();
#pragma endregion

#pragma region JointTypes
	//////////////////////////////////////////////////////////////////////////////
//Get Joint Types finding textdots
//////////////////////////////////////////////////////////////////////////////
	collision_count = 0;
	UNREFERENCED_PARAMETER(context);

	CGetTextDotObject go3;
	go3.SetCommandPrompt(L"Select text dots");
	go3.EnablePreSelect(FALSE);
	go3.EnableDeselectAllBeforePostSelect(FALSE);
	go3.GetObjects(1, 0);


	//	return go3.CommandResult();


	if (go3.CommandResult() == CRhinoCommand::success) {
		const int object_count = go3.ObjectCount();

		std::vector< std::pair<ON_3dPoint, int> > text_dots;
		text_dots.reserve(object_count);

		int i;
		for (i = 0; i < object_count; i++) {
			const ON_TextDot* p = ON_TextDot::Cast(go3.Object(i).Geometry());
			if (p) {
				ON_wString sPoint;
				RhinoFormatPoint(p->CenterPoint(), sPoint);

				auto text = p->PrimaryText();
				int value = (int)_wtof(text);
				text_dots.emplace_back(p->CenterPoint(), value);//0 insertion possible in that case joint is skipped

				//RhinoApp().Print(text);
				//RhinoApp().Print(L"TextDot%d: point = (%ls), text = \"%ls\"\n", i, static_cast<const wchar_t*>(sPoint), p->PrimaryText());
				//RhinoApp().Print(L"%i ", value);
			}
		}
		//RhinoApp().Print(L" \n");


		
		for (int i = 0; i < text_dots.size(); i++) {//AABB.size()

			//RhinoApp().Print(L" x %f y %f z %f \n", text_dots[i].first.x, text_dots[i].first.y, text_dots[i].first.z);
			//std::vector<int> result;
			auto callback = [i, &text_dots, &input_polyline_pairs, &input_joint_types, &collision_count,  & context](int foundValue) -> bool
			{
				//RhinoApp().Print(L" __________________ \n");
				//Get lines points
				IK::Point_3 p(text_dots[i].first.x, text_dots[i].first.y, text_dots[i].first.z);
				//RhinoApp().Print(L" x %f y %f z %f \n", p.hx(), p.hy(), p.hz());

				//Check the distance between top and bottom outlines edge
				int edge = 0;
				double closest_distance = std::abs(CGAL_PolylineUtil::closest_distance(p, input_polyline_pairs[foundValue * 2 + 0], edge));
				//RhinoApp().Print(L"Element %i Closest Distance 0: %f \n", foundValue,  closest_distance);
				double flag = closest_distance < GlobalToleranceSquare * 100;
				if (flag) {
					int face_or_edge = text_dots[i].second < 0 ? 0 : 2 + edge;
					int type = text_dots[i].second < -100 ? 0 : std::abs(text_dots[i].second);
					input_joint_types[foundValue][face_or_edge] = type;
					//RhinoApp().Print(L" Element %i edge %i \n", foundValue, edge);
					//RhinoApp().Print(L" Closest Distance 0: %f \n", closest_distance);
					collision_count++;
					return true;
				}
				





				 closest_distance = std::abs(CGAL_PolylineUtil::closest_distance(p, input_polyline_pairs[foundValue * 2 + 1], edge));
				 //RhinoApp().Print(L" Element %i Closest Distance 0: %f \n", foundValue, closest_distance);
				 flag = closest_distance < GlobalToleranceSquare * 100;
				if (flag) {
					int face_or_edge = text_dots[i].second < 0 ? 1 : 2 + edge;
					int type = text_dots[i].second < -100 ? 0 : std::abs(text_dots[i].second);
					input_joint_types[foundValue][face_or_edge] = std::abs(type);
					//RhinoApp().Print(L" Element %i edge %i \n", foundValue, edge);
					//RhinoApp().Print(L" Closest Distance 1: %f \n", closest_distance);
					collision_count++;
					return true;
				}
				
			
				
				//ON_Polyline pline0;
				//for (auto& p : input_polyline_pairs[foundValue * 2 + 0])
				//	pline0.Append(ON_3dPoint(p.hx(), p.hy(), p.hz()));
				//ON_Polyline pline1;
				//for (auto& p : input_polyline_pairs[foundValue * 2 + 1])
				//	pline1.Append(ON_3dPoint(p.hx(), p.hy(), p.hz()));
				//CRhinoCurveObject* curve_object0 = context.m_doc.AddCurveObject(pline0);
				//CRhinoCurveObject* curve_object1 = context.m_doc.AddCurveObject(pline1);
				//RhinoApp().Print(L" __________________ \n");

				return false;
			};

			double min[3] = { text_dots[i].first.x- tol, text_dots[i].first.y - tol, text_dots[i].first.z - tol };
			double max[3] = { text_dots[i].first.x + tol, text_dots[i].first.y + tol, text_dots[i].first.z + tol };
			int nhits = tree.Search(min, max, callback);//callback in this method call callback above
			//RhinoApp().Print(L" ________%i__________ \n", nhits);

		}


	}
	RhinoApp().Print(L" Joint Types Collision count: %i \n", collision_count);
	if (collision_count == 0) input_joint_types.clear();
	//RhinoApp().Print(L" Joint Type vector count: %i \n", input_joint_types.size());



#pragma endregion

#pragma region GlobalParameters
	/////////////////////////////////////////////////////////////////////
//Command Line Menu search_type | division_distance | shift
/////////////////////////////////////////////////////////////////////
	search_type = 2;
	division_distance = 1000;
	shift = 0.5;
	output_type = 4;


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

	for (int i = 0; i < joint_parameters.size(); i += 3) {
		joint_parameters[i] = division_distance;
		joint_parameters[i+1] = shift;
	}

#pragma endregion

#pragma region LocalJointParameters

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

		ids[3] = menu_joint_parameters.AddCommandOptionNumber(RHCMDOPTNAME(L"SSO_Dist"), &joint_parameters[3], nullptr, FALSE, 0.001, 100000.0);
		ids[4] = menu_joint_parameters.AddCommandOptionNumber(RHCMDOPTNAME(L"SSO_Shift"), &joint_parameters[4], nullptr, FALSE, 0.0, 1.0);
		ids[5] = menu_joint_parameters.AddCommandOptionNumber(RHCMDOPTNAME(L"SSO_Type"), &joint_parameters[5], nullptr, FALSE, 10, 19);

		ids[6] = menu_joint_parameters.AddCommandOptionNumber(RHCMDOPTNAME(L"TS_Dist"), &joint_parameters[6], nullptr, FALSE, 0.001, 100000.0);
		ids[7] = menu_joint_parameters.AddCommandOptionNumber(RHCMDOPTNAME(L"TS_Shift"), &joint_parameters[7], nullptr, FALSE, 0.0, 1.0);
		ids[8] = menu_joint_parameters.AddCommandOptionNumber(RHCMDOPTNAME(L"TS_Type"), &joint_parameters[8], nullptr, FALSE, 20, 29);

		ids[9] = menu_joint_parameters.AddCommandOptionNumber(RHCMDOPTNAME(L"C_Dist"), &joint_parameters[9], nullptr, FALSE, 0.001, 100000.0);
		ids[10] = menu_joint_parameters.AddCommandOptionNumber(RHCMDOPTNAME(L"C_Shift"), &joint_parameters[10], nullptr, FALSE, 0.0, 1.0);
		ids[11] = menu_joint_parameters.AddCommandOptionNumber(RHCMDOPTNAME(L"C_Type"), &joint_parameters[11], nullptr, FALSE, 30, 39);

		ids[12] = menu_joint_parameters.AddCommandOptionNumber(RHCMDOPTNAME(L"TT_Dist"), &joint_parameters[12], nullptr, FALSE, 0.001, 100000.0);
		ids[13] = menu_joint_parameters.AddCommandOptionNumber(RHCMDOPTNAME(L"TT_Shift"), &joint_parameters[13], nullptr, FALSE, 0.0, 1.0);
		ids[14] = menu_joint_parameters.AddCommandOptionNumber(RHCMDOPTNAME(L"TT_Type"), &joint_parameters[14], nullptr, FALSE, 40, 49);

		ids[15] = menu_joint_parameters.AddCommandOptionNumber(RHCMDOPTNAME(L"SSR_Dist"), &joint_parameters[15], nullptr, FALSE, 0.001, 100000.0);
		ids[16] = menu_joint_parameters.AddCommandOptionNumber(RHCMDOPTNAME(L"SSR_Shift"), &joint_parameters[16], nullptr, FALSE, 0.0, 1.0);
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
#pragma endregion



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
	std::vector<double> joint_parameters = { 1000, 0.5, 1,  1000, 0.5, 10 ,  1000, 0.5, 20 ,  1000, 0.5, 30 ,  1000, 0.5, 40 ,  1000, 0.5, 50 };
	std::vector<std::vector<IK::Vector_3>> input_insertion_vectors;
	std::vector<std::vector<int>> input_joint_types;
	if (!UI(context, input_polyline_pairs, search_type, division_distance, shift, output_type, joint_parameters, input_insertion_vectors, input_joint_types)) return CRhinoCommand::failure;


	/////////////////////////////////////////////////////////////////////
	//Main Method
	/////////////////////////////////////////////////////////////////////
	RhinoApp().Print("============================================================================== CPP + \n");
	auto start = std::chrono::high_resolution_clock::now();
	//input
	//std::vector<std::vector<IK::Vector_3>> input_insertion_vectors;
	//std::vector<std::vector<int>> input_joint_types;
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

