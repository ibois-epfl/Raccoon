#pragma once
#include <string>
#include "compas.h"
#include "joint.h"
#include "CGAL_IntersectionUtil.h"
#include "CGAL_XFormUtil.h"
#include "CGAL_PolylineUtil.h"
#include "CGAL_Print.h"
class element {
public:

	/////////////////////////////////////////////////////////////////////////////////////////
	//Parameters
	/////////////////////////////////////////////////////////////////////////////////////////
	int id = -1;
	double thickness = 0;

	/////////////////////////////////////////////////////////////////////////////////////////
	//Basic Geometry Parameters 
	/////////////////////////////////////////////////////////////////////////////////////////
	CGAL::Bbox_3 aabb;
	IK::Vector_3 oob[5];
	std::vector<CGAL_Polyline> polylines; //pairs of polylines + each side face
	std::vector<IK::Plane_3> planes; //top and bottom planes + each side face

	std::vector<IK::Vector_3> edge_vectors;//if set
	std::vector<int> joint_types;//if set
	IK::Segment_3 axis;

	/////////////////////////////////////////////////////////////////////////////////////////
	//Joinery Geometry Parameters 
	//pointers to joints, joint geometry will be modified inside joint to avoid copy
	//joint must be sorted according to edge id to be merged
	/////////////////////////////////////////////////////////////////////////////////////////
	//std::map<int, std::pair<bool, int>>edgeID_mf_jointID; //e0,true,jointID
	std::vector<std::vector<std::tuple<int, bool, double>>> j_mf; //(joint id, male/female, parameter on edge) elements[result[i + 0]].j_mf[e0].push_back(std::tuple<int, bool, double>(jointID, true,0));
	//std::vector<CGAL_Polyline> modified_polylines;
	//public Mesh mesh;

	/////////////////////////////////////////////////////////////////////////////////////////
	//Component parameters
	/////////////////////////////////////////////////////////////////////////////////////////
	std::string key;
	IK::Plane_3 component_plane;


	/////////////////////////////////////////////////////////////////////////////////////////
	//Constructors
	/////////////////////////////////////////////////////////////////////////////////////////
	element();
	element(int);

	void get_joints_geometry(std::vector<joint>& joints, std::vector <std::vector <CGAL_Polyline>>& output, int what_to_expose);
	void element::get_joints_geometry_as_closed_polylines_replacing_edges(std::vector<joint>& joints, std::vector <std::vector <CGAL_Polyline>>& output);
	void element::get_joints_geometry_as_closed_polylines_performing_intersection(std::vector<joint>& joints, std::vector <std::vector <CGAL_Polyline>>& output);
};


inline element::element() {}


inline element::element(int _id) : id(_id) {

}

inline void element::get_joints_geometry(std::vector<joint>& joints, std::vector <std::vector <CGAL_Polyline>>& output, int what_to_expose) {

	//you are in a loop
	for (int i = 0; i < j_mf.size(); i++) {//loop joint id 
		for (size_t j = 0; j < j_mf[i].size(); j++) {//loop joints per each face + 1 undefined
			switch (what_to_expose) {
				case(0):
					output[this->id].push_back(joints[std::get<0>(j_mf[i][j])].joint_area);
					break;
				case(1):
					output[this->id].push_back(joints[std::get<0>(j_mf[i][j])].joint_lines[0]);
					output[this->id].push_back(joints[std::get<0>(j_mf[i][j])].joint_lines[1]);
					break;
				case(2):
					output[this->id].push_back(joints[std::get<0>(j_mf[i][j])].joint_volumes[0]);
					output[this->id].push_back(joints[std::get<0>(j_mf[i][j])].joint_volumes[1]);
					output[this->id].push_back(joints[std::get<0>(j_mf[i][j])].joint_volumes[2]);
					output[this->id].push_back(joints[std::get<0>(j_mf[i][j])].joint_volumes[3]);
					break;
				case(3):
					for (int k = 0; k < joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), true).size(); k++) {
						output[this->id].push_back(joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), true)[k]);//cut
						output[this->id].push_back(joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), false)[k]);//direction
					}
					break;
				default:
					break;
			}
		}
	}
}

