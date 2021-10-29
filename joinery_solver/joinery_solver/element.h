#pragma once
#include <string>
#include "compas.h"
#include "joint.h"
#include "CGAL_IntersectionUtil.h"

class element
{
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
		IK::Segment_3 axis;

		/////////////////////////////////////////////////////////////////////////////////////////
		//Joinery Geometry Parameters 
		//pointers to joints, joint geometry will be modified inside joint to avoid copy
		//joint must be sorted according to edge id to be merged
		/////////////////////////////////////////////////////////////////////////////////////////
		//std::map<int, std::pair<bool, int>>edgeID_mf_jointID; //e0,true,jointID
		std::vector<std::pair<int,bool>> j_mf; //jointID_malefemale
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

		void get_joints_geometry(std::vector<joint>& joints, std::vector <CGAL_Polyline>& output, int what_to_expose);
		void element::get_joints_geometry_as_closed_polylines(std::vector<joint>& joints, std::vector <CGAL_Polyline>& output);
};


inline element::element() { }

//void element::get_joints_geometry(std::vector<CGAL_Polyline>)
//{
//}

inline element::element(int _id) {
	this->id = _id;
}

inline void element::get_joints_geometry(std::vector<joint>& joints, std::vector <CGAL_Polyline>& output, int what_to_expose) {

	for (int i = 0; i < polylines.size(); i++) {
		if (j_mf[i].first != -1) {

			switch (what_to_expose)
			{
			case(0):
				//if(joints[j_mf[i].first].f0==0 && joints[j_mf[i].first].f1==3)
					output.push_back(joints[j_mf[i].first].joint_area);
				break;
			case(1):
				output.push_back(joints[j_mf[i].first].joint_lines[0]);
				output.push_back(joints[j_mf[i].first].joint_lines[1]);
				break;
			case(2):
				output.push_back(joints[j_mf[i].first].joint_volumes[0]);
				output.push_back(joints[j_mf[i].first].joint_volumes[1]);

				output.push_back(joints[j_mf[i].first].joint_volumes[2]);
				output.push_back(joints[j_mf[i].first].joint_volumes[3]);

				break;
			case(3):
				output.insert(
					output.end(),
					joints[j_mf[i].first](j_mf[i].second, true).begin(),
					joints[j_mf[i].first](j_mf[i].second, true).end()
				);
				output.insert(
					output.end(),
					joints[j_mf[i].first](j_mf[i].second, false).begin(),
					joints[j_mf[i].first](j_mf[i].second, false).end()
				);
				break;
			default:
				break;
			}
		

		}
	}



}

