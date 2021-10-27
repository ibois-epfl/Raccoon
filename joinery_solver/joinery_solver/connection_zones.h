#pragma once

#include "compas.h"
#include "CGAL_XFormUtil.h"
#include "CGAL_VectorUtil.h"
#include "CGAL_PolylineUtil.h"
#include "CGAL_BoxUtil.h"
#include "CGAL_PlaneUtil.h"
#include "CGAL_Print.h"
#include "RTree.h"
#include "AxisPlane.h"
#include "clipper.h"
#include "clipperUtil.h"

#include <algorithm>
#include <thread>
#include <functional>
#include <vector>
#include <execution>

#include "element.h"
#include "joint.h"
#include "joint_library.h"

inline void get_elements(std::vector<CGAL_Polyline>& pp, std::vector<element>& elements) {

	int n = pp.size() * 0.5;
	elements = std::vector<element>(n);

	for (int i = 0, id = 0; i < pp.size(); i += 2, id++) {


		elements[id].id = i * 0.5;

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//Get BoundingBox - AABB
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//Create copy of a polyline and transform points
		CGAL_Polyline twoPolylines;
		twoPolylines.resize(pp[i].size() + pp[i + 1].size());
		std::copy(pp[i].begin(), pp[i].end(), twoPolylines.begin());
		std::copy(pp[i + 1].begin(), pp[i + 1].end(), twoPolylines.begin() + pp[i].size());


		CGAL::Bbox_3 AABB = CGAL::bbox_3(twoPolylines.begin(), twoPolylines.end(), IK());

		CGAL_Polyline AABB_Min_Max = {
			IK::Point_3(AABB.xmin() - 1 * GlobalTolerance, AABB.ymin() - 1 * GlobalTolerance, AABB.zmin() - 1 * GlobalTolerance),
			IK::Point_3(AABB.xmax() + 1 * GlobalTolerance, AABB.ymax() + 1 * GlobalTolerance, AABB.zmax() + 1 * GlobalTolerance),
		};

		AABB = CGAL::bbox_3(AABB_Min_Max.begin(), AABB_Min_Max.end(), IK());

		elements[id].aabb = AABB;

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//Get Object Oriented BoundingBox - OOB -> transform to XY and back to 3D
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//Create Planes
		IK::Vector_3 planeAxes[4];
		CGAL_PolylineUtil::AveragePlane(pp[i], planeAxes);

		//Create Transformation
		CGAL::Aff_transformation_3<IK> xform_toXY = CGAL_XFormUtil::VectorsToXY(planeAxes[0], planeAxes[1], planeAxes[2], planeAxes[3]);
		CGAL::Aff_transformation_3<IK> xform_toXY_Inv = xform_toXY.inverse();

		//Transform the merged polyline to xy and compute xyBounding Box

		for (auto it = twoPolylines.begin(); it != twoPolylines.end(); ++it) {
			*it = it->transform(xform_toXY);
			//printf("CPP Transformed Point %d %d %d \n", it->x(), it->y(), it->z());
		}

		//Compute bounding box, get center point, and contruct 5 size vector, where 5th dimension is halfsite,  then transform box back to 3D by an inverse matrix
		CGAL::Bbox_3 AABBXY = CGAL::bbox_3(twoPolylines.begin(), twoPolylines.end(), IK());
		double scale = 10;
		IK::Vector_3 box[5] = {
			IK::Vector_3(
				(AABBXY.xmin() + AABBXY.xmax()) * 0.5,
				(AABBXY.ymin() + AABBXY.ymax()) * 0.5,
				(AABBXY.zmin() + AABBXY.zmax()) * 0.5),
			IK::Vector_3(1,0,0),
			IK::Vector_3(0,1,0),
			IK::Vector_3(0,0,1),
			IK::Vector_3(
			fabs((1.0 + GlobalTolerance * 1) * (AABBXY.xmax() - AABBXY.xmin()) * 0.5),//0.00001
			fabs((1.0 + GlobalTolerance * 1) * (AABBXY.ymax() - AABBXY.ymin()) * 0.5),
			fabs((1.0 + GlobalTolerance * 1) * (AABBXY.zmax() - AABBXY.zmin()) * 0.5))
		};
		//CGAL_Debug(1.0 + GlobalTolerance * 0.00001);

		CGAL_BoxUtil::TransformPlaneOrPlane(box, xform_toXY_Inv);
		CGAL_BoxUtil::Assign(box, elements[i * 0.5].oob, 5);

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//Check orientation of polylines and reverse if needed
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		if (twoPolylines.back().z() > 0) {
			std::reverse(pp[i].begin(), pp[i].end());
			std::reverse(pp[i + 1].begin(), pp[i + 1].end());
		}


		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//Get Side Polylines and Planes
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		elements[id].polylines = std::vector<CGAL_Polyline>(1 + pp[i].size());
		elements[id].polylines[0] = pp[i];
		elements[id].polylines[1] = pp[i + 1];

		elements[id].planes = std::vector<IK::Plane_3>(1 + pp[i].size());


		//IK::Point_3 origin = CGAL_PolylineUtil::Center(pp[i]);
		IK::Vector_3 normal;
		CGAL_VectorUtil::AverageNormal(pp[i], normal, true, false);
		elements[id].planes[0] = IK::Plane_3(CGAL_PolylineUtil::Center(pp[i]), normal);
		elements[id].planes[1] = IK::Plane_3(CGAL_PolylineUtil::Center(pp[i + 1]), -normal);
		elements[id].thickness = std::sqrt(CGAL::squared_distance(pp[i][0], elements[id].planes[1].projection(pp[i][0])));

		for (int j = 0; j < pp[i].size() - 1; j++) {
			elements[id].planes[2 + j] = IK::Plane_3(pp[i][j + 1], pp[i][j], pp[i + 1][j + 1]);
			elements[id].polylines[2 + j] = { pp[i][j],  pp[i][j + 1], pp[i + 1][j + 1], pp[i + 1][j], pp[i][j] };
		}

		//Edge initialization
		elements[id].jointID_malefemale = std::vector<std::pair<int, bool>>(pp[i].size() + 1, std::pair<int, bool>(-1, false));


	}



}