inline bool sort_by_third(const std::tuple<int, bool, double>& a, const std::tuple<int, bool, double>& b) {
	return (std::get<2>(a) < std::get<2>(b));
}

inline void element::get_joints_geometry_as_closed_polylines_replacing_edges(std::vector<joint>& joints, std::vector <std::vector <CGAL_Polyline>>& output) {



	//you are in a loop

	///////////////////////////////////////////////////////////////////////////////
	//Copy top and bottom polylines
	///////////////////////////////////////////////////////////////////////////////
	CGAL_Polyline polyline0;
	CGAL_Polyline polyline1;
	CGAL_PolylineUtil::Duplicate(polylines[0], polyline0);
	CGAL_PolylineUtil::Duplicate(polylines[1], polyline1);
	int n = polyline0.size() - 1;
	bool debug = false;
	if (debug) CGAL_Debug(-4);

	///////////////////////////////////////////////////////////////////////////////
	//Sorts joints by edges
	//Get closest parameter to edge and sort by this values
	///////////////////////////////////////////////////////////////////////////////
	for (int i = 0; i < n; i++) {//iterate edge count

		for (int j = 0; j < this->j_mf[i + 2].size(); j++) { //iterate edges, skipping first two

			if (joints[std::get<0>(j_mf[i + 2][j])].name == "" || joints[std::get<0>(j_mf[i + 2][j])].name == "cr_c_ip_0") continue;

			IK::Segment_3 element_edge(polyline0[i], polyline0[i + 1]);
			IK::Point_3 joint_point = joints[std::get<0>(j_mf[i + 2][j])](std::get<1>(j_mf[i + 2][j]), true).back()[0];
			double t;
			CGAL_IntersectionUtil::ClosestPointTo(joint_point, element_edge, t);
			std::get<2>(j_mf[i + 2][j]) = t;
		}

		std::sort(j_mf[i + 2].begin(), j_mf[i + 2].end(), sort_by_third);
	}


	//CGAL_Debug(polylines[0][0], true);
	//CGAL_Debug(polylines[0][1], true);




	///////////////////////////////////////////////////////////////////////////////
	//Perform Intersection, skip first two elements because they are top and bottom
	///////////////////////////////////////////////////////////////////////////////
	for (int i = 0; i < n; i++) {//iterate sides only

		for (int j = 0; j < this->j_mf[i + 2].size(); j++) {//

			if (j_mf[i + 2].size() > 0) {//only if one line can be inserted


				///////////////////////////////////////////////////////////////////////////////
				//Skip undefined names and if tiles has more than 1 polyline to insert
				///////////////////////////////////////////////////////////////////////////////
				if (joints[std::get<0>(j_mf[i + 2][j])].name == "" || joints[std::get<0>(j_mf[i + 2][j])].name == "cr_c_ip_0") continue;
				if (joints[std::get<0>(j_mf[i + 2][j])](std::get<1>(j_mf[i + 2][j]), true).size() != 2) continue;


				///////////////////////////////////////////////////////////////////////////////
				//Take last lines Element outline can be not in the same order as joint outlines Take joint segment and measure its point to the plane
				///////////////////////////////////////////////////////////////////////////////
				bool flag =
					CGAL::squared_distance(planes[0].projection(joints[std::get<0>(j_mf[i + 2][j])](std::get<1>(j_mf[i + 2][j]), true).back()[0]), joints[std::get<0>(j_mf[i + 2][j])](std::get<1>(j_mf[i + 2][j]), true).back()[0]) >
					CGAL::squared_distance(planes[0].projection(joints[std::get<0>(j_mf[i + 2][j])](std::get<1>(j_mf[i + 2][j]), false).back()[0]), joints[std::get<0>(j_mf[i + 2][j])](std::get<1>(j_mf[i + 2][j]), false).back()[0])
					;

				if (flag)
					joints[std::get<0>(j_mf[i + 2][j])].reverse(std::get<1>(j_mf[i + 2][j]));


				IK::Segment_3 joint_line_0(joints[std::get<0>(j_mf[i + 2][j])](std::get<1>(j_mf[i + 2][j]), true).back()[0], joints[std::get<0>(j_mf[i + 2][j])](std::get<1>(j_mf[i + 2][j]), true).back()[1]);
				IK::Segment_3 joint_line_1(joints[std::get<0>(j_mf[i + 2][j])](std::get<1>(j_mf[i + 2][j]), false).back()[0], joints[std::get<0>(j_mf[i + 2][j])](std::get<1>(j_mf[i + 2][j]), false).back()[1]);

				///////////////////////////////////////////////////////////////////////////////
				//Take last lines
				///////////////////////////////////////////////////////////////////////////////
				int prev = (((i - 1) % n) + n) % n;
				int next = (((i + 1) % n) + n) % n;
				int nextnext = (((i + 2) % n) + n) % n;

				IK::Segment_3 next_segment_0(polyline0[prev], polyline0[i]);
				IK::Segment_3 prev_segment_0(polyline0[next], polyline0[nextnext]);
				IK::Segment_3 next_segment_1(polyline1[prev], polyline1[i]);
				IK::Segment_3 prev_segment_1(polyline1[next], polyline1[nextnext]);
				if (debug) CGAL_Debug(4);

				///////////////////////////////////////////////////////////////////////////////
				//Intersect them with side lines, same principle must work on both polygons
				///////////////////////////////////////////////////////////////////////////////
				//1 Perform intersection line-line (needs implementation from rhino)
				//3 If intersecting relocate joint line points --|*---------*|--, if not overlaping do not change |  *-----*  |.
				IK::Point_3 p0_int, p1_int, p2_int, p3_int;
				double t0_int, t1_int, t2_int, t3_int;

				IK::Plane_3 joint_line_plane_0_prev(joint_line_0[0], planes[2 + i].orthogonal_vector());
				IK::Plane_3 joint_line_plane_0_next(joint_line_0[0], planes[2 + i].orthogonal_vector());
				IK::Plane_3 joint_line_plane_1_prev(joint_line_1[0], planes[2 + i].orthogonal_vector());
				IK::Plane_3 joint_line_plane_1_next(joint_line_1[0], planes[2 + i].orthogonal_vector());


				IK::Vector_3 v(0, 0, 2);


				bool flag0 = CGAL_IntersectionUtil::plane_plane_plane(planes[2 + prev], joint_line_plane_0_prev, planes[0], p0_int, joint_line_0, t0_int);

				//output.push_back({ p0_int + v,joint_line_0[0]+ v,CGAL_PolylineUtil::Center(polylines[2 + prev]) + v,CGAL_PolylineUtil::Center(polylines[0]) + v });

				bool flag1 = CGAL_IntersectionUtil::plane_plane_plane(planes[2 + next], joint_line_plane_0_next, planes[0], p1_int, joint_line_0, t1_int);
				//output.push_back({ p1_int + v ,joint_line_0[0] + v,CGAL_PolylineUtil::Center(polylines[2 + next]) + v,CGAL_PolylineUtil::Center(polylines[0]) + v });

				bool flag2 = CGAL_IntersectionUtil::plane_plane_plane(planes[2 + prev], joint_line_plane_1_prev, planes[1], p2_int, joint_line_1, t2_int);
				//output.push_back({ p2_int + v,joint_line_1[0] + v,CGAL_PolylineUtil::Center(polylines[2 + prev]) + v,CGAL_PolylineUtil::Center(polylines[1]) + v });

				bool flag3 = CGAL_IntersectionUtil::plane_plane_plane(planes[2 + next], joint_line_plane_1_next, planes[1], p3_int, joint_line_1, t3_int);
				//output.push_back({ p3_int + v,joint_line_1[0] + v,CGAL_PolylineUtil::Center(polylines[2 + next]) + v,CGAL_PolylineUtil::Center(polylines[1]) + v });


				///////////////////////////////////////////////////////////////////////////////
				//2 Relocate side segments points to intersection points
				///////////////////////////////////////////////////////////////////////////////


				if (flag0) polyline0[i] = p0_int;
				if (flag1) polyline0[next] = p1_int;


				if (flag2) polyline1[i] = p2_int;
				if (flag3) polyline1[next] = p3_int;

				///////////////////////////////////////////////////////////////////////////////
				//3 Check orientation of joint outlines, if needed flip
				///////////////////////////////////////////////////////////////////////////////
				bool flip = CGAL::has_smaller_distance_to_point(
					polyline0[i],
					joints[std::get<0>(j_mf[i + 2][j])](std::get<1>(j_mf[i + 2][j]), !flag)[0].front(),
					joints[std::get<0>(j_mf[i + 2][j])](std::get<1>(j_mf[i + 2][j]), !flag)[0].back()
				);
				;
				if (!flip) {

					//bottom
					std::reverse(
						joints[std::get<0>(j_mf[i + 2][j])](std::get<1>(j_mf[i + 2][j]), !flag)[0].begin(),
						joints[std::get<0>(j_mf[i + 2][j])](std::get<1>(j_mf[i + 2][j]), !flag)[0].end()
					);

					//top
					std::reverse(
						joints[std::get<0>(j_mf[i + 2][j])](std::get<1>(j_mf[i + 2][j]), flag)[0].begin(),
						joints[std::get<0>(j_mf[i + 2][j])](std::get<1>(j_mf[i + 2][j]), flag)[0].end()
					);
				}


			}//if joint edge
		}//for j
	} //for i


	//Update the end
	polyline0[polyline0.size() - 1] = polyline0[0];
	polyline1[polyline1.size() - 1] = polyline1[0];




	///////////////////////////////////////////////////////////////////////////////
	//After intersection is ready merger all polylines
	///////////////////////////////////////////////////////////////////////////////
	CGAL_Polyline newPolyline0;
	CGAL_Polyline newPolyline1;

	for (int i = 0; i < n; i++) {//iterate sides only

		newPolyline0.push_back(polyline0[i]);
		newPolyline1.push_back(polyline1[i]);

		for (int j = 0; j < this->j_mf[i + 2].size(); j++) {//
			if (j_mf[i + 2].size() > 0) {
				if (joints[std::get<0>(j_mf[i + 2][j])].name == "" || joints[std::get<0>(j_mf[i + 2][j])].name == "cr_c_ip_0") continue;
				if (joints[std::get<0>(j_mf[i + 2][j])](std::get<1>(j_mf[i + 2][j]), true).size() == 2) {
					for (auto pp : joints[std::get<0>(j_mf[i + 2][j])](std::get<1>(j_mf[i + 2][j]), true)[0]) newPolyline0.push_back(pp);
					for (auto pp : joints[std::get<0>(j_mf[i + 2][j])](std::get<1>(j_mf[i + 2][j]), false)[0]) newPolyline1.push_back(pp);
				}
			}
		}

		if (i == n - 1) {
			newPolyline0.push_back(newPolyline0[0]);
			newPolyline1.push_back(newPolyline1[0]);
			output[this->id].push_back(newPolyline0);
			output[this->id].push_back(newPolyline1);
		}
	}


	for (int i = 0; i < 2; i++) {//iterate top only



		for (size_t j = 0; j < j_mf[i].size(); j++) {

			if (joints[std::get<0>(j_mf[i][j])].name == "")continue;

			//Check hole position
			bool flag =
				CGAL::squared_distance(planes[0].projection(joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), true).back()[0]), joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), true).back()[0]) >
				CGAL::squared_distance(planes[0].projection(joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), false).back()[0]), joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), false).back()[0])
				;

			if (flag)
				joints[std::get<0>(j_mf[i][j])].reverse(std::get<1>(j_mf[i][j]));

			for (int k = 0; k < joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), true).size() - 1; k++) {
				output[this->id].push_back(joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), true)[k]);
				output[this->id].push_back(joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), false)[k]);
			}

		}


	}



	//close





	///////////////////////////////////////////////////////////////////////////////
	//output
	///////////////////////////////////////////////////////////////////////////////

	return;










}

