#pragma once
#include "compas.h"
#include "joint.h"

namespace joint_library {

	inline double RemapNumbers(const double& speed = 50, const double& low1 = 0, const double& high1 = 2000, const double& low2 = 0, const double& high2 = 1) {
		return low2 + (speed - low1) * (high2 - low2) / (high1 - low1);
	}


	inline double Lerp(const double& value1, const double& value2, const double& amount) {
		return value1 + (value2 - value1) * amount;
	}



	inline void interpolate_points(const IK::Point_3& from, const IK::Point_3& to, const int Steps, const bool includeEnds, std::vector<IK::Point_3>& interpolated_points) {


		if (includeEnds) {

			interpolated_points.reserve(Steps + 2);
			interpolated_points.emplace_back(from);

			for (int i = 1; i < Steps + 1; i++) {
				double num = i / (double)(1 + Steps);
				interpolated_points.emplace_back(Lerp(from.hx(), to.hx(), num), Lerp(from.hy(), to.hy(), num), Lerp(from.hz(), to.hz(), num));
				
			}

			interpolated_points.emplace_back(to);
		}
		else {

			interpolated_points.reserve(Steps);

			for (int i = 1; i < Steps + 1; i++) {
				double num = i / (double)(1 + Steps);
				interpolated_points.emplace_back(Lerp(from.hx(), to.hx(), num),Lerp(from.hy(), to.hy(), num),Lerp(from.hz(), to.hz(), num));
			}
		}

	
	}



	//type_typeidedge_subtypeieinplane_id
	//0 - do not merge, 1 - edge insertion, 2 - hole 3 - insert between multiple edges hole



	//1-9
	inline void ss_e_ip_0(joint& joint) {


		joint.name = "ss_e_ip_0";

		//Joint lines, always the last line or rectangle is not a joint but an cutting element
		joint.f[0] = {
			{  IK::Point_3(0,-0.5,0.357142857142857), IK::Point_3(-0.5,-0.5,0.357142857142857), IK::Point_3(-0.5,-0.5,0.214285714285714), IK::Point_3(0.5,-0.5,0.214285714285714), IK::Point_3(0.5,-0.5,0.0714285714285714), IK::Point_3(-0.5,-0.5,0.0714285714285714), IK::Point_3(-0.5,-0.5,-0.0714285714285714), IK::Point_3(0.5,-0.5,-0.0714285714285714), IK::Point_3(0.5,-0.5,-0.214285714285714), IK::Point_3(-0.5,-0.5,-0.214285714285714), IK::Point_3(-0.5,-0.5,-0.357142857142857), IK::Point_3(0,-0.5,-0.357142857142857) },
			{ IK::Point_3(0,-0.5,0.5), IK::Point_3(0,-0.5,-0.5)}
		};

		joint.f[1] = {
			{  IK::Point_3(0,0.5,0.357142857142857), IK::Point_3(-0.5,0.5,0.357142857142857), IK::Point_3(-0.5,0.5,0.214285714285714), IK::Point_3(0.5,0.5,0.214285714285714), IK::Point_3(0.5,0.5,0.0714285714285714), IK::Point_3(-0.5,0.5,0.0714285714285714), IK::Point_3(-0.5,0.5,-0.0714285714285714), IK::Point_3(0.5,0.5,-0.0714285714285714), IK::Point_3(0.5,0.5,-0.214285714285714), IK::Point_3(-0.5,0.5,-0.214285714285714), IK::Point_3(-0.5,0.5,-0.357142857142857), IK::Point_3(0,0.5,-0.357142857142857) },
			{ IK::Point_3(0,0.5,0.5), IK::Point_3(0,0.5,-0.5) }
		};

		joint.m[0] = {
			{ IK::Point_3(0,-0.5,0.357142857142857), IK::Point_3(-0.5,-0.5,0.357142857142857), IK::Point_3(-0.5,-0.5,0.214285714285714), IK::Point_3(0.5,-0.5,0.214285714285714), IK::Point_3(0.5,-0.5,0.0714285714285714), IK::Point_3(-0.5,-0.5,0.0714285714285714), IK::Point_3(-0.5,-0.5,-0.0714285714285714), IK::Point_3(0.5,-0.5,-0.0714285714285714), IK::Point_3(0.5,-0.5,-0.214285714285714), IK::Point_3(-0.5,-0.5,-0.214285714285714), IK::Point_3(-0.5,-0.5,-0.357142857142857), IK::Point_3(0,-0.5,-0.357142857142857) },
			{ IK::Point_3(0,-0.5,0.5), IK::Point_3(0,-0.5,-0.5) }
		};


		joint.m[1] = {
			{ IK::Point_3(0,0.5,0.357142857142857), IK::Point_3(-0.5,0.5,0.357142857142857), IK::Point_3(-0.5,0.5,0.214285714285714), IK::Point_3(0.5,0.5,0.214285714285714), IK::Point_3(0.5,0.5,0.0714285714285714), IK::Point_3(-0.5,0.5,0.0714285714285714), IK::Point_3(-0.5,0.5,-0.0714285714285714), IK::Point_3(0.5,0.5,-0.0714285714285714), IK::Point_3(0.5,0.5,-0.214285714285714), IK::Point_3(-0.5,0.5,-0.214285714285714), IK::Point_3(-0.5,0.5,-0.357142857142857), IK::Point_3(0,0.5,-0.357142857142857)  },
			{ IK::Point_3(0,0.5,0.5), IK::Point_3(0,0.5,-0.5) }
		};


		joint.f_boolean_type = { '1','1' };
		joint.m_boolean_type = { '1','1' };

		joint.orient_to_connection_area();
	}