inline bool intersection_2D(CGAL_Polyline& p0, CGAL_Polyline& p1, IK::Plane_3& plane, CGAL_Polyline& c, double scale = 100000.0) {

	/////////////////////////////////////////////////////////////////////////////////////
	//Orient from 3D to 2D
	/////////////////////////////////////////////////////////////////////////////////////
	CGAL_Polyline a;
	CGAL_Polyline b;
	CGAL_PolylineUtil::Duplicate(p0, a);
	CGAL_PolylineUtil::Duplicate(p1, b);

	/////////////////////////////////////////////////////////////////////////////////////
	//Create Transformation
	/////////////////////////////////////////////////////////////////////////////////////
	CGAL::Aff_transformation_3<IK> xform_toXY = CGAL_XFormUtil::PlaneToXY(p0[0], plane);
	CGAL::Aff_transformation_3<IK> xform_toXY_Inv = xform_toXY.inverse();
	CGAL_PolylineUtil::Transform(a, xform_toXY);
	CGAL_PolylineUtil::Transform(b, xform_toXY);

	/////////////////////////////////////////////////////////////////////////////////////
	//Convert to Clipper
	/////////////////////////////////////////////////////////////////////////////////////
	std::vector< ClipperLib::IntPoint > pathA(a.size() - 1);
	std::vector< ClipperLib::IntPoint > pathB(b.size() - 1);
	for (int i = 0; i < a.size() - 1; i++) {
		pathA[i] = ClipperLib::IntPoint(a[i].x() * scale, a[i].y() * scale);
		//printf("%f,%f,%f \n", a[i].x(), a[i].y(), a[i].z());
	}
	//printf("\n");
	for (int i = 0; i < b.size() - 1; i++) {
		pathB[i] = ClipperLib::IntPoint(b[i].x() * scale, b[i].y() * scale);
		//printf("%f,%f,%f \n", b[i].x(), b[i].y(), b[i].z());
	}

	//printf("\n");
	//printf("\n");

	ClipperLib::Clipper clipper;
	clipper.AddPath(pathA, ClipperLib::ptSubject, true);
	clipper.AddPath(pathB, ClipperLib::ptClip, true);
	ClipperLib::Paths C;
	clipper.Execute(ClipperLib::ctIntersection, C, ClipperLib::pftNonZero, ClipperLib::pftNonZero);



	if (C.size() > 0) {

		//CGAL_Debug(C[0].size());

		//CleanPolygon(C[0], GlobalTolerance * scale);//clean polygons

		//if (C[0].size() > 3 && Area(C[0]) > GlobalClipperAreaTolerance * scale * scale) {//skip triangles and very small polygons 
		if (C[0].size() > 3 && Area(C[0]) > (Area(pathA)) * GlobalClipperAreaTolerance) {//skip triangles and very small polygons 
		
			//CGAL_Debug(Area(C[0]));
			c.resize(C[0].size() + 1);

			for (int i = 0; i < C[0].size(); i++) {
				IK::Point_3 p(C[0][i].X / scale, C[0][i].Y / scale, 0);
				p = p.transform(xform_toXY_Inv);//Rotate back to 3D
				c[i] = p;
			}

			c[C[0].size()] = c[0];//Close

		}
		else {
			return false;
		}


	}
	else {
		return false;
	}

	/////////////////////////////////////////////////////////////////////////////////////
	//Output
	/////////////////////////////////////////////////////////////////////////////////////
	return true;

}


