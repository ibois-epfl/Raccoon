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
	bool element::intersection_closed_and_open_paths_2D(CGAL_Polyline& closed_pline_cutter, CGAL_Polyline& pline_to_cut, IK::Plane_3& plane, CGAL_Polyline& c);
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
					output[this->id].emplace_back(joints[std::get<0>(j_mf[i][j])].joint_area);
					break;
				case(1):
					output[this->id].emplace_back(joints[std::get<0>(j_mf[i][j])].joint_lines[0]);
					output[this->id].emplace_back(joints[std::get<0>(j_mf[i][j])].joint_lines[1]);
					break;
				case(2):
					output[this->id].emplace_back(joints[std::get<0>(j_mf[i][j])].joint_volumes[0]);
					output[this->id].emplace_back(joints[std::get<0>(j_mf[i][j])].joint_volumes[1]);
					output[this->id].emplace_back(joints[std::get<0>(j_mf[i][j])].joint_volumes[2]);
					output[this->id].emplace_back(joints[std::get<0>(j_mf[i][j])].joint_volumes[3]);
					break;
				case(3):
					output[this->id].emplace_back(this->polylines[0]);//cut
					output[this->id].emplace_back(this->polylines[1]);//cut
					for (int k = 0; k < joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), true).size(); k++) {
						output[this->id].emplace_back(joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), true)[k]);//cut
						output[this->id].emplace_back(joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), false)[k]);//direction
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

inline bool element::intersection_closed_and_open_paths_2D(CGAL_Polyline& closed_pline_cutter, CGAL_Polyline& pline_to_cut, IK::Plane_3& plane, CGAL_Polyline& c) {

	/////////////////////////////////////////////////////////////////////////////////////
	//Orient from 3D to 2D
	/////////////////////////////////////////////////////////////////////////////////////
	CGAL_Polyline a;
	CGAL_Polyline b;
	CGAL_PolylineUtil::Duplicate(pline_to_cut, a);
	CGAL_PolylineUtil::Duplicate(closed_pline_cutter, b);

	/////////////////////////////////////////////////////////////////////////////////////
	//Create Transformation
	/////////////////////////////////////////////////////////////////////////////////////
	CGAL::Aff_transformation_3<IK> xform_toXY = CGAL_XFormUtil::PlaneToXY(b[0], plane);
	CGAL::Aff_transformation_3<IK> xform_toXY_Inv = xform_toXY.inverse();
	CGAL_PolylineUtil::Transform(a, xform_toXY);
	CGAL_PolylineUtil::Transform(b, xform_toXY);

	/////////////////////////////////////////////////////////////////////////////////////
	//Find Max Coordinate to get Scale factor
	/////////////////////////////////////////////////////////////////////////////////////

	double max_coordinate = 0;
	for (int i = 0; i < a.size() - 1; i++) {
		if (max_coordinate < std::abs(a[i].hx()))
			max_coordinate = std::abs(a[i].hx());

		if (max_coordinate < std::abs(a[i].hy()))
			max_coordinate = std::abs(a[i].hy());
	}

	for (int i = 0; i < b.size() - 1; i++) {
		if (max_coordinate < std::abs(b[i].hx()))
			max_coordinate = std::abs(b[i].hx());

		if (max_coordinate < std::abs(a[i].hy()))
			max_coordinate = std::abs(b[i].hy());
	}

	double scale = std::pow(10, 17 - CGAL_MathUtil::count_digits(max_coordinate));
	CGAL_Debug(scale);
	/////////////////////////////////////////////////////////////////////////////////////
	//Convert to Clipper
	/////////////////////////////////////////////////////////////////////////////////////
	std::vector< ClipperLib::IntPoint > pathA(a.size());
	std::vector< ClipperLib::IntPoint > pathB(b.size() - 1);

	for (int i = 0; i < a.size() ; i++) {
		pathA[i] = ClipperLib::IntPoint(a[i].x() * scale, a[i].y() * scale);
		printf("%f,%f,%f \n", a[i].x(), a[i].y(), a[i].z());
	}
	//printf("\n");
	for (int i = 0; i < b.size() - 1; i++) {
		pathB[i] = ClipperLib::IntPoint(b[i].x() * scale, b[i].y() * scale);
		printf("%f,%f,%f \n", b[i].x(), b[i].y(), b[i].z());
	}



	ClipperLib::Clipper clipper;
	
	clipper.AddPath(pathA, ClipperLib::ptSubject, false);
	clipper.AddPath(pathB, ClipperLib::ptClip, true);
	//ClipperLib::Paths C;
	ClipperLib::PolyTree C;
	clipper.Execute(ClipperLib::ctIntersection, C, ClipperLib::pftNonZero, ClipperLib::pftNonZero);


	CGAL_Debug(C.ChildCount());
	if (C.ChildCount() > 0) {

		//Calculate number of points
		int count = 0;
		ClipperLib::PolyNode* polynode = C.GetFirst();
		while (polynode) {
			//do stuff with polynode here
			if (polynode->Contour.size() <= 1)
				continue;
			count += polynode->Contour.size();
			polynode = polynode->GetNext();
		}
		c.reserve(count);


		polynode = C.GetFirst();
		count = 0;
		while (polynode) {
			//do stuff with polynode here
			if (polynode->Contour.size() <= 1)
				continue;

			//Check if seam is correctly placed
			for (size_t j = 0; j < polynode->Contour.size(); j++)
				c.emplace_back(polynode->Contour[j].X / scale, polynode->Contour[j].Y / scale, 0);
		
			polynode = polynode->GetNext();
			count++;
		}
	


		//Transform to 3D space
		for (int i = 0; i < c.size(); i++)
			c[i] = c[i].transform(xform_toXY_Inv);


	} else {
		return false;
	}

	/////////////////////////////////////////////////////////////////////////////////////
	//Output
	/////////////////////////////////////////////////////////////////////////////////////
	return true;

}

inline void element::get_joints_geometry_as_closed_polylines_performing_intersection(std::vector<joint>& joints, std::vector <std::vector <CGAL_Polyline>>& output) {


	CGAL_Polyline pline0 = this->polylines[0];
	CGAL_Polyline pline1 = this->polylines[1];


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

					int eA, eB;
					joints[std::get<0>(j_mf[i][j])].get_edge_ids(std::get<1>(j_mf[i][j]), eA, eB);
					if (false) {//split by line, in this case you need to know which side is inside

					} else {//split by full polygon 
						CGAL_Polyline joint_pline_0;
						intersection_closed_and_open_paths_2D(pline0, joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), true).front(), this->planes[0], joint_pline_0);
						output[this->id].emplace_back(joint_pline_0);
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