	//10-19
	inline void ss_e_op_0(joint& joint) {


		joint.name = "ss_e_op_0";

		//Joint lines, always the last line or rectangle is not a joint but an cutting element
		joint.f[0] = { {  IK::Point_3(0.5,0.5,-0.357142857142857), IK::Point_3(0.5,-0.5,-0.357142857142857), IK::Point_3(0.5,-0.5,-0.214285714285714), IK::Point_3(0.5,0.5,-0.214285714285714), IK::Point_3(0.5,0.5,-0.0714285714285715), IK::Point_3(0.5,-0.5,-0.0714285714285713), IK::Point_3(0.5,-0.5,0.0714285714285715), IK::Point_3(0.5,0.5,0.0714285714285714), IK::Point_3(0.5,0.5,0.214285714285714), IK::Point_3(0.5,-0.5,0.214285714285714), IK::Point_3(0.5,-0.5,0.357142857142857), IK::Point_3(0.5,0.5,0.357142857142857) }, 
			{ IK::Point_3(0.5,0.5,-0.5), IK::Point_3(0.5,0.5,0.5) } };


		joint.f[1] = { {IK::Point_3(-0.5,0.5,-0.357142857142857), IK::Point_3(-0.5,-0.5,-0.357142857142857), IK::Point_3(-0.5,-0.5,-0.214285714285714), IK::Point_3(-0.5,0.5,-0.214285714285714), IK::Point_3(-0.5,0.5,-0.0714285714285715), IK::Point_3(-0.5,-0.5,-0.0714285714285713), IK::Point_3(-0.5,-0.5,0.0714285714285715), IK::Point_3(-0.5,0.5,0.0714285714285714), IK::Point_3(-0.5,0.5,0.214285714285714), IK::Point_3(-0.5,-0.5,0.214285714285714), IK::Point_3(-0.5,-0.5,0.357142857142857), IK::Point_3(-0.5,0.5,0.357142857142857) }, 
			{ IK::Point_3(-0.5,0.5,-0.5), IK::Point_3(-0.5,0.5,0.5) } };


		joint.m[0] = { {  IK::Point_3(-0.5,0.5,0.357142857142857), IK::Point_3(0.5,0.5,0.357142857142857), IK::Point_3(0.5,0.5,0.214285714285714), IK::Point_3(-0.5,0.5,0.214285714285714), IK::Point_3(-0.5,0.5,0.0714285714285715), IK::Point_3(0.5,0.5,0.0714285714285713), IK::Point_3(0.5,0.5,-0.0714285714285715), IK::Point_3(-0.5,0.5,-0.0714285714285713), IK::Point_3(-0.5,0.5,-0.214285714285714), IK::Point_3(0.5,0.5,-0.214285714285714), IK::Point_3(0.5,0.5,-0.357142857142857), IK::Point_3(-0.5,0.5,-0.357142857142857) },
			{ IK::Point_3(-0.5,0.5,0.5), IK::Point_3(-0.5,0.5,-0.5) } };


		joint.m[1] = { { IK::Point_3(-0.5,-0.5,0.357142857142857), IK::Point_3(0.5,-0.5,0.357142857142857), IK::Point_3(0.5,-0.5,0.214285714285714), IK::Point_3(-0.5,-0.5,0.214285714285714), IK::Point_3(-0.5,-0.5,0.0714285714285713), IK::Point_3(0.5,-0.5,0.0714285714285712), IK::Point_3(0.5,-0.5,-0.0714285714285716), IK::Point_3(-0.5,-0.5,-0.0714285714285715), IK::Point_3(-0.5,-0.5,-0.214285714285714), IK::Point_3(0.5,-0.5,-0.214285714285714), IK::Point_3(0.5,-0.5,-0.357142857142857), IK::Point_3(-0.5,-0.5,-0.357142857142857)}, 
			{ IK::Point_3(-0.5,-0.5,0.5), IK::Point_3(-0.5,-0.5,-0.5) } };




		joint.f_boolean_type = { '1','1' };
		joint.m_boolean_type = { '1','1' };

		joint.orient_to_connection_area();
	}



