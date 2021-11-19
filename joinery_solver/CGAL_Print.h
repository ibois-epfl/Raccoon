#pragma once

#include "compas.h"
#include <float.h>
#include <inttypes.h>
static int Digs = DECIMAL_DIG;

inline void CGAL_Debug() {
	printf("\n");
}

//inline void CGAL_Debug(uint64_t v) {
//
//	printf("%" PRIu64 "\n", v);
//}

inline void CGAL_Debug(size_t v) {
	printf("%" PRIu64 "\n", v);
	//printf("CPP size %zi\n", v);
}



inline void CGAL_Debug(bool v) {
	printf("CPP bool %i\n", v);
}



inline void CGAL_Debug(int v) {
	printf("CPP int %i\n", v);
}

inline void CGAL_Debug(double v) {
	printf("CPP double %.17g\n", v);
}

inline void CGAL_Debug(double x, double y) {
	printf("CPP Vector %f %f \n", x, y);
}

inline void CGAL_Debug(double x, double y, double z) {
	printf("CPP Vector %f %f %f \n", x, y, z);
}

inline void CGAL_Debug(bool x, bool y, bool z, bool k) {
	printf("%i %i %i %i \n", x, y, z, k);
}

inline void CGAL_Debug(int x, int y, int z, int k) {
	printf("%i %i %i %i \n", x, y, z, k);
}


inline void CGAL_Debug(IK::Vector_3 v) {
	printf("CPP Vector %.17g, %.17g, %.17g \n", v.x(), v.y(), v.z());
}




inline void CGAL_Debug(IK::Point_3 v, bool render = true) {
	if (render) {
		printf("%.17g, %.17g, %.17g \n", v.x(), v.y(), v.z());
	}
	else {
		printf("CPP Vector %.17g, %.17g, %.17gf \n", v.x(), v.y(), v.z());
	}
	
}

inline void CGAL_Debug(IK::Point_3 v) {
	printf("CPP Vector %.17g, %.17g, %.17g \n", v.x(), v.y(), v.z());
}