inline bool plane_to_face(std::vector<
	CGAL_Polyline>& Polyline0,
	std::vector<CGAL_Polyline>& Polyline1,
	std::vector<IK::Plane_3>& Plane0,
	std::vector<IK::Plane_3>& Plane1,
	int& e0, int& e1,
	CGAL_Polyline& joint_area,
	//CGAL_Polyline(&joint_quads)[2],
	CGAL_Polyline(&joint_lines)[2],
	CGAL_Polyline(&joint_volumes_pairA_pairB)[4],
	int& type,

	double angleTol = 16,
	bool checkOverlap = false) {



	//////////////////////////////////////////////////////////////////////////////
	//Check 
	//1. Vector angles if they are not close to parallel 
	//2. Are polylines overlapping (usually skipped within first except if stucture is planar) 3. Check overlay between center lines
	//////////////////////////////////////////////////////////////////////////////
	double angle = 90.0 - fabs(CGAL::approximate_angle(Plane0[0].orthogonal_vector(), Plane1[0].orthogonal_vector()) - 90);
	if (angle < angleTol) return false;


	//if (checkOverlap) if (Polylines0Center.plines[0].ToNurbsCurve().CurvesOverlap(Polylines1Center.plines[0].ToNurbsCurve())) return; //Happens very rarely if elements are planar






	//Line centerIntersectionLine = Line.Unset;
	//Line centerIntersectionLineMax = Line.Unset;
	IK::Segment_3 centerIntersectionLine;
	IK::Segment_3 centerIntersectionLineMax;

	//
	//    if (!CGAL_PolylineUtil::PlanePolyline(Polyline0[0], Polyline1[0], Plane0[0], Plane1[0], centerIntersectionLine, centerIntersectionLineMax)) {
	///*        CGAL_Debug(centerIntersectionLine[0],true);
	//        CGAL_Debug(centerIntersectionLine[1],true)*/;
	//        //jointLine = { centerIntersectionLine[0],centerIntersectionLine[1] };
	//        //jointArea = { centerIntersectionLine[0],centerIntersectionLine[1] };
	//        return false;
	//    }

		//return true;

		//////////////////////////////////////////////////////////////////////////////
		//Inputs for intersection
		//////////////////////////////////////////////////////////////////////////////
	auto cx0 = &Polyline0[0];
	auto cx1 = &Polyline0[1];
	auto cy0 = &Polyline1[0];
	auto cy1 = &Polyline1[1];

	auto px0 = &Plane0[0];
	auto px1 = &Plane0[1];
	auto py0 = &Plane1[0];
	auto py1 = &Plane1[1];

	////////////////////////////////////////////////////////////////////////////////
	////Intersection results -> 8 points -> 4 lines
	////////////////////////////////////////////////////////////////////////////////





	//IK::Segment_3 cx0_py0__cy0_px0_Max;
	//IK::Segment_3 cx0_py1__cy1_px0_Max;
	//IK::Segment_3 cx1_py0__cy0_px1_Max;
	//IK::Segment_3 cx1_py1__cy1_px1_Max;

	//////////////////////////////////////////////////////////////////////////////
	//Perform intersection
	//////////////////////////////////////////////////////////////////////////////
	//printf("\n_");
	IK::Segment_3 cx0_py0__cy0_px0;
	if (!CGAL_PolylineUtil::PlanePolyline(*cx0, *cy0, *px0, *py0, cx0_py0__cy0_px0)) return false;//, cx0_py0__cy0_px0_Max
	//printf("A");

	IK::Segment_3 cx0_py1__cy1_px0;
	if (!CGAL_PolylineUtil::PlanePolyline(*cx0, *cy1, *px0, *py1, cx0_py1__cy1_px0)) return false;//, cx0_py1__cy1_px0_Max
	//printf("B");

	IK::Segment_3 cx1_py0__cy0_px1;
	if (!CGAL_PolylineUtil::PlanePolyline(*cx1, *cy0, *px1, *py0, cx1_py0__cy0_px1)) return false;//, cx1_py0__cy0_px1_Max
	//printf("C");

	IK::Segment_3 cx1_py1__cy1_px1;
	if (!CGAL_PolylineUtil::PlanePolyline(*cx1, *cy1, *px1, *py1, cx1_py1__cy1_px1)) return false;//,cx1_py1__cy1_px1_Max
	//printf("D");
	//printf("_\n");

	//////////////////////////////////////////////////////////////////////////////
	//Sort Lines
	//////////////////////////////////////////////////////////////////////////////
	if (CGAL::approximate_angle(cx0_py0__cy0_px0.to_vector(), cx0_py1__cy1_px0.to_vector()) > CGAL::approximate_angle(cx0_py0__cy0_px0.to_vector(), -cx0_py1__cy1_px0.to_vector()))
		cx0_py1__cy1_px0 = cx0_py1__cy1_px0.opposite();
	if (CGAL::approximate_angle(cx0_py0__cy0_px0.to_vector(), cx1_py0__cy0_px1.to_vector()) > CGAL::approximate_angle(cx0_py0__cy0_px0.to_vector(), -cx1_py0__cy0_px1.to_vector()))
		cx1_py0__cy0_px1 = cx1_py0__cy0_px1.opposite();
	if (CGAL::approximate_angle(cx0_py0__cy0_px0.to_vector(), cx1_py1__cy1_px1.to_vector()) > CGAL::approximate_angle(cx0_py0__cy0_px0.to_vector(), -cx1_py1__cy1_px1.to_vector()))
		cx1_py1__cy1_px1 = cx1_py1__cy1_px1.opposite();



	//////////////////////////////////////////////////////////////////////////////
	//Define joint area from lines
	//////////////////////////////////////////////////////////////////////////////
	joint_volumes_pairA_pairB[0] = { cx0_py0__cy0_px0[0], cx0_py1__cy1_px0[0], cx1_py1__cy1_px1[0], cx1_py0__cy0_px1[0], cx0_py0__cy0_px0[0] };
	joint_volumes_pairA_pairB[1] = { cx0_py0__cy0_px0[1], cx0_py1__cy1_px0[1], cx1_py1__cy1_px1[1], cx1_py0__cy0_px1[1], cx0_py0__cy0_px0[1] };

	//////////////////////////////////////////////////////////////////////////////
	//Find shortest and longest curve based on closest point
	//-----*-----*--
	//*--------------*
	//////////////////////////////////////////////////////////////////////////////
	IK::Segment_3 c;
	CGAL_PolylineUtil::MidLine(cx0_py1__cy1_px0, cx1_py0__cy0_px1, c);
	CGAL_PolylineUtil::ScaleLine(c, 10);

	//Find Max gap
	double cpt0[4];
	CGAL_PolylineUtil::ClosestPointTo(cx0_py0__cy0_px0[0], c, cpt0[0]);
	CGAL_PolylineUtil::ClosestPointTo(cx0_py1__cy1_px0[0], c, cpt0[1]);
	CGAL_PolylineUtil::ClosestPointTo(cx1_py0__cy0_px1[0], c, cpt0[2]);
	CGAL_PolylineUtil::ClosestPointTo(cx1_py1__cy1_px1[0], c, cpt0[3]);
	std::sort(cpt0, cpt0 + 4);


	double cpt1[4];
	CGAL_PolylineUtil::ClosestPointTo(cx0_py0__cy0_px0[1], c, cpt1[0]);
	CGAL_PolylineUtil::ClosestPointTo(cx0_py1__cy1_px0[1], c, cpt1[1]);
	CGAL_PolylineUtil::ClosestPointTo(cx1_py0__cy0_px1[1], c, cpt1[2]);
	CGAL_PolylineUtil::ClosestPointTo(cx1_py1__cy1_px1[1], c, cpt1[3]);
	std::sort(cpt1, cpt1 + 4);

	double cpt[8] = { cpt0[0] ,cpt0[1] ,cpt0[2] ,cpt0[3],  cpt1[0] ,cpt1[1] ,cpt1[2] ,cpt1[3] };
	std::sort(cpt, cpt + 8);

	IK::Segment_3 lMin(CGAL_PolylineUtil::PointAt(c, cpt0[3]), CGAL_PolylineUtil::PointAt(c, cpt1[0]));
	IK::Segment_3 lMax(CGAL_PolylineUtil::PointAt(c, cpt[0]), CGAL_PolylineUtil::PointAt(c, cpt[7]));


	//////////////////////////////////////////////////////////////////////////////
	//When elements are not planar there are two options: 1) Rectangular beams are 3D overaly 2) Plate is connected to sharp corner in an angle
	//////////////////////////////////////////////////////////////////////////////
		//IK::Plane_3 jointAreaPlane0(jointArea0[0], jointArea0[1], jointArea0[2]);
		//IK::Plane_3 jointAreaPlane1(jointArea1[0], jointArea1[1], jointArea1[2]);


	//if (CGAL::squared_distance(jointAreaPlane0.projection(jointArea0[2]), jointArea0[2]) > GlobalToleranceSquare * 2 ||
	//    CGAL::squared_distance(jointAreaPlane1.projection(jointArea1[2]), jointArea1[2]) > GlobalToleranceSquare * 2 ) {

	//    //IK::Point_3 allPts [8] = {
	//    //    cx0_py0__cy0_px0_Max[0], cx0_py0__cy0_px0_Max[1],
	//    //    cx0_py1__cy1_px0_Max[0], cx0_py1__cy1_px0_Max[1],
	//    //    cx1_py0__cy0_px1_Max[0], cx1_py0__cy0_px1_Max[1],
	//    //    cx1_py1__cy1_px1_Max[0], cx1_py1__cy1_px1_Max[1]
	//    //};

	//    IK::Plane_3 lMaxPlane0(lMax[0], lMax.direction());
	//    IK::Plane_3 lMaxPlane1(lMax[1], lMax.direction());

	//    //LineTwoPlanes()

	//    ////Compute oriented bbox from allPts
	//    //Box box = new Box(lMax.ToPlane(true), allPts);

	//    ////Intersect line with box
	//    //Rhino.Geometry.Intersect.Intersection.LineBox(lMax, box, 0.01, out Interval lineParams);

	//    ////Remake maxLine
	//    //lMax = new Line(lMax.PointAt(lineParams.T0), lMax.PointAt(lineParams.T1));
	//}


	//////////////////////////////////////////////////////////////////////////////
	//RE-Define joint area from lines
	//////////////////////////////////////////////////////////////////////////////
	//lMin mid plane
	IK::Point_3 lMinMid;
	CGAL_VectorUtil::MidPointConst(lMin.min(), lMin.max(), lMinMid);
	IK::Plane_3 midPlane(lMinMid, lMin.to_vector());//Min line mid-plane


	//Intersection lMax with midPlane
	IK::Point_3 midPlane_lMax;
	CGAL_PlaneUtil::PlaneLineIntersection(midPlane, lMax, midPlane_lMax);


	//Get max distance from middle point of min line
	int maxID = CGAL::squared_distance(lMax[0], midPlane_lMax) > CGAL::squared_distance(lMax[1], midPlane_lMax) ? 0 : 1;
	IK::Vector_3 v = maxID == 1 ? lMax[1] - midPlane_lMax : -(lMax[0] - midPlane_lMax);
	v *= (1 + GlobalExtend[2]);


	//intersection mid plane with four lines and move it in both directions
	CGAL_Polyline rm;
	CGAL_PlaneUtil::Plane4LinesIntersection(midPlane, cx0_py0__cy0_px0, cx0_py1__cy1_px0, cx1_py1__cy1_px1, cx1_py0__cy0_px1, rm);
	joint_volumes_pairA_pairB[0] = { IK::Point_3(rm[0]) + v, IK::Point_3(rm[1]) + v, IK::Point_3(rm[2]) + v, IK::Point_3(rm[3]) + v, IK::Point_3(rm[4]) + v };
	joint_volumes_pairA_pairB[1] = { IK::Point_3(rm[0]) - v, IK::Point_3(rm[1]) - v, IK::Point_3(rm[2]) - v, IK::Point_3(rm[3]) - v, IK::Point_3(rm[4]) - v };

	// jointArea0 = jointArea0;
	 //jointArea0.insert(jointArea0.end(), jointArea1.begin(), jointArea1.end());



	 //jointArea0 = rm.Translate(lMax.direction().Unit() * (-(1 + this.extend[2]) + 0.00) * maxLength);//For some reason extend by 1.5
	 //jointArea1 = rm.Translate(lMax.direction().Unit() * ((1 + this.extend[2]) + 0.00) * maxLength);//For some reason extend by 1.5

	if (GlobalExtend[0] + GlobalExtend[1] > 0) {
		CGAL_PolylineUtil::Extend(joint_volumes_pairA_pairB[0], 0, 0, 0, GlobalExtend[0], GlobalExtend[0]);
		CGAL_PolylineUtil::Extend(joint_volumes_pairA_pairB[0], 2, 0, 0, GlobalExtend[0], GlobalExtend[0]);
		CGAL_PolylineUtil::Extend(joint_volumes_pairA_pairB[0], 1, 0, 0, GlobalExtend[1], GlobalExtend[1]);
		CGAL_PolylineUtil::Extend(joint_volumes_pairA_pairB[0], 3, 0, 0, GlobalExtend[1], GlobalExtend[1]);

		CGAL_PolylineUtil::Extend(joint_volumes_pairA_pairB[1], 0, 0, 0, GlobalExtend[0], GlobalExtend[0]);
		CGAL_PolylineUtil::Extend(joint_volumes_pairA_pairB[1], 2, 0, 0, GlobalExtend[0], GlobalExtend[0]);
		CGAL_PolylineUtil::Extend(joint_volumes_pairA_pairB[1], 1, 0, 0, GlobalExtend[1], GlobalExtend[1]);
		CGAL_PolylineUtil::Extend(joint_volumes_pairA_pairB[1], 3, 0, 0, GlobalExtend[1], GlobalExtend[1]);
	}

	//For the sake of consistency
	//joint_volumes_pairA_pairB[2] = joint_volumes_pairA_pairB[0];
	//joint_volumes_pairA_pairB[3] = joint_volumes_pairA_pairB[1];


	////////////////////////////////////////////////////////////////////////////////
	////return plate joints
	////////////////////////////////////////////////////////////////////////////////
	//Joint pj = new Joint(this.joints.Count, x.key, y.key, -1, -1, new List<Polyline>{ jointArea0, jointArea1 }, jointLines, new OutlineType[]{ OutlineType.Side, OutlineType.Side }, CollisionType.PlaneFace);//OutlineType
	//this.joints.Add(pj);
	return true;


}


