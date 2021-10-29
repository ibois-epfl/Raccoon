#include "compas.h"
#include "connection_zones.h"

std::vector<compas::RowMatrixXd> get_connection_zones(
	Eigen::Ref<const compas::RowMatrixXd>& V,
	Eigen::Ref<const compas::RowMatrixXi>& F
) {



	//////////////////////////////////////////////////////////////////////////////
	//Convert Raw data to list of Polyline
	//////////////////////////////////////////////////////////////////////////////
	std::vector<CGAL_Polyline> polyline_pairs = compas::polylines_from_vertices_and_faces(V, F);
	const int n = polyline_pairs.size() * 0.5;
	int search_type = 0;//Implement
	bool show_plane_normals = true;

	//////////////////////////////////////////////////////////////////////////////
	//Create elements, AABB, OBB, P, Pls, thickness
	//////////////////////////////////////////////////////////////////////////////
	std::vector<element> elements;
	get_elements(polyline_pairs, elements);

	//////////////////////////////////////////////////////////////////////////////
	//Create joints, Perform Joint Area Search
	//////////////////////////////////////////////////////////////////////////////
	auto joints = std::vector<joint>();
	rtree_search(elements, search_type, joints);

	//////////////////////////////////////////////////////////////////////////////
	//Create and Align Joints
	//////////////////////////////////////////////////////////////////////////////

	for (int i = 0; i < joints.size(); i++) {
		//Cross_Simple(joints[i]);

		switch (joints[i].type) {

		case(12):
			if (joints[i].type == 12)
				joint_library::ss_e_ip_0(joints[i]);
			break;

		case(11):
			if (joints[i].type == 11)
				joint_library::ss_e_op_0(joints[i]);
			break;

		}



	}
	//CGAL_Debug(joints.size());
	//////////////////////////////////////////////////////////////////////////////
	//Iterate joint address
	//////////////////////////////////////////////////////////////////////////////
	std::vector<CGAL_Polyline> plines;
	plines.reserve(elements.size() * 4);
	for (int i = 0; i < elements.size(); i++) {//takes 30-50 ms just to copy past polyline geometry

	  //elements[i].get_joints_geometry(joints, plines,3);//push joint geometry from joint to element
		elements[i].get_joints_geometry_as_closed_polylines(joints, plines);

	}

	//////////////////////////////////////////////////////////////////////////////
	//Display Normals
	//////////////////////////////////////////////////////////////////////////////
	if (show_plane_normals) {
		for (int i = 1; i < 2; i++) {//Pls.size()
			for (int j = 0; j < elements[i].planes.size(); j++) {//
			   // auto planeDisplay = CGAL_PlaneUtil::PlaneToLine(Pls[i][j].point(), Pls[i][j], 10, 10, 10);
			   // auto planeDisplay = CGAL_PlaneUtil::PlaneToLine(CGAL_PolylineUtil::Center(P[i][j]), Pls[i][j], 10, 10, 10);
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
		pybind11::arg("F").noconvert()
	);
}