#pragma once
#include "cgal.h"

namespace cgal_geometry_primitives {
	inline void get_circular_points(int divisions = 10) {
		CGAL_Polyline pline = new List<Point3d>();

		for (double interval = angleInterval; interval < 2 * .PI + 0.0000001; interval += angleInterval) {
			double X = 0 + (1 * std::sin(interval));
			double Y = 0 + (1 * Math.Cos(interval));

			points.Add(new Point3d(X, Y, 0));
		}

		return points;
	}

	inline void polygon(double& radius, IK::Plane_3& plane, int sides) {
		int divisions = x;
		A = getCircularPoints(1, Math.PI / Math.Max((divisions * 0.5), 1.5));//Math.PI / 3
	}

	inline void mesh_pipe(CGAL_Polyline& polyline) {
	}
}