inline void element::get_joints_geometry_as_closed_polylines_performing_intersection(std::vector<joint>& joints, std::vector <std::vector <CGAL_Polyline>>& output) {


	CGAL_Polyline pline0 = this->polylines[0];
	CGAL_Polyline pline1 = this->polylines[1];
	CGAL::Aff_transformation_3 xform0 = CGAL_XFormUtil::PlaneToXY(pline0[0], this->planes[0]);
	CGAL::Aff_transformation_3 xform1 = CGAL_XFormUtil::PlaneToXY(pline1[0], this->planes[1]);
	CGAL::Aff_transformation_3 xform0_inv = xform0.inverse();
	CGAL::Aff_transformation_3 xform1_inv = xform1.inverse();
	CGAL_PolylineUtil::Transform(pline0, xform0);
	CGAL_PolylineUtil::Transform(pline1, xform1);


	//you are in a loop
	//only for objects that has one element as a joint and edges to insert are known

	//CGAL_Debug(9999);
	for (int i = 2; i < this->j_mf.size(); i++) {
		//CGAL_Debug(this->j_mf[i].size());
		for (int j = 0; j < this->j_mf[i].size(); j++) {//

			

			///////////////////////////////////////////////////////////////////////////////
			//Skip undefined names and if tiles has more than 1 polyline to insert
			///////////////////////////////////////////////////////////////////////////////
			if (joints[std::get<0>(j_mf[i][j])].name == "" ) continue;
			if (joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), true).size() != 2) continue;

			///////////////////////////////////////////////////////////////////////////////
			//Take last lines Element outline can be not in the same order as joint outlines Take joint segment and measure its point to the plane
			///////////////////////////////////////////////////////////////////////////////
			bool flag =
				CGAL::squared_distance(planes[0].projection(joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), true).back()[0]), joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), true).back()[0]) >
				CGAL::squared_distance(planes[0].projection(joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), false).back()[0]), joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), false).back()[0])
				;

			///////////////////////////////////////////////////////////////////////////////
			//Intersect rectangle or line
			///////////////////////////////////////////////////////////////////////////////
			switch (joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), true).back().size()) {

				case(2)://one edge

					break;

				case(5):
				{//two edges
			//Intersect segment 0 with polyline

					//Orient to 2D for parameter retrieval
					CGAL_Polyline joint_pline0 = joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), true).back();
					CGAL_Polyline joint_pline1 = joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), false).back();
					CGAL_PolylineUtil::Transform(joint_pline0, xform0);
					CGAL_PolylineUtil::Transform(joint_pline1, xform1);

					//Perform 2D operations

					int eA, eB;
					joints[std::get<0>(j_mf[i][j])].get_edge_ids(std::get<1>(j_mf[i][j]), eA, eB);



					//Line-Segment -> Take first element polyline intersect with joint polyline, within all points take closes to the line
					//Do the same for the 2nd polygon, if needed
					IK::Line_3 l0(this->polylines[0][eA - 1], this->polylines[0][eA]);

					for (int k = 0; k < joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), true).back().size()-1; k++) {
						IK::Line_3 s0(joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), true).back()[k-1], joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), true).back()[k]);
						CGAL::intersection(l0, s0);
					}

					//
					//IK::Line_3 l1(this->polylines[0][eB - 1], this->polylines[0][eB]);

					//Find closest parameters for both parts

					//split polygon in to parts by poilygon parameters t0, t1
					//Sort them according to order and select points in between t0, next, next ... next until t1 is found


					if (eA != eB) {

					}
					break;
				}
				default:
					continue;
					break;
			}



			if (flag)
				joints[std::get<0>(j_mf[i][j])].reverse(std::get<1>(j_mf[i][j]));


			CGAL_Debug(
				joints[std::get<0>(j_mf[i][j])].e0_0,
				joints[std::get<0>(j_mf[i][j])].e0_1,
				joints[std::get<0>(j_mf[i][j])].e1_0,
				joints[std::get<0>(j_mf[i][j])].e1_1
				
				);
		}
	}

}
