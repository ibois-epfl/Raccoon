#include "stdafx.h"
#include "connection_zones.h"

void get_connection_zones_test() {
	//fsdfsdfsd
}

void get_connection_zones(
	std::vector<CGAL_Polyline>& input_polyline_pairs,
	std::vector<std::vector<IK::Vector_3>>& input_insertion_vectors,
	std::vector<std::vector<int>>& input_joint_types,
	std::vector<std::vector<int>>& input_three_valence_element_indices_and_instruction,

	//output
	std::vector<std::vector<CGAL_Polyline>>& plines,

	int search_type,
	double division_distance,
	double shift

) {


	const int n = input_polyline_pairs.size() * 0.5;

	//////////////////////////////////////////////////////////////////////////////
	//Create elements, AABB, OBB, P, Pls, thickness
	//////////////////////////////////////////////////////////////////////////////
	
	std::vector<element> elements;
	get_elements(input_polyline_pairs, input_insertion_vectors, input_joint_types, elements);
	
	//////////////////////////////////////////////////////////////////////////////
	//Create joints, Perform Joint Area Search
	//////////////////////////////////////////////////////////////////////////////



	auto joints = std::vector<joint>();
	auto joints_map = std::unordered_map<uint64_t, int>();
	rtree_search(elements, search_type, joints, joints_map);

	//if (true) {
	//	std::ofstream myfile;
	//	myfile.open("C:\\IBOIS57\\_Code\\Software\\Python\\Pybind11Example\\vsstudio\\Release\\output.txt");
	//	myfile << (elements.size()) << "\n";
	//	myfile << (joints.size()) << "\n";
	//	//myfile << (input_three_valence_element_indices_and_instruction.size()) << "\n";
	//	myfile.close();
	//}
	//return;

	//////////////////////////////////////////////////////////////////////////////
	//3-valence joints
	//////////////////////////////////////////////////////////////////////////////


	if (input_three_valence_element_indices_and_instruction.size() > 0)
		three_valence_joint_alignment(input_three_valence_element_indices_and_instruction, elements, joints, joints_map, division_distance);//plines, 
	
	////////////////////////////////////////////////////////////////////////////////
	////Create and Align Joints 1. Iterate type 2. Select joint based on not/given user joint_type
	////////////////////////////////////////////////////////////////////////////////

	joint_library::construct_joint_by_index(elements, joints, division_distance, shift);


	//////////////////////////////////////////////////////////////////////////////
	//Iterate joint address
	//////////////////////////////////////////////////////////////////////////////

	plines = std::vector<std::vector<CGAL_Polyline>>(elements.size());
	for (int i = 0; i < elements.size(); i++) {//takes 30-50 ms just to copy past polyline geometry

		//elements[i].get_joints_geometry(joints, plines, 0);
		//elements[i].get_joints_geometry(joints, plines, 1);
		//elements[i].get_joints_geometry(joints, plines, 2);
		elements[i].get_joints_geometry(joints, plines, 3);//push joint geometry from joint to element
		//elements[i].get_joints_geometry_as_closed_polylines_replacing_edges(joints, plines);
		elements[i].get_joints_geometry_as_closed_polylines_performing_intersection(joints, plines);

		

	}

	//////////////////////////////////////////////////////////////////////////////
	//Display Normals
	//////////////////////////////////////////////////////////////////////////////

	//if (false) {
	//	for (int i = 1; i < elements.size(); i++) {//Pls.size()
	//		for (int j = 0; j < elements[i].planes.size(); j++) {//
	//			auto center = CGAL_PolylineUtil::Center(elements[i].polylines[j]);

	//			if (elements[i].edge_vectors.size() > 0)
	//				plines.push_back({ center,center + elements[i].edge_vectors[j] });
	//			// auto planeDisplay = CGAL_PlaneUtil::PlaneToLine(Pls[i][j].point(), Pls[i][j], 10, 10, 10);
	//			//auto planeDisplay = CGAL_PlaneUtil::PlaneToLine(CGAL_PolylineUtil::Center(elements[i].polylines[j]), elements[i].planes[j], 10, 10, 10);
	//			//plines.push_back(planeDisplay);
	//		}
	//	}
	//}



}

std::vector<compas::RowMatrixXd> get_connection_zones_compas(
	Eigen::Ref<const compas::RowMatrixXd>& polylines_vertices_XYZ,
	Eigen::Ref<const compas::RowMatrixXi>& polylines_vertices_count_int,
	Eigen::Ref<const compas::RowMatrixXd>& face_vectors_XYZ,
	Eigen::Ref<const compas::RowMatrixXi>& face_joints_types_int,
	Eigen::Ref<const compas::RowMatrixXi>& three_valence_element_indices_and_instruction,

	int search_type,
	double division_distance,
	double shift
) {





	//////////////////////////////////////////////////////////////////////////////
	//Convert Raw data to list of Polyline
	//////////////////////////////////////////////////////////////////////////////
	//std::vector<element> elements;
	//std::vector<CGAL_Polyline> polyline_pairs = compas::polylines_from_vertices_and_faces(polylines_vertices_XYZ, polylines_vertices_count_int);
	std::vector<CGAL_Polyline> out_polyline_pairs;
	std::vector<std::vector<IK::Vector_3>> out_insertion_vectors;
	std::vector<std::vector<int>> out_joint_types;
	std::vector<std::vector<int>> out_three_valence_element_indices_and_instruction;

	compas::polylines_from_vertices_and_faces_and_properties(
		polylines_vertices_XYZ,
		polylines_vertices_count_int,
		face_vectors_XYZ,
		face_joints_types_int,
		three_valence_element_indices_and_instruction,
		out_polyline_pairs,
		out_insertion_vectors,
		out_joint_types,
		out_three_valence_element_indices_and_instruction
	);
	
	std::vector<std::vector<CGAL_Polyline>> output;
	get_connection_zones(
		out_polyline_pairs,
		out_insertion_vectors,
		out_joint_types,
		out_three_valence_element_indices_and_instruction,
		output,
		search_type,
		division_distance,
		shift
	);

	//////////////////////////////////////////////////////////////////////////////
//Convert Output to Raw Data
//////////////////////////////////////////////////////////////////////////////
	return compas::result_from_polylinesVector(output, true);
}



//
//void init_connectionzones(pybind11::module& m) {
//    pybind11::module submodule = m.def_submodule("connectionzonesCGAL");
//
//    submodule.def(
//        "get_connection_zones",
//        &get_connection_zones,
//        pybind11::arg("V").noconvert(),
//        pybind11::arg("F").noconvert(),
//        pybind11::arg("D").noconvert(),
//        pybind11::arg("J").noconvert(),
//        pybind11::arg("X").noconvert()
//    );
//}