	inline void ss_e_op_1(joint& joint, const double& division_distance, const double& shift) {


		joint.name = "ss_e_op_1";

		//Resize arrays
		joint.f[0].reserve(2);
		joint.f[1].reserve(2);
		joint.m[0].reserve(2);
		joint.m[1].reserve(2);

		////////////////////////////////////////////////////////////////////
		//Number of divisions
		//Input joint line (its lengths)
		//Input distance for division
		////////////////////////////////////////////////////////////////////
		double joint_length = CGAL::squared_distance(joint.joint_lines[0][0], joint.joint_lines[0][1]); // Math.Abs(500);
		int divisions = (int)std::ceil(joint_length / (division_distance * division_distance));
		divisions = (int)std::max(2, std::min(20, divisions));
		divisions += divisions % 2;
		////////////////////////////////////////////////////////////////////
		//Interpolate points
		////////////////////////////////////////////////////////////////////
		std::vector<IK::Point_3> arrays[4];





		interpolate_points(IK::Point_3(0.5, -0.5, -0.5), IK::Point_3(0.5, -0.5, 0.5), divisions, false, arrays[0]);
		interpolate_points(IK::Point_3(-0.5, -0.5, -0.5), IK::Point_3(-0.5, -0.5, 0.5), divisions, false, arrays[1]);
		interpolate_points(IK::Point_3(-0.5, 0.5, -0.5), IK::Point_3(-0.5, 0.5, 0.5), divisions, false, arrays[2]);
		interpolate_points(IK::Point_3(0.5, 0.5, -0.5), IK::Point_3(0.5, 0.5, 0.5), divisions, false, arrays[3]);




		////////////////////////////////////////////////////////////////////
		//Move segments
		////////////////////////////////////////////////////////////////////
		int start = 0;
		
		IK::Vector_3 v = shift == 0 ? IK::Vector_3(0,0,0) : IK::Vector_3(0,0, RemapNumbers(shift, 0, 1.0, -0.25, 0.25) / (divisions+1) );
		for (int i = start; i < 4; i += 1) {

			int mid = (int)(arrays[i].size() * 0.5);
			for (int j = 0; j < arrays[i].size(); j++) {
				//int flip = (j % 2 < 0) ? 1 : -1;
			

				int flip = (j < mid) ? 1 : -1;

				if (i == 1) {
					if (j < (mid - 1) || j > mid)
						arrays[i][j] -= 4 * v * flip;
				}
				else if (i == 0 || i == 2) {
					if (j < (mid - 1) || j > mid)
						arrays[i][j] -= 2 * v * flip;
				}

				
			}
		}

		////////////////////////////////////////////////////////////////////
		//Create Polylines
		////////////////////////////////////////////////////////////////////

		for (int i = 0; i < 4; i += 2) {

			CGAL_Polyline pline;
			pline.reserve(arrays[0].size() * 2);

			for (int j = 0; j < arrays[0].size(); j++) {

				bool flip = j % 2 == 0;
				flip = i < 2 ? flip : !flip;

				if (flip) {
					pline.push_back(arrays[i + 0][j]);
					pline.push_back(arrays[i + 1][j]);
				}
				else {
					pline.push_back(arrays[i + 1][j]);
					pline.push_back(arrays[i + 0][j]);
				}

			}

			if (i < 2) {
				joint.m[1] = {
					pline,
					//{ pline[0], pline[pline.size() - 1] },
					{ pline[0], pline[pline.size() - 1] }
				};
			}
			else {
				joint.m[0] = {
					pline,
					//{ pline[0], pline[pline.size() - 1] },
					{ pline[0], pline[pline.size() - 1] }

				};
			}
		}

		for (int i = 1; i < 4; i += 2) {

			CGAL_Polyline pline;
			pline.reserve(arrays[0].size() * 2);

			for (int j = 0; j < arrays[0].size(); j++) {

				bool flip = j % 2 == 0;
				flip = i < 2 ? flip : !flip;

				if (flip) {
					pline.push_back(arrays[i + 0][j]);
					pline.push_back(arrays[(i + 1) % 4][j]);
				}
				else {
					pline.push_back(arrays[(i + 1) % 4][j]);
					pline.push_back(arrays[i + 0][j]);
				}

			}

			if (i < 2) {
				joint.f[0] = {
					pline,
					//{ pline[0],pline[pline.size() - 1] },
					{ pline[0],pline[pline.size() - 1] }
				};
			}
			else {
				joint.f[1] = {
					pline,
					//{ pline[0],pline[pline.size() - 1] },
					{ pline[0],pline[pline.size() - 1] }
				};
			}
		}




		joint.f_boolean_type = { '1','1' };
		joint.m_boolean_type = { '1','1' };

		joint.orient_to_connection_area();
	}

	