inline bool face_to_face(
	std::vector<CGAL_Polyline>& Polyline0,
	std::vector<CGAL_Polyline>& Polyline1,
	std::vector<IK::Plane_3>& Plane0,
	std::vector<IK::Plane_3>& Plane1,
	int& e0, int& e1,
	CGAL_Polyline& joint_area,
	//CGAL_Polyline(&joint_quads)[2],
	CGAL_Polyline(&joint_lines)[2],
	CGAL_Polyline(&joint_volumes_pairA_pairB)[4],
	int& type) {


	//printf("CPP StartIndersection \n");

	for (int i = 0; i < Plane0.size(); i++) {
		for (int j = 0; j < Plane1.size(); j++) {

			//Check if polygons are co-planar
			bool coplanar = CGAL_PlaneUtil::IsCoplanar(Plane0[i], Plane1[j], false, GlobalTolerance);//O(n*n) +10 ms

			//CGAL_Debug(coplanar);

			if (coplanar) {

				//Perform 2D Intersection 20 ms
				bool hasIntersection = intersection_2D(Polyline0[i], Polyline1[j], Plane0[i], joint_area, GlobalClipperScale);// +20 ms 10000.0; GlobalClipperScale

			   //////////////////////////////////////////////////////////////////////////////////////////////////
			   //Intersection lines and rectangles
			   //////////////////////////////////////////////////////////////////////////////////////////////////
				if (hasIntersection) {


					e0 = i;
					e1 = j;
					int type0 = i > 1 ? 0 : 1;
					int type1 = j > 1 ? 0 : 1;
					type = type0 + type1;

					//Get Line: Option A(0) - Side-Side i>2 && i > 2, Option B(1) - Side-Top i>2, C(2) Top-Top

					//jointArea1 = jointArea0;//default case (Top-top)
					IK::Segment_3 joint_line0;
					IK::Plane_3 averagePlane0(CGAL::midpoint(Polyline0[0][0], Polyline0[1][0]), Plane0[0].orthogonal_vector());//Center Plane
					CGAL_Polyline joint_quads0;
					if (i > 1) {//Side-Top  or Side-Side

						//Middle line for alignment
						IK::Segment_3 alignmentSegment(CGAL::midpoint(Polyline0[0][i - 2], Polyline0[1][i - 2]), CGAL::midpoint(Polyline0[0][i - 1], Polyline0[1][i - 1]));

						//Intersect: a) clipper region, b) center plane

						bool isLine = CGAL_PolylineUtil::PolylinePlane(joint_area, averagePlane0, alignmentSegment, joint_line0);

						//Planes to get a quad
						if (isLine) {
							bool isQuad = CGAL_PlaneUtil::QuadFromLineAndTopBottomPlanes(Plane0[i], joint_line0, Plane0[0], Plane0[1], joint_quads0);
							//joint_quads0.push_back(Polyline0[0][0]);
							//joint_quads0.push_back(Polyline0[1][0]);
							//joint_quads0.push_back(Plane0[i].point());
						}else {
							CGAL_Debug(joint_quads0.size());
							return false;
						}
					}

					//Intersect second time for the side-side connection

					IK::Segment_3 joint_line1;
					IK::Plane_3 averagePlane1(CGAL::midpoint(Polyline1[0][0], Polyline1[1][0]), Plane1[0].orthogonal_vector());//Center Plane
					CGAL_Polyline joint_quads1;
					if (j > 1) {//Side-Side

						//Middle line for alignment
						IK::Segment_3 alignmentSegment(CGAL::midpoint(Polyline1[0][i - 2], Polyline1[1][i - 2]), CGAL::midpoint(Polyline1[0][i - 1], Polyline1[1][i - 1]));

						bool isLine = CGAL_PolylineUtil::PolylinePlane(joint_area, averagePlane1, alignmentSegment, joint_line1);
					

						//Planes to get a quad
						if (isLine) {
							bool isQuad = CGAL_PlaneUtil::QuadFromLineAndTopBottomPlanes(Plane1[j], joint_line1, Plane1[0], Plane1[1],  joint_quads1);
	  					    //joint_volumes_pairA_pairB[0] = joint_quads1;
							//joint_volumes_pairA_pairB[1] = joint_quads1;
						} else {
							CGAL_Debug(joint_quads1.size());
							return false;
							continue;
						}

					}
					//Top-top

					//return true;
					////////////////////////////////////////////////////////////////////////////////
					//ToDo set edge direction - Check Insertion angle if �dge axis is assigned
					//Applies for both elements
					////////////////////////////////////////////////////////////////////////////////
					IK::Vector_3 dir;//not set
		
				
			

					//Only one direction is taken
					//dir = this.elements[j.f0].ev[j.e0 - 2];
					bool notSharpAngle = true;
					bool dirSet = dir.hx() > GlobalTolerance && dir.hy() > GlobalTolerance && dir.hz() > GlobalTolerance;// == CGAL::NULL_VECTOR;
					//CGAL_Debug(dirSet);
					if (dirSet) {
						if (CGAL_VectorUtil::GetAngle90(dir, Plane0[i].orthogonal_vector()) > 45) {
							notSharpAngle = false;
						}
					}

					//////////////////////////////////////////////////////////////////////////////////////////////////
					//Indentify connection volumes
					//////////////////////////////////////////////////////////////////////////////////////////////////
					//CGAL_Debug(type);
					if (type == 0) {//side-side

						////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						//Elements are rotated 
						////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						//CGAL_Debug(CGAL_VectorUtil::Distance(jointLine0[0], jointLine0[1]));
						//CGAL_Debug(CGAL_VectorUtil::Distance(jointLine1[0], jointLine1[1]));

						if (CGAL_VectorUtil::IsParallelTo(joint_line0[0] - joint_line0[1], joint_line1[0] - joint_line1[1], GlobalTolerance) == 0) {

							//joint_lines[0] = { joint_line0[0], joint_line0[1] };
							//joint_lines[1] = { joint_line1[0], joint_line1[1] };

							////////////////////////////////////////////////////////////////////////////////
							//Get average intersection line
							////////////////////////////////////////////////////////////////////////////////
							IK::Segment_3 average_segment = CGAL::has_smaller_distance_to_point(joint_line0[0], joint_line1[0], joint_line1[1]) ? IK::Segment_3(CGAL::midpoint(joint_line0[0], joint_line1[0]), CGAL::midpoint(joint_line0[1], joint_line1[1])) : IK::Segment_3(CGAL::midpoint(joint_line0[0], joint_line1[1]), CGAL::midpoint(joint_line0[1], joint_line1[0]));
							
							////////////////////////////////////////////////////////////////////////////////
							//Create Plane to XY transformation matrix
							////////////////////////////////////////////////////////////////////////////////
							IK::Vector_3 o (average_segment[0].hx(), average_segment[0].hy(), average_segment[0].hz());
							IK::Vector_3 x = average_segment.to_vector();//CGAL_VectorUtil::Unitize(x);
							IK::Vector_3 z = Plane0[i].orthogonal_vector();//CGAL_VectorUtil::Unitize(z);
							IK::Vector_3 y = CGAL::cross_product(x,z);//CGAL_VectorUtil::Unitize(y);
							CGAL::Aff_transformation_3<IK> xform = CGAL_XFormUtil::VectorsToXY(o, x, y, z);

							////////////////////////////////////////////////////////////////////////////////
							//Decide min or max rectangle
							////////////////////////////////////////////////////////////////////////////////
							CGAL_Polyline joint_area_copy;
							bool min = true;
							if (min) 
								CGAL_PolylineUtil::Duplicate(joint_area, joint_area_copy);
							else {
								joint_area_copy.insert(joint_area_copy.end(), std::begin(Polyline0[i]), std::end(Polyline0[i]));
								joint_area_copy.insert(joint_area_copy.end(), std::begin(Polyline1[j]), std::end(Polyline1[j]));
							}
							
							////////////////////////////////////////////////////////////////////////////////
							//2D - Get boundary ractangle
							////////////////////////////////////////////////////////////////////////////////
							CGAL_PolylineUtil::Transform(joint_area_copy, xform);
							auto AABB = CGAL::bbox_3(joint_area_copy.begin(), joint_area_copy.end(), IK());
							IK::Segment_3 segmentX(IK::Point_3(AABB.xmin(), AABB.ymin(), AABB.zmin()), IK::Point_3(AABB.xmax(), AABB.ymin(), AABB.zmin()));
							IK::Segment_3 segmentY(IK::Point_3(AABB.xmin(), AABB.ymin(), AABB.zmin()), IK::Point_3(AABB.xmin(), AABB.ymax(), AABB.zmin()));
							CGAL_Polyline average_rectangle = { segmentX[0],segmentX[1],segmentX[0] + segmentX.to_vector() + segmentY.to_vector(),segmentY[1] };
							
							////////////////////////////////////////////////////////////////////////////////
							//3D Orient to 3D
							////////////////////////////////////////////////////////////////////////////////
							CGAL::Aff_transformation_3<IK> xformInv = xform.inverse();
							CGAL_PolylineUtil::Transform(average_rectangle, xformInv);

							////////////////////////////////////////////////////////////////////////////////
							//Create Joint rectangles by an offset of element thickness
							////////////////////////////////////////////////////////////////////////////////
							IK::Vector_3 offset_vector = z;
							if (dirSet) {
								offset_vector = dir;
								CGAL_Debug((dir.x() + dir.y() + dir.z()));
							}

							CGAL_VectorUtil::Unitize(offset_vector);
							double d0 = 0.5*std::sqrt(CGAL::squared_distance(Plane0[0].point(), Plane0[1].projection(Plane0[0].point())));
							offset_vector *= d0;

							joint_volumes_pairA_pairB[0] =  {  average_rectangle[3] + offset_vector, average_rectangle[3] - offset_vector, average_rectangle[0] - offset_vector, average_rectangle[0] + offset_vector, average_rectangle[0] + offset_vector };
							joint_volumes_pairA_pairB[1] = {average_rectangle[2] + offset_vector,average_rectangle[2] - offset_vector,average_rectangle[1] - offset_vector,average_rectangle[1] + offset_vector,  average_rectangle[1] + offset_vector };
							//joint_volumes_pairA_pairB[2] = joint_volumes_pairA_pairB[0];
							//joint_volumes_pairA_pairB[3] = joint_volumes_pairA_pairB[1];


						////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						//Elements are parallel
						////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						} else {

							//joint_lines[0] = { joint_line0[0], joint_line0[1] };
							//joint_lines[1] = { joint_line1[0], joint_line1[1] };

							////////////////////////////////////////////////////////////////////////////////
							//Get Overlap-Line // scale it down ?
							////////////////////////////////////////////////////////////////////////////////
							IK::Segment_3 lJ;
							CGAL_PolylineUtil::LineLineOverlapAverage(joint_line0, joint_line1, lJ);
							joint_lines[0] = { lJ[0],lJ[1] };
							joint_lines[1] = joint_lines[0];

							////////////////////////////////////////////////////////////////////////////////
							//Get Plane perpedicular to overlap-axis //with or without insertion vector |-----------|
							////////////////////////////////////////////////////////////////////////////////

							IK::Plane_3 plEnd0 = IK::Plane_3(lJ[0], lJ.to_vector());// averagePlane0.orthogonal_vector(), CGAL::cross_product(lJ.to_vector(), averagePlane0.orthogonal_vector()));

							if (dirSet)
								IK::Plane_3(lJ[0], CGAL::cross_product(dir, CGAL::cross_product(lJ.to_vector(), dir)));
					

							IK::Plane_3 plEnd1(lJ[1], plEnd0.orthogonal_vector());
							IK::Plane_3 pl_mid(CGAL::midpoint(lJ[0], lJ[1]), plEnd0.orthogonal_vector());

							////////////////////////////////////////////////////////////////////////////////
							//Get dihedral angle to a) in-plane b) out-of-plane c) not-valid sharper than 20 deg
							//returns an approximation of the signed dihedral angle in the tetrahedron pqrs of edge pq.
							//The sign is negative if orientation(p, q, r, s) is CGAL::NEGATIVEand positive otherwise.The angle is given in degrees.
							////////////////////////////////////////////////////////////////////////////////
							IK::Point_3 centerPoint0 = averagePlane0.projection(CGAL_PolylineUtil::Center(Polyline0[0]));
							IK::Point_3 centerPoint1 = averagePlane1.projection(CGAL_PolylineUtil::Center(Polyline1[0]));
							auto dihedralAngle = std::abs(CGAL::approximate_dihedral_angle(lJ[0], lJ[1], centerPoint0, centerPoint1));

							if (dihedralAngle < 20) {//160
								return false;


							}
							else if (dihedralAngle <= 150) {//OUT-OF-PLANE // && jointArea0.size()>0

							 ////////////////////////////////////////////////////////////////////////////////
							 //Rotate line-joint 90 degrees and intersect with adjacent element top and bottom planes
							 //This is needed to check if adjacent element top and bottom planes are in the same or opposite order
							 ////////////////////////////////////////////////////////////////////////////////
								IK::Vector_3 connectionNormal = Plane0[i].orthogonal_vector();
								IK::Vector_3 lJ_normal = lJ.to_vector();
								IK::Vector_3 lJ_v_90 = CGAL::cross_product(lJ_normal, connectionNormal) * 0.5;
								IK::Line_3 lj_l_90(lJ[0], lJ_v_90);

								IK::Point_3 pl0_0_p = CGAL_PlaneUtil::LinePlane(lj_l_90, Plane0[0]);
								IK::Point_3 pl1_0_p = CGAL_PlaneUtil::LinePlane(lj_l_90, Plane1[0]);
								IK::Point_3 pl1_1_p = CGAL_PlaneUtil::LinePlane(lj_l_90, Plane1[1]);

								IK::Plane_3 planes[4];
								planes[1] = Plane0[0];

								if (CGAL::has_larger_distance_to_point(pl0_0_p, pl1_0_p, pl1_1_p)) {
									planes[2] = Plane1[0];
									planes[3] = Plane0[1];
									planes[0] = Plane1[1];
								}
								else {
									planes[2] = Plane1[1];
									planes[3] = Plane0[1];
									planes[0] = Plane1[0];
								}

								////////////////////////////////////////////////////////////////////////////////
								//Intersect End plane |-----------------------| with top and bottom planes
								////////////////////////////////////////////////////////////////////////////////
								joint_volumes_pairA_pairB[0] = CGAL_PlaneUtil::plane_4_planes(plEnd0, planes);
								joint_volumes_pairA_pairB[1] = CGAL_PlaneUtil::plane_4_planes(plEnd1, planes);
								//joint_volumes_pairA_pairB[2] = { joint_volumes_pairA_pairB[0][3],joint_volumes_pairA_pairB[0][0],joint_volumes_pairA_pairB[0][1],joint_volumes_pairA_pairB[0][2] };
								//joint_volumes_pairA_pairB[3] = { joint_volumes_pairA_pairB[1][3],joint_volumes_pairA_pairB[1][0],joint_volumes_pairA_pairB[1][1],joint_volumes_pairA_pairB[1][2] };

							}
							else {//IN-PLANE

							//joint_lines[0] = { joint_line0[0], joint_line0[1] };
							//joint_lines[1] = { joint_line1[0], joint_line1[1] };

							 ////////////////////////////////////////////////////////////////////////////////
							 //Intersect current top and bottom element planes, including the offseted joint face planes with |......................| end planes
							 ////////////////////////////////////////////////////////////////////////////////
								double d0 = 0.5*std::sqrt(CGAL::squared_distance(Plane0[0].point(), Plane0[1].projection(Plane0[0].point())));
								IK::Plane_3 offset_plane_0 = CGAL_PlaneUtil::offset(Plane0[i], -d0);
								IK::Plane_3 offset_plane_1 = CGAL_PlaneUtil::offset(Plane0[i], d0);

								IK::Plane_3 loopOfPlanes0[4] = { offset_plane_0, Plane0[0], offset_plane_1, Plane0[1], };
								IK::Plane_3 loopOfPlanes1[4] = { offset_plane_0, Plane1[0], offset_plane_1, Plane1[1], };

								////////////////////////////////////////////////////////////////////////////////
								//Intersect End plane |-----------------------| with top and bottom planes
								////////////////////////////////////////////////////////////////////////////////
								joint_volumes_pairA_pairB[0] = CGAL_PlaneUtil::plane_4_planes(plEnd0, loopOfPlanes0);
								joint_volumes_pairA_pairB[1] = CGAL_PlaneUtil::plane_4_planes(plEnd1, loopOfPlanes0);
								joint_volumes_pairA_pairB[2] = CGAL_PlaneUtil::plane_4_planes(plEnd0, loopOfPlanes1);
								joint_volumes_pairA_pairB[3] = CGAL_PlaneUtil::plane_4_planes(plEnd1, loopOfPlanes1);
							}


						}


					}
					else if (type == 1) {//top-side
						joint_lines[0] = { joint_line0[0] ,joint_line0[1] };
						joint_lines[1] = { joint_line1[0] ,joint_line1[1] };

						//////////////////////////////////////////////////////////////////////////////////
						//Which element is male or female?
						//////////////////////////////////////////////////////////////////////////////////
						bool male_or_female = i > j;

						IK::Plane_3* plane0_0 = male_or_female ? &Plane0[0] : &Plane1[0];
						//IK::Plane_3* plane0_1 = male_or_female ? &Plane0[1] : &Plane1[1];
						IK::Plane_3* plane1_0 = !male_or_female ? &Plane0[i] : &Plane1[j];//female collision plane
						IK::Plane_3* plane1_1 = !male_or_female ? &Plane0[std::abs(i-1)] : &Plane1[std::abs(j-1)];

						//CGAL_Polyline* pline0 = male_or_female ? &Polyline0[0] : &Polyline1[0];
						//CGAL_Polyline* pline1 = !male_or_female ? &Polyline0[1] : &Polyline1[1];

						//IK::Segment_3* line_0 = male_or_female ? &joint_line0 : &joint_line1;//male, female does not exist int top-side
						CGAL_Polyline* quad_0 = male_or_female ? &joint_quads0 : &joint_quads1;//male, female does not exist int top-side

						
						//////////////////////////////////////////////////////////////////////////////////////
						//Two possibilities: if dir is unset move it perpendicularly else move by element direction
						//This case will only work for top-side connection when elements are parallell
						//For other cases you need to find a way to get opposite plane i.e. mesh intersection
						//////////////////////////////////////////////////////////////////////////////////////
						IK::Vector_3 offset_vector = CGAL_PlaneUtil::orthogonal_vector_between_two_plane_pairs(*plane0_0, *plane1_0, *plane1_1);
					
						//if (dirSet) {
							//offset_vector = CGAL_PlaneUtil::vector_two_planes(dir, *plane1_0, *plane1_1);
						//}


						//////////////////////////////////////////////////////////////////////////////////
						//Create Connection rectangles
						//////////////////////////////////////////////////////////////////////////////////
						joint_volumes_pairA_pairB[0] = { (*quad_0)[0], (*quad_0)[1], (*quad_0)[1]+offset_vector, (*quad_0)[0] + offset_vector, (*quad_0)[0] };
						joint_volumes_pairA_pairB[1] = { (*quad_0)[3], (*quad_0)[2], (*quad_0)[2] + offset_vector, (*quad_0)[3] + offset_vector, (*quad_0)[3] };

						//joint_volumes_pairA_pairB[2] = joint_volumes_pairA_pairB[0];
						//joint_volumes_pairA_pairB[3] = joint_volumes_pairA_pairB[1];
						
						return true;
					}
					else {
						return true;
					}



					return true;
				}//Has Intersection
			}//Co-planar

		}//for j
	}//for i

	return false;

}


