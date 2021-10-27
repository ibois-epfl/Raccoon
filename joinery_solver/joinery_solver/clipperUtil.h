//#pragma once
//#include "compas.h"
//#include "CGAL_XFormUtil.h"
//#include "CGAL_PolylineUtil.h"
//#include "clipper.h"
//
//
//
//
//
//
//
//inline void IsPointPairInside(CGAL_Polyline &polyline, IK::Plane_3 &plane, IK::Point_3 (&testPoints)[2] , bool(&testPointsFlag)[2], double scale = 100000.0) {
//
//    /////////////////////////////////////////////////////////////////////////////////////
//    //Orient from 3D to 2D
//    /////////////////////////////////////////////////////////////////////////////////////
//    CGAL_Polyline a;
//    CGAL_PolylineUtil::Duplicate(polyline, a);
//
//    IK::Point_3 pts[2];
//    pts[0] = IK::Point_3(testPoints[0].x(), testPoints[0].y(), testPoints[0].z());
//    pts[1] = IK::Point_3(testPoints[1].x(), testPoints[1].y(), testPoints[1].z());
//
//    /////////////////////////////////////////////////////////////////////////////////////
//    //Create Transformation - Orient to XY
//    /////////////////////////////////////////////////////////////////////////////////////
//    CGAL::Aff_transformation_3<IK> xform_toXY = CGAL_XFormUtil::PlaneToXY(polyline[0], plane);
//    CGAL_PolylineUtil::Transform(a, xform_toXY);
//    pts[0] = xform_toXY.transform(pts[0]);
//    pts[1] = xform_toXY.transform(pts[1]);
//
//    /////////////////////////////////////////////////////////////////////////////////////
//    //Convert to Clipper
//    /////////////////////////////////////////////////////////////////////////////////////
//    std::vector< ClipperLib::IntPoint > pathA(a.size() - 1);
//    for (int i = 0; i < a.size() - 1; i++) {
//        pathA[i] = ClipperLib::IntPoint(a[i].x() * scale, a[i].y() * scale);
//        //printf("%f,%f,%f \n", a[i].x(), a[i].y(), a[i].z());
//    }
//
//    auto p0 = ClipperLib::IntPoint(pts[0].x() * scale, pts[0].y() * scale);
//    auto p1 = ClipperLib::IntPoint(pts[1].x() * scale, pts[1].y() * scale);
//
    /////////////////////////////////////////////////////////////////////////////////////
////    //Check if point is inside polyline
//    /////////////////////////////////////////////////////////////////////////////////////
//    testPointsFlag[0] = ClipperLib::PointInPolygon(p0, pathA) != 0;
//    testPointsFlag[1] = ClipperLib::PointInPolygon(p1, pathA) != 0;
//
//
//}