	//20-29
	inline void ts_e_p_0(joint& joint) {


		joint.name = "ts_e_p_0";


		joint.f[0] = { { IK::Point_3(-0.5,-0.5,0.357142857142857), IK::Point_3(0.5,-0.5,0.357142857142857), IK::Point_3(0.5,-0.5,0.214285714285714), IK::Point_3(-0.5,-0.5,0.214285714285714), IK::Point_3(-0.5,-0.5,0.357142857142857) }, { IK::Point_3(-0.5,-0.5,0.0714285714285715), IK::Point_3(0.5,-0.5,0.0714285714285715), IK::Point_3(0.5,-0.5,-0.0714285714285713), IK::Point_3(-0.5,-0.5,-0.0714285714285713), IK::Point_3(-0.5,-0.5,0.0714285714285715) }, { IK::Point_3(-0.5,-0.5,-0.214285714285714), IK::Point_3(0.5,-0.5,-0.214285714285714), IK::Point_3(0.5,-0.5,-0.357142857142857), IK::Point_3(-0.5,-0.5,-0.357142857142857), IK::Point_3(-0.5,-0.5,-0.214285714285714) }, { IK::Point_3(-0.5,-0.5,0.357142857142857), IK::Point_3(-0.5,-0.5,-0.357142857142857), IK::Point_3(0.5,-0.5,-0.357142857142857), IK::Point_3(0.5,-0.5,0.357142857142857), IK::Point_3(-0.5,-0.5,0.357142857142857) } };



		joint.f[1] = { { IK::Point_3(-0.5,0.5,0.357142857142857), IK::Point_3(0.5,0.5,0.357142857142857), IK::Point_3(0.5,0.5,0.214285714285714), IK::Point_3(-0.5,0.5,0.214285714285714), IK::Point_3(-0.5,0.5,0.357142857142857) }, { IK::Point_3(-0.5,0.5,0.0714285714285713), IK::Point_3(0.5,0.5,0.0714285714285713), IK::Point_3(0.5,0.5,-0.0714285714285715), IK::Point_3(-0.5,0.5,-0.0714285714285715), IK::Point_3(-0.5,0.5,0.0714285714285713) }, { IK::Point_3(-0.5,0.5,-0.214285714285714), IK::Point_3(0.5,0.5,-0.214285714285714), IK::Point_3(0.5,0.5,-0.357142857142857), IK::Point_3(-0.5,0.5,-0.357142857142857), IK::Point_3(-0.5,0.5,-0.214285714285714) }, { IK::Point_3(-0.5,0.5,0.357142857142857), IK::Point_3(-0.5,0.5,-0.357142857142857), IK::Point_3(0.5,0.5,-0.357142857142857), IK::Point_3(0.5,0.5,0.357142857142857), IK::Point_3(-0.5,0.5,0.357142857142857) } };



		//Joint lines, always the last line or rectangle is not a joint but an cutting element
		joint.m[0] = { { IK::Point_3(0.5,-0.5,-0.357142857142857), IK::Point_3(0.5,0.5,-0.357142857142857), IK::Point_3(0.5,0.5,-0.214285714285714), IK::Point_3(0.5,-0.5,-0.214285714285714), IK::Point_3(0.5,-0.5,-0.0714285714285715), IK::Point_3(0.5,0.5,-0.0714285714285713), IK::Point_3(0.5,0.5,0.0714285714285715), IK::Point_3(0.5,-0.5,0.0714285714285714), IK::Point_3(0.5,-0.5,0.214285714285714), IK::Point_3(0.5,0.5,0.214285714285714), IK::Point_3(0.5,0.5,0.357142857142857), IK::Point_3(0.5,-0.5,0.357142857142857) }, { IK::Point_3(0.5,-0.5,-0.357142857142857), IK::Point_3(0.5,-0.5,0.357142857142857) } };


		joint.m[1] = { { IK::Point_3(-0.5,-0.5,-0.357142857142857), IK::Point_3(-0.5,0.5,-0.357142857142857), IK::Point_3(-0.5,0.5,-0.214285714285714), IK::Point_3(-0.5,-0.5,-0.214285714285714), IK::Point_3(-0.5,-0.5,-0.0714285714285715), IK::Point_3(-0.5,0.5,-0.0714285714285713), IK::Point_3(-0.5,0.5,0.0714285714285715), IK::Point_3(-0.5,-0.5,0.0714285714285714), IK::Point_3(-0.5,-0.5,0.214285714285714), IK::Point_3(-0.5,0.5,0.214285714285714), IK::Point_3(-0.5,0.5,0.357142857142857), IK::Point_3(-0.5,-0.5,0.357142857142857) }, { IK::Point_3(-0.5,-0.5,-0.357142857142857), IK::Point_3(-0.5,-0.5,0.357142857142857) } };



		joint.f_boolean_type = { '2','2','2','2' };
		joint.m_boolean_type = { '1','1' };

		joint.orient_to_connection_area();
	}