inline void search(

	//Input
	std::vector<CGAL::Bbox_3>& AABB,
	std::vector<IK::Vector_3[5]>& OOBs,
	std::vector<std::vector<CGAL_Polyline>>& P,
	std::vector<std::vector<IK::Plane_3>>& Pl,
	std::vector<int>& pairs,
	int searchType,

	//Output
	std::vector<int>& outputPairs, //Contact Element ID and Local ID
	std::vector<CGAL_Polyline>& outputContactPolylines //Contact Areas + Lines




) {



	//////////////////////////////////////////////////////////////////////////////
	// Search Contact zones
	//////////////////////////////////////////////////////////////////////////////

	for (int i = 0; i < pairs.size(); i += 2) {

		CGAL_Polyline joint_area;
		CGAL_Polyline joint_quads[2];
		CGAL_Polyline joint_lines[2];
		CGAL_Polyline joint_volumes_pairA_pairB[4];
		int e0, e1, type;

		bool flag = true;
		switch (searchType) {
		case(0):
			flag = face_to_face(P[pairs[i]], P[pairs[i + 1]], Pl[pairs[i]], Pl[pairs[i + 1]], e0, e1, joint_area, joint_lines, joint_volumes_pairA_pairB, type);
			break;
		case(1):
			flag = plane_to_face(P[pairs[i]], P[pairs[i + 1]], Pl[pairs[i]], Pl[pairs[i + 1]], e0, e1, joint_area, joint_lines, joint_volumes_pairA_pairB, type);
			break;
		}

		if (!flag) continue;

		if (e0 != e1) {

			outputPairs.push_back(pairs[i]);
			outputPairs.push_back(pairs[i + 1]);
			outputPairs.push_back(e0);
			outputPairs.push_back(e1);

			outputContactPolylines.push_back(joint_area);
			outputContactPolylines.insert(outputContactPolylines.end(), std::begin(joint_quads), std::end(joint_quads));
			outputContactPolylines.insert(outputContactPolylines.end(), std::begin(joint_lines), std::end(joint_lines));
			outputContactPolylines.insert(outputContactPolylines.end(), std::begin(joint_volumes_pairA_pairB), std::end(joint_volumes_pairA_pairB));

		}
	}




}