inline void element::get_joints_geometry_as_closed_polylines(std::vector<joint>& joints, std::vector <CGAL_Polyline>& output) {
	//CGAL_Debug(polylines[0][0], true);
	//CGAL_Debug(polylines[0][1], true);

	bool debug = false;

	///////////////////////////////////////////////////////////////////////////////
	//Perform Intersection, skip first two elements because they are top and bottom
	///////////////////////////////////////////////////////////////////////////////
	int n = polylines[0].size() - 1;

	for (int i = 0; i < n; i++) {//iterate sides only

		if (j_mf[i + 2].first != -1 && joints[j_mf[i + 2].first](j_mf[i + 2].second, true).size() > 0) {

			if(debug) CGAL_Debug(0);
			///////////////////////////////////////////////////////////////////////////////
			//Take last lines Element outline can be not in the same order as joint outlines Take joint segment and measure its point to the plane
			///////////////////////////////////////////////////////////////////////////////
			bool flag = 
				CGAL::squared_distance(planes[0].projection(joints[j_mf[i + 2].first](j_mf[i + 2].second, true).back()[0]), joints[j_mf[i + 2].first](j_mf[i + 2].second, true).back()[0]) >
				CGAL::squared_distance(planes[0].projection(joints[j_mf[i + 2].first](j_mf[i + 2].second, false).back()[0]), joints[j_mf[i + 2].first](j_mf[i + 2].second, false).back()[0])
			;

			if (debug) CGAL_Debug(1);

			if (flag)
				joints[j_mf[i + 2].first].reverse(j_mf[i + 2].second);

			if (debug) CGAL_Debug(2);


			IK::Segment_3 joint_line_0(joints[j_mf[i + 2].first](j_mf[i + 2].second, true).back()[0], joints[j_mf[i + 2].first](j_mf[i + 2].second, true).back()[1]);
			IK::Segment_3 joint_line_1(joints[j_mf[i + 2].first](j_mf[i + 2].second, false).back()[0], joints[j_mf[i + 2].first](j_mf[i + 2].second, false).back()[1]);

			//CGAL_Debug(joint_line_0[0], true);
			//CGAL_Debug(joint_line_0[1], true);
			if (debug) CGAL_Debug(3);
			
			///////////////////////////////////////////////////////////////////////////////
			//Take last lines
			///////////////////////////////////////////////////////////////////////////////
			int prev = (((i - 1) % n) + n) % n;
			int next = (((i + 1) % n) + n) % n;
			int nextnext = (((i + 2) % n) + n) % n;

			IK::Segment_3 next_segment_0(polylines[0][prev], polylines[0][i]);
			IK::Segment_3 prev_segment_0(polylines[0][next], polylines[0][nextnext]);
			IK::Segment_3 next_segment_1(polylines[1][prev], polylines[1][i]);
			IK::Segment_3 prev_segment_1(polylines[1][next], polylines[1][nextnext]);
			if (debug) CGAL_Debug(4);

			///////////////////////////////////////////////////////////////////////////////
			//Intersect them with side lines, same principle must work on both polygons
			///////////////////////////////////////////////////////////////////////////////
			//1 Perform intersection line-line (needs implementation from rhino)
			//3 If intersecting relocate joint line points --|*---------*|--, if not overlaping do not change |  *-----*  |.
			IK::Point_3 p0_int, p1_int, p2_int, p3_int;
			double t0_int, t1_int, t2_int, t3_int;

			IK::Plane_3 joint_line_plane_0(joint_line_0[0], CGAL::cross_product(planes[0].orthogonal_vector(), joint_line_0.to_vector()));
			IK::Plane_3 joint_line_plane_1(joint_line_1[0], CGAL::cross_product(planes[1].orthogonal_vector(), joint_line_1.to_vector()));
			if (debug) CGAL_Debug(4.5);
			CGAL_IntersectionUtil::plane_plane_plane(planes[2 + prev], joint_line_plane_0, planes[0], p0_int, joint_line_0, t0_int);
			if (debug) CGAL_Debug(4.6);
			CGAL_IntersectionUtil::plane_plane_plane(planes[2 + next], joint_line_plane_0, planes[0], p1_int, joint_line_0, t1_int);

			CGAL_IntersectionUtil::plane_plane_plane(planes[2 + prev], joint_line_plane_1, planes[1], p2_int, joint_line_1, t2_int);
			CGAL_IntersectionUtil::plane_plane_plane(planes[2 + next], joint_line_plane_1, planes[1], p3_int, joint_line_1, t3_int);

			if (debug) CGAL_Debug(5);
			///////////////////////////////////////////////////////////////////////////////
			//2 Relocate side segments points to intersection points
			///////////////////////////////////////////////////////////////////////////////
			polylines[0][i] = p0_int;
			polylines[0][next] = p1_int;
			//joints[j_mf[i + 2].first](j_mf[i + 2].second, true)[0][0] = p0_int; //only when end point are not on the same axis
			//joints[j_mf[i + 2].first](j_mf[i + 2].second, true)[0].back() = p1_int;//only when end point are not on the same axis

			polylines[1][i] = p2_int;
			polylines[1][next] = p3_int;
			//joints[j_mf[i + 2].first](j_mf[i + 2].second, false)[0][0] = p2_int;//only when end point are not on the same axis
			//joints[j_mf[i + 2].first](j_mf[i + 2].second, false)[0].back() = p3_int;//only when end point are not on the same axis
			if (debug) CGAL_Debug(6);
			///////////////////////////////////////////////////////////////////////////////
			//3 Check orientation of joint outlines, if needed flip
			///////////////////////////////////////////////////////////////////////////////
			bool flip = CGAL::has_smaller_distance_to_point(
				p0_int,
				joints[j_mf[i + 2].first](j_mf[i + 2].second, !flag)[0].front(),
				joints[j_mf[i + 2].first](j_mf[i + 2].second, !flag)[0].back()
			);
			if (debug) CGAL_Debug(7);
			if (!flip) {

				//bottom
				std::reverse(
					joints[j_mf[i + 2].first](j_mf[i + 2].second, !flag)[0].begin(),
					joints[j_mf[i + 2].first](j_mf[i + 2].second, !flag)[0].end()
				);

				//top
				std::reverse(
					joints[j_mf[i + 2].first](j_mf[i + 2].second, flag)[0].begin(),
					joints[j_mf[i + 2].first](j_mf[i + 2].second, flag)[0].end()
				);
			}
			if (debug) CGAL_Debug(8);

		}//if joint edge
	} //for i


	//Update the end
	polylines[0][polylines[0].size() - 1] = polylines[0][0];
	polylines[1][polylines[1].size() - 1] = polylines[1][0];





	///////////////////////////////////////////////////////////////////////////////
	//After intersection is ready merger all polylines
	///////////////////////////////////////////////////////////////////////////////
	CGAL_Polyline newPolyline0;
	CGAL_Polyline newPolyline1;

	for (int i = 0; i < n; i++) {//iterate sides only

		newPolyline0.push_back(polylines[0][i]);
		newPolyline1.push_back(polylines[1][i]);

		if (j_mf[i + 2].first != -1 && joints[j_mf[i + 2].first](j_mf[i + 2].second, true).size() > 0) {
			for (auto pp : joints[j_mf[i + 2].first](j_mf[i + 2].second, true)[0]) newPolyline0.push_back(pp);
			for (auto pp : joints[j_mf[i + 2].first](j_mf[i + 2].second, false)[0]) newPolyline1.push_back(pp);
		}
	}


	//close
	newPolyline0.push_back(newPolyline0[0]);
	newPolyline1.push_back(newPolyline1[0]);
	
	//CGAL_Debug();
	//for (auto ppp : newPolyline0)
	//	CGAL_Debug(ppp, true);
	//CGAL_Debug();
	//for (auto ppp : newPolyline1)
	//	CGAL_Debug(ppp, true);



	///////////////////////////////////////////////////////////////////////////////
	//output
	///////////////////////////////////////////////////////////////////////////////
	output.push_back(newPolyline0);
	output.push_back(newPolyline1);
	return;




	


	


}

