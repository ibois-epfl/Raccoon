#include "compas.h"
#include "connection_zones.h"

std::vector<compas::RowMatrixXd> get_connection_zones(
	Eigen::Ref<const compas::RowMatrixXd>& polylines_vertices_XYZ,
	Eigen::Ref<const compas::RowMatrixXi>& polylines_vertices_count_int,
	Eigen::Ref<const compas::RowMatrixXd>& face_vectors_XYZ,
	Eigen::Ref<const compas::RowMatrixXi>& face_joints_types_int,
	Eigen::Ref<const compas::RowMatrixXi>& three_valence_element_indices_and_instruction
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
	

	std::vector<CGAL_Polyline> plines;
	const int n = out_polyline_pairs.size() * 0.5;
	int search_type = 0;//Implement
	bool show_plane_normals = true;

	//////////////////////////////////////////////////////////////////////////////
	//Create elements, AABB, OBB, P, Pls, thickness
	//////////////////////////////////////////////////////////////////////////////
	std::vector<element> elements;
	get_elements(out_polyline_pairs, out_insertion_vectors, out_joint_types, elements );

	//////////////////////////////////////////////////////////////////////////////
	//Create joints, Perform Joint Area Search
	//////////////////////////////////////////////////////////////////////////////
	
	auto joints = std::vector<joint>();
	auto joints_map = std::unordered_map<uint64_t, int>();
	rtree_search(elements, search_type, joints, joints_map);

	//////////////////////////////////////////////////////////////////////////////
	//Create and Align Joints
	//////////////////////////////////////////////////////////////////////////////
	three_valence_joint_alignment(
		out_three_valence_element_indices_and_instruction,
		elements,
		joints,
		joints_map,
		plines);


	for (int i = 0; i < joints.size(); i++) {
		//Cross_Simple(joints[i]);



		switch (joints[i].type) {

		case(12):
			//if (joints[i].type == 12)
				//joint_library::ss_e_ip_0(joints[i]);
			break;

		case(11):

	

			//joint types are given by user
			if (elements[joints[i].f0].joint_types.size() > 0) {

				int id0 = elements[joints[i].f0].joint_types[joints[i].e0];
				int id1 = elements[joints[i].f1].joint_types[joints[i].e1];
				//CGAL_Debug(id0, id1);

				if (id0 > 9 && id0 < 20)
					joint_library::construct_joint_by_index(joints[i], id0);
				else if (id1 > 9 && id1 < 20)
					joint_library::construct_joint_by_index(joints[i], id1);

			}
			else
				joint_library::ts_e_p_0(joints[i]);//default option

			break;

		case(20):



			//joint types are given by user
			if (elements[joints[i].f0].joint_types.size() > 0) {

				int id0 = elements[joints[i].f0].joint_types[joints[i].e0];
				int id1 = elements[joints[i].f1].joint_types[joints[i].e1];
				//CGAL_Debug(id0, id1);

				if (id0 > 19 && id0 < 30) 
					joint_library::construct_joint_by_index(joints[i], id0);
				else if (id1 > 19 && id1 < 30) 
					joint_library::construct_joint_by_index(joints[i], id1);

			}else 
				joint_library::ts_e_p_0(joints[i]);//default option
			

			
	

			break;

		}



	}
	//CGAL_Debug(joints.size());
	//////////////////////////////////////////////////////////////////////////////
	//Iterate joint address
	//////////////////////////////////////////////////////////////////////////////

	plines.reserve(elements.size() * 4);
	for (int i = 0; i < elements.size(); i++) {//takes 30-50 ms just to copy past polyline geometry

		//elements[i].get_joints_geometry(joints, plines, 0);
		//elements[i].get_joints_geometry(joints, plines,1);
		//elements[i].get_joints_geometry(joints, plines, 2);//push joint geometry from joint to element
		//elements[i].get_joints_geometry(joints, plines, 3);
			elements[i].get_joints_geometry_as_closed_polylines(joints, plines);

	}

	//////////////////////////////////////////////////////////////////////////////
	//Display Normals
	//////////////////////////////////////////////////////////////////////////////
	if (show_plane_normals) {
		for (int i = 1; i < elements.size(); i++) {//Pls.size()
			for (int j = 0; j < elements[i].planes.size(); j++) {//
				auto center = CGAL_PolylineUtil::Center(elements[i].polylines[j]);

				if(elements[i].edge_vectors.size()>0)
					plines.push_back({ center,center + elements[i].edge_vectors[j] });
			   // auto planeDisplay = CGAL_PlaneUtil::PlaneToLine(Pls[i][j].point(), Pls[i][j], 10, 10, 10);
			   //auto planeDisplay = CGAL_PlaneUtil::PlaneToLine(CGAL_PolylineUtil::Center(elements[i].polylines[j]), elements[i].planes[j], 10, 10, 10);
			   //plines.push_back(planeDisplay);
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////////
	//Convert Output to Raw Data
	//////////////////////////////////////////////////////////////////////////////
	return compas::result_from_polylinesVector(plines);

}




void init_connectionzones(pybind11::module& m) {
	pybind11::module submodule = m.def_submodule("connectionzonesCGAL");

	submodule.def(
		"get_connection_zones",
		&get_connection_zones,
		pybind11::arg("V").noconvert(),
		pybind11::arg("F").noconvert(),
		pybind11::arg("D").noconvert(),
		pybind11::arg("J").noconvert(),
		pybind11::arg("X").noconvert()
	);
}