inline void rtree_search(

	//Input
	std::vector<element>& elements,
	//std::vector<CGAL::Bbox_3>& AABB,
	//std::vector<IK::Vector_3[5]>& OOBs,
	//std::vector<std::vector<CGAL_Polyline>>& P,
	//std::vector<std::vector<IK::Plane_3>>& Pl,
	int search_type,

	//Output
	std::vector<joint>& joints
	//std::vector<std::vector<std::pair<int, int>>>& elements_joints_address
	//std::vector<int>& outputPairs, //Contact Element ID and Local ID
	//std::vector<CGAL_Polyline>& outputContactPolylines //Contact Areas + Lines


) {

	//////////////////////////////////////////////////////////////////////////////
	// Create RTree
	//////////////////////////////////////////////////////////////////////////////

	RTree<int, double, 3> tree;

	//////////////////////////////////////////////////////////////////////////////
	// Insert AABB
	//////////////////////////////////////////////////////////////////////////////

	for (int i = 0; i < elements.size(); i++) {
		double min[3] = { elements[i].aabb.xmin(), elements[i].aabb.ymin(), elements[i].aabb.zmin() };
		double max[3] = { elements[i].aabb.xmax(), elements[i].aabb.ymax(), elements[i].aabb.zmax() };
		tree.Insert(min, max, i);
	}

	//////////////////////////////////////////////////////////////////////////////
	// Search Closest Boxes | Skip duplicates pairs | Perform callback with OBB
	//////////////////////////////////////////////////////////////////////////////
	std::vector<int> result;
	for (int i = 0; i < elements.size(); i++) {//AABB.size()

		//std::vector<int> result;
		auto callback = [&result, i, &elements](int foundValue) -> bool {

			if (i < foundValue && CGAL_BoxUtil::GetCollision(elements[i].oob, elements[foundValue].oob)) {

				//CGAL_Debug(i);
				//CGAL_Debug(foundValue);

				result.push_back(i);
				result.push_back(foundValue);
			}
			return true;
		};

		double min[3] = { elements[i].aabb.xmin(), elements[i].aabb.ymin(), elements[i].aabb.zmin() };
		double max[3] = { elements[i].aabb.xmax(), elements[i].aabb.ymax(), elements[i].aabb.zmax() };
		int nhits = tree.Search(min, max, callback);//callback in this method call callback above

	}



	//////////////////////////////////////////////////////////////////////////////
	// Search Contact zones
	//////////////////////////////////////////////////////////////////////////////
	int jointID = 0;
	for (int i = 0; i < result.size(); i += 2) {

		CGAL_Polyline joint_area;
		CGAL_Polyline joint_quads[2];
		CGAL_Polyline joint_lines[2];
		CGAL_Polyline joint_volumes_pairA_pairB[4];
		int e0, e1, type;

		bool flag = true;
		switch (search_type) {
		case(0):
			flag = face_to_face(elements[result[i]].polylines, elements[result[i + 1]].polylines, elements[result[i]].planes, elements[result[i + 1]].planes, e0, e1, joint_area, joint_lines, joint_volumes_pairA_pairB, type);
			break;
		case(1):
			flag = plane_to_face(elements[result[i]].polylines, elements[result[i + 1]].polylines, elements[result[i]].planes, elements[result[i + 1]].planes, e0, e1, joint_area, joint_lines, joint_volumes_pairA_pairB, type);
			break;
		}

		if (!flag) continue;

		if (joint_area.size() > 0) {


			joints.emplace_back(
				jointID,
				result[i], result[i + 1],
				e0, e1,
				joint_area,
				joint_lines,
				joint_volumes_pairA_pairB
			);

			elements[result[i + 0]].jointID_malefemale[e0] = (std::pair<int,bool>(jointID, true));
			elements[result[i + 1]].jointID_malefemale[e1] = (std::pair<int, bool>(jointID, false));

			jointID++;
		}

	

	}

	//CGAL_Debug(9999999);
	//CGAL_Debug(joints.size());

}

