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
		std::vector<std::pair<int,bool>> jointID_malefemale; //e0,true,jointID
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

		void get_joints_geometry(std::vector<joint>& joints, std::vector <CGAL_Polyline>& output);
		void element::get_joints_geometry_as_closed_polylines(std::vector<joint>& joints, std::vector <CGAL_Polyline>& output);
};


inline element::element() { }

//void element::get_joints_geometry(std::vector<CGAL_Polyline>)
//{
//}

inline element::element(int _id) {
	this->id = _id;
}

inline void element::get_joints_geometry(std::vector<joint>& joints, std::vector <CGAL_Polyline>& output) {

	for (int i = 0; i < polylines.size(); i++) {
		if (jointID_malefemale[i].first != -1) {
			output.insert(
				output.end(),
				joints[jointID_malefemale[i].first](jointID_malefemale[i].second, true).begin(),
				joints[jointID_malefemale[i].first](jointID_malefemale[i].second, true).end()
			);
			output.insert(
				output.end(),
				joints[jointID_malefemale[i].first](jointID_malefemale[i].second, false).begin(),
				joints[jointID_malefemale[i].first](jointID_malefemale[i].second, false).end()
			);
		}
	}



}

inline void element::get_joints_geometry_as_closed_polylines(std::vector<joint>& joints, std::vector <CGAL_Polyline>& output) {
	//CGAL_Debug(polylines[0][0], true);
	//CGAL_Debug(polylines[0][1], true);



	///////////////////////////////////////////////////////////////////////////////
	//Perform Intersection, skip first two elements because they are top and bottom
	///////////////////////////////////////////////////////////////////////////////
	int n = polylines[0].size() - 1;

	for (int i = 0; i < n; i++) {//iterate sides only
		if (jointID_malefemale[i + 2].first != -1 && joints[jointID_malefemale[i + 2].first](jointID_malefemale[i + 2].second, true).size() > 0) {

			if (joints[jointID_malefemale[i + 2].first].type == 12) {//
				//CGAL_Debug(99999);
				CGAL_Debug(10000+id);
				CGAL_Debug(joints[jointID_malefemale[i + 2].first].f0);
				CGAL_Debug(joints[jointID_malefemale[i + 2].first].f1);
			}

			///////////////////////////////////////////////////////////////////////////////
			//Take last lines Element outline can be not in the same order as joint outlines Take joint segment and measure its point to the plane
			///////////////////////////////////////////////////////////////////////////////
			CGAL_Polyline* joints_polylines[2] = {
			&joints[jointID_malefemale[i + 2].first](jointID_malefemale[i + 2].second, true).back(),
			&joints[jointID_malefemale[i + 2].first](jointID_malefemale[i + 2].second, false).back()
			};

			bool flag = CGAL::squared_distance(planes[0].projection((*joints_polylines[0])[0]), (*joints_polylines[0])[0]) > CGAL::squared_distance(planes[0].projection((*joints_polylines[1])[0]), (*joints_polylines[1])[0]);

			if (flag) {
				joints_polylines[0] = &joints[jointID_malefemale[i + 2].first](jointID_malefemale[i + 2].second, false).back();
				joints_polylines[1] = &joints[jointID_malefemale[i + 2].first](jointID_malefemale[i + 2].second, true).back();
			}
			////
			IK::Segment_3 joint_line_0((*joints_polylines[0])[0], (*joints_polylines[0])[1]);
			IK::Segment_3 joint_line_1((*joints_polylines[1])[0], (*joints_polylines[1])[1]);

			CGAL_Debug(joint_line_0[0], true);
			CGAL_Debug(joint_line_0[1], true);

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

			CGAL_Debug(prev_segment_0[0], true);
			CGAL_Debug(prev_segment_0[1], true);

			CGAL_Debug(next_segment_0[0], true);
			CGAL_Debug(next_segment_0[1], true);

			///////////////////////////////////////////////////////////////////////////////
			//Intersect them with side lines, same principle must work on both polygons
			///////////////////////////////////////////////////////////////////////////////
			//1 Perform intersection line-line (needs implementation from rhino)
			//2 Relocate side segments points to intersection points
			//3 If intersecting relocate joint line points --|*---------*|--, if not overlaping do not change |  *-----*  |.
			IK::Point_3 p0_int, p1_int, p2_int, p3_int;
			double t0_int, t1_int, t2_int, t3_int;

			//CGAL_IntersectionUtil::line_line_line(
			//	prev_segment_0,
			//	joint_line_0,
			//	next_segment_0,
			//	p0_int, p1_int, t0_int, t1_int);
			/*CGAL_Debug(p0_int, true);
			CGAL_Debug(p1_int, true);*/

		}else {}

	}

	return;




	


	


}

