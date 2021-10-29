#pragma once
#include "compas.h"
#include "joint.h"

namespace joint_library {

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

		joint.m_boolean_type = { 1,1 };
		joint.f_boolean_type = { 1,1 };

		//Orient to 3D
		joint.orient_to_connection_area();
	}

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





		joint.f_boolean_type = { 2,2 };
		joint.m_boolean_type = { 2,2 };

		joint.orient_to_connection_area();
	}


}