inline void get_obb_and_planes(
	//Input
	std::vector<CGAL_Polyline>& pp,
	//Output
	std::vector< CGAL::Bbox_3>& AABBs,
	std::vector<IK::Vector_3[5]>& OOBs,
	std::vector< std::vector<CGAL_Polyline>>& P,
	std::vector< std::vector<IK::Plane_3>>& Pls) {

	int n = pp.size() * 0.5;

	AABBs = std::vector<CGAL::Bbox_3>(n);
	OOBs = std::vector<IK::Vector_3[5]>(n);
	P = std::vector<std::vector<CGAL_Polyline>>(n);
	Pls = std::vector<std::vector<IK::Plane_3>>(n);



	for (int i = 0; i < pp.size(); i += 2) {

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//Get BoundingBox - AABB
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//Create copy of a polyline and transform points
		CGAL_Polyline twoPolylines;
		twoPolylines.resize(pp[i].size() + pp[i + 1].size());
		std::copy(pp[i].begin(), pp[i].end(), twoPolylines.begin());
		std::copy(pp[i + 1].begin(), pp[i + 1].end(), twoPolylines.begin() + pp[i].size());

		//auto t = CGAL_XFormUtil::Scale(CGAL_PolylineUtil::Center(twoPolylines), 1+ GlobalTolerance);//change when dilate works
		//CGAL_PolylineUtil::Transform(twoPolylines, t);//change when dilate works

		CGAL::Bbox_3 AABB = CGAL::bbox_3(twoPolylines.begin(), twoPolylines.end(), IK());

		CGAL_Polyline AABB_Min_Max = {
			IK::Point_3(AABB.xmin() - 1 * GlobalTolerance, AABB.ymin() - 1 * GlobalTolerance, AABB.zmin() - 1 * GlobalTolerance),
			IK::Point_3(AABB.xmax() + 1 * GlobalTolerance, AABB.ymax() + 1 * GlobalTolerance, AABB.zmax() + 1 * GlobalTolerance),
		};

		AABB = CGAL::bbox_3(AABB_Min_Max.begin(), AABB_Min_Max.end(), IK());

		//CGAL_Debug(AABB.zmax());
		//AABB.dilate(1E34);
		//CGAL_Debug(AABB.zmax());
		//AABB = CGAL::bbox_3()
		//AABB.dilate(GlobalTolerance);//Does not work
		AABBs[i * 0.5] = AABB;

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//Get Object Oriented BoundingBox - OOB -> transform to XY and back to 3D
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//Create Planes
		IK::Vector_3 planeAxes[4];
		CGAL_PolylineUtil::AveragePlane(pp[i], planeAxes);

		//Create Transformation
		CGAL::Aff_transformation_3<IK> xform_toXY = CGAL_XFormUtil::VectorsToXY(planeAxes[0], planeAxes[1], planeAxes[2], planeAxes[3]);
		CGAL::Aff_transformation_3<IK> xform_toXY_Inv = xform_toXY.inverse();

		//Transform the merged polyline to xy and compute xyBounding Box

		for (auto it = twoPolylines.begin(); it != twoPolylines.end(); ++it) {
			*it = it->transform(xform_toXY);
			//printf("CPP Transformed Point %d %d %d \n", it->x(), it->y(), it->z());
		}

		//Compute bounding box, get center point, and contruct 5 size vector, where 5th dimension is halfsite,  then transform box back to 3D by an inverse matrix
		CGAL::Bbox_3 AABBXY = CGAL::bbox_3(twoPolylines.begin(), twoPolylines.end(), IK());
		double scale = 10;
		IK::Vector_3 box[5] = {
			IK::Vector_3(
				(AABBXY.xmin() + AABBXY.xmax()) * 0.5,
				(AABBXY.ymin() + AABBXY.ymax()) * 0.5,
				(AABBXY.zmin() + AABBXY.zmax()) * 0.5),
			IK::Vector_3(1,0,0),
			IK::Vector_3(0,1,0),
			IK::Vector_3(0,0,1),
			IK::Vector_3(
			fabs((1.0 + GlobalTolerance * 1) * (AABBXY.xmax() - AABBXY.xmin()) * 0.5),//0.00001
			fabs((1.0 + GlobalTolerance * 1) * (AABBXY.ymax() - AABBXY.ymin()) * 0.5),
			fabs((1.0 + GlobalTolerance * 1) * (AABBXY.zmax() - AABBXY.zmin()) * 0.5))
		};
		//CGAL_Debug(1.0 + GlobalTolerance * 0.00001);




		CGAL_BoxUtil::TransformPlaneOrPlane(box, xform_toXY_Inv);
		CGAL_BoxUtil::Assign(box, OOBs[i * 0.5], 5);

		//CGAL_Debug(box[0]);
		//CGAL_Debug(box[1]);
		//CGAL_Debug(box[2]);
		//CGAL_Debug(box[3]);
		//CGAL_Debug(box[4]);


		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//Check orientation of polylines and reverse if needed
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		if (twoPolylines.back().z() > 0) {
			std::reverse(pp[i].begin(), pp[i].end());
			std::reverse(pp[i + 1].begin(), pp[i + 1].end());
		}


		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//Get Side Polylines and Planes
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		P[i * 0.5] = std::vector<CGAL_Polyline>(1 + pp[i].size());
		P[i * 0.5][0] = pp[i];
		P[i * 0.5][1] = pp[i + 1];

		Pls[i * 0.5] = std::vector<IK::Plane_3>(1 + pp[i].size());


		//IK::Point_3 origin = CGAL_PolylineUtil::Center(pp[i]);
		IK::Vector_3 normal;
		CGAL_VectorUtil::AverageNormal(pp[i], normal,true,false);
		Pls[i * 0.5][0] = IK::Plane_3(CGAL_PolylineUtil::Center(pp[i]), normal);
		Pls[i * 0.5][1] = IK::Plane_3(CGAL_PolylineUtil::Center(pp[i + 1]), -normal);

		for (int j = 0; j < pp[i].size() - 1; j++) {
			Pls[i * 0.5][2 + j] = IK::Plane_3(pp[i][j + 1], pp[i][j], pp[i + 1][j + 1]);
			P[i * 0.5][2 + j] = { pp[i][j],  pp[i][j + 1], pp[i + 1][j + 1], pp[i + 1][j], pp[i][j] };
		}


	}



}


//inline bool OrientTile(CGAL_Polyline &rect0, CGAL_Polyline &rect1, std::vector<std::vector<CGAL_Polyline>> &orientedTiles, int tileType = 0) {
//
//    if (rect0.size() != 5 || rect1.size() != 5)
//        return false;
//
//    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    //Get a Tile
//    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    std::vector<CGAL_Polyline> tile = CGAL_PolylineUtil::Cross_Simple();
//
//    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    //Transform a tile
//    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    for (int i = 0; i < tile.size(); i++) {
//        CGAL::Aff_transformation_3<IK> X;
//
//        CGAL_XFormUtil::ChangeBasis(
//            IK::Point_3(-0.5, -0.5, -0.5), IK::Vector_3(1, 0, 0), IK::Vector_3(0, 1, 0), IK::Vector_3(0, 0, 1),
//            rect0[0], (rect0[1] - rect0[0]), (rect0[3] - rect0[0]), (rect1[0] - rect0[0]),
//            X
//        );
//
//        CGAL_PolylineUtil::Transform(tile[i], X);
//
//    }
//
//    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    //Output
//    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    orientedTiles.push_back(tile);
//    return true;
//}