	inline void ts_e_p_1(joint& joint) {


		joint.name = "ts_e_p_1";


		joint.f[0] = { { IK::Point_3(-0.5,-0.5,-0.277777777777778), IK::Point_3(0.5,-0.5,-0.277777777777778), IK::Point_3(0.5,-0.5,-0.388888888888889), IK::Point_3(-0.5,-0.5,-0.388888888888889), IK::Point_3(-0.5,-0.5,-0.277777777777778) }, { IK::Point_3(-0.5,-0.5,0.166666666666667), IK::Point_3(0.5,-0.5,0.166666666666667), IK::Point_3(0.5,-0.5,0.0555555555555556), IK::Point_3(-0.5,-0.5,0.0555555555555556), IK::Point_3(-0.5,-0.5,0.166666666666667) }, { IK::Point_3(-0.5,-0.5,0.166666666666667), IK::Point_3(-0.5,-0.5,-0.388888888888889), IK::Point_3(0.5,-0.5,-0.388888888888889), IK::Point_3(0.5,-0.5,0.166666666666667), IK::Point_3(-0.5,-0.5,0.166666666666667) } };


		joint.f[1] = { { IK::Point_3(-0.5,0.5,-0.277777777777778), IK::Point_3(0.5,0.5,-0.277777777777778), IK::Point_3(0.5,0.5,-0.388888888888889), IK::Point_3(-0.5,0.5,-0.388888888888889), IK::Point_3(-0.5,0.5,-0.277777777777778) }, { IK::Point_3(-0.5,0.5,0.166666666666667), IK::Point_3(0.5,0.5,0.166666666666667), IK::Point_3(0.5,0.5,0.0555555555555556), IK::Point_3(-0.5,0.5,0.0555555555555556), IK::Point_3(-0.5,0.5,0.166666666666667) }, { IK::Point_3(-0.5,0.5,0.166666666666667), IK::Point_3(-0.5,0.5,-0.388888888888889), IK::Point_3(0.5,0.5,-0.388888888888889), IK::Point_3(0.5,0.5,0.166666666666667), IK::Point_3(-0.5,0.5,0.166666666666667) } };


		//Joint lines, always the last line or rectangle is not a joint but an cutting element
		joint.m[0] = { { IK::Point_3(0.5,-0.5,0.166666666666667), IK::Point_3(0.5,0.5,0.166666666666667), IK::Point_3(0.5,0.5,0.0555555555555556), IK::Point_3(0.5,-0.5,0.0555555555555556), IK::Point_3(0.5,-0.5,-0.277777777777778), IK::Point_3(0.5,0.5,-0.277777777777778), IK::Point_3(0.5,0.5,-0.388888888888889), IK::Point_3(0.5,-0.5,-0.388888888888889)}, { IK::Point_3(0.5,-0.5,0.5), IK::Point_3(0.5,-0.5,-0.5) } };

		joint.m[1] = { { IK::Point_3(-0.5,-0.5,0.166666666666667), IK::Point_3(-0.5,0.5,0.166666666666667), IK::Point_3(-0.5,0.5,0.0555555555555558), IK::Point_3(-0.5,-0.5,0.0555555555555557), IK::Point_3(-0.5,-0.5,-0.277777777777778), IK::Point_3(-0.5,0.5,-0.277777777777778), IK::Point_3(-0.5,0.5,-0.388888888888889), IK::Point_3(-0.5,-0.5,-0.388888888888889) }, { IK::Point_3(-0.5,-0.5,0.5), IK::Point_3(-0.5,-0.5,-0.5) } };


		joint.f_boolean_type = { '2','2','2','2' };
		joint.m_boolean_type = { '1','1' };

		joint.orient_to_connection_area();
	}

	//30-39
	inline void cr_c_ip_0(joint& joint) {


		//printf("Cross_Simple %zi", joint.m.size());

		joint.name = "cr_c_ip_0";

		joint.f[0] = {
		{ IK::Point_3(0.5, -0.5, 0), IK::Point_3(0.5, 0.5, 0), IK::Point_3(0.5, 0.5, 0.55), IK::Point_3(0.5, -0.5, 0.55), IK::Point_3(0.5, -0.5, 0) },
		{ IK::Point_3(-0.5,-0.5,0),IK::Point_3(-0.5,0.5,0),IK::Point_3(-0.5,0.5,0.55),IK::Point_3(-0.5,-0.5,0.55),IK::Point_3(-0.5,-0.5,0) }
		};


		joint.m[0] = {
		{ IK::Point_3(0.5,0.5,-0.55),IK::Point_3(-0.5,0.5,-0.55),IK::Point_3(-0.5,0.5,0),IK::Point_3(0.5,0.5,0),IK::Point_3(0.5,0.5,-0.55) },
		{ IK::Point_3(0.5,-0.5,-0.55),IK::Point_3(-0.5,-0.5,-0.55),IK::Point_3(-0.5,-0.5,0),IK::Point_3(0.5,-0.5,0),IK::Point_3(0.5,-0.5,-0.55) }
		};

		joint.m_boolean_type = { '0','0' };
		joint.f_boolean_type = { '0','0' };

		//Orient to 3D
		joint.orient_to_connection_area();
	}

	inline void construct_joint_by_index(joint& joint, int& id_representing_joing_name, const double& division_distance, const double& shift) {

		if (id_representing_joing_name == 0) {
			cr_c_ip_0(joint);
		}
		else if (id_representing_joing_name > 0 && id_representing_joing_name < 10) {
			ss_e_ip_0(joint);
		}
		else if (id_representing_joing_name > 9 && id_representing_joing_name < 20) {

			switch (id_representing_joing_name)
			{
			case(10):
				ss_e_op_1(joint, division_distance, shift);
				break;
			default:
				ss_e_op_0(joint);
				break;
			}


		}
		else if (id_representing_joing_name > 19 && id_representing_joing_name < 30) {
			switch (id_representing_joing_name)
			{
			case(20):
				ts_e_p_1(joint);
				break;
			default:
				ts_e_p_0(joint);
				break;
			}
			
		}

		else if (id_representing_joing_name > 39 && id_representing_joing_name < 40) {
			cr_c_ip_0(joint);
		}
	}




}