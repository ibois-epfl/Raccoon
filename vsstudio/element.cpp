#include "element.h"

element::element() { }

//void element::get_joints_geometry(std::vector<CGAL_Polyline>)
//{
//}

element::element(int _id) {
	this->id = _id;
}

void element::get_joints_geometry(std::vector<joint>& joints, std::vector <CGAL_Polyline>& output) {

	for (int i = 0; i < polylines.size(); i++) {
		if (jointID_malefemale[i].first != -1) {
			output.insert(
				output.end(),
				joints[jointID_malefemale[i].first][jointID_malefemale[i].second].begin(),
				joints[jointID_malefemale[i].first][jointID_malefemale[i].second].end()
			);
		}
	}



}

void element::get_joints_geometry_as_closed_polylines(std::vector<joint>& joints, std::vector <CGAL_Polyline>& output) {


	///////////////////////////////////////////////////////////////////////////////
	//Perform Intersection, skip first two elements because they are top and bottom
	///////////////////////////////////////////////////////////////////////////////
	int n = polylines[0].size() - 1;
	for (int i = 0; i < polylines[0].size()-1; i++) {

		if (jointID_malefemale[i].first != -1) {

			auto plines_of_joint = joints[jointID_malefemale[i+2].first][jointID_malefemale[i+2].second];

			///////////////////////////////////////////////////////////////////////////////
			//Take last lines
			///////////////////////////////////////////////////////////////////////////////
			
			//Joint Polygon
			IK::Segment_3 last_segment_0 (plines_of_joint[plines_of_joint.size() - 2][0], plines_of_joint[plines_of_joint.size() - 2][1]);
			IK::Segment_3 last_segment_1 (plines_of_joint[plines_of_joint.size() - 1][0], plines_of_joint[plines_of_joint.size() - 1][1]);

			//Original Polygon
			int prev = (((i - 1) % n) + n) % n;
			int next = (((i + 1) % n) + n) % n;
			IK::Segment_3 next_segment_0(polylines[0][prev], polylines[0][i]);
			IK::Segment_3 prev_segment_0(polylines[0][i], polylines[0][next]);
			IK::Segment_3 next_segment_1(polylines[1][prev], polylines[1][i]);
			IK::Segment_3 prev_segment_1(polylines[1][i], polylines[1][next]);


			///////////////////////////////////////////////////////////////////////////////
			//Intersect them with side lines, same principle must work on both polygons
			///////////////////////////////////////////////////////////////////////////////
			//1 Perform intersection line-line (needs implementation from rhino)
			//2 Relocate side segments points to intersection points
			//3 If intersecting relocate joint line points --|*---------*|--, if not overlaping do not change |  *-----*  |.

	




		} else {


		}


	}


}