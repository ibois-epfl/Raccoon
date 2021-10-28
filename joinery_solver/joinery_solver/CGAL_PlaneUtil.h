#pragma once
#include "compas.h"
#include "CGAL_IntersectionUtil.h"


namespace CGAL_PlaneUtil {





    inline IK::Vector_3 vector_two_planes(IK::Vector_3& v, IK::Plane_3& plane0, IK::Plane_3& plane1) {
        //printf("vector_two_planes \n");
        //IK::Line_3 line(IK::Point_3(0, 0, 0), v);

        //auto result0 = CGAL::intersection(plane0, line);
        //auto result1 = CGAL::intersection(plane1, line);

        //auto p0 = *boost::get<IK::Point_3>(&*result0);
        //auto p1 = *boost::get<IK::Point_3>(&*result1);

        //return p1 - p0;



        IK::Segment_3 line(IK::Point_3(0, 0, 0), IK::Point_3(v.hx(), v.hy(), v.hz()));
        IK::Point_3 p0;
        IK::Point_3 p1;
        CGAL_IntersectionUtil::Intersect(line, plane0, p0);
        CGAL_IntersectionUtil::Intersect(line, plane1, p1);
        //return p1 - p0;
    }

    inline IK::Vector_3 orthogonal_vector_between_two_plane_pairs(IK::Plane_3& plane_pair0_0, IK::Plane_3& plane_pair1_0, IK::Plane_3& plane_pair1_1) {


    
        IK::Line_3 l0;
        IK::Line_3 l1;
        CGAL_IntersectionUtil::Intersect(plane_pair0_0, plane_pair1_0, l0);
        CGAL_IntersectionUtil::Intersect(plane_pair0_0, plane_pair1_1, l1);

        return l1.point() - l0.projection(l1.point());

    }

    inline IK::Plane_3 offset(IK::Plane_3& mainPlane, double d) {

        IK::Vector_3 normal = mainPlane.orthogonal_vector();
        CGAL_VectorUtil::Unitize(normal);
        normal *= d;

        return IK::Plane_3(mainPlane.point() + normal, normal);
    }

    inline CGAL_Polyline plane_4_planes(IK::Plane_3& mainPlane, IK::Plane_3(&sequence_of_planes)[4]) {

        //auto result0 = CGAL::intersection(sequence_of_planes[0], sequence_of_planes[1], mainPlane);
        //auto result1 = CGAL::intersection(sequence_of_planes[1], sequence_of_planes[2], mainPlane);
        //auto result2 = CGAL::intersection(sequence_of_planes[2], sequence_of_planes[3], mainPlane);
        //auto result3 = CGAL::intersection(sequence_of_planes[3], sequence_of_planes[0], mainPlane);

        //return {
        // *boost::get<IK::Point_3>(&*result0),
        //*boost::get<IK::Point_3>(&*result1),
        //*boost::get<IK::Point_3>(&*result2),
        //*boost::get<IK::Point_3>(&*result3),
        //*boost::get<IK::Point_3>(&*result0)
        //};

        IK::Point_3 p0; CGAL_IntersectionUtil::Intersect(sequence_of_planes[0], sequence_of_planes[1], mainPlane, p0);
        IK::Point_3 p1; CGAL_IntersectionUtil::Intersect(sequence_of_planes[1], sequence_of_planes[2], mainPlane, p1);
        IK::Point_3 p2; CGAL_IntersectionUtil::Intersect(sequence_of_planes[2], sequence_of_planes[3], mainPlane, p2);
        IK::Point_3 p3; CGAL_IntersectionUtil::Intersect(sequence_of_planes[3], sequence_of_planes[0], mainPlane, p3);
        return { p0 ,p1,p2,p3,p0 };

    }

    inline IK::Point_3 LinePlane(IK::Line_3& line, IK::Plane_3& plane) {

        IK::Point_3 point; CGAL_IntersectionUtil::Intersect(IK::Segment_3(line.point(),line.point()+line.to_vector()), plane, point);

        //auto result = CGAL::intersection(line, plane);
        ///*    if (!result)
        //        CGAL_Debug(0);*/
        //auto point = *boost::get<IK::Point_3>(&*result);
        return point;

    }

    inline IK::Point_3 SegmentPlane(IK::Segment_3& segment, IK::Plane_3& plane) {

        IK::Point_3 point; CGAL_IntersectionUtil::Intersect(segment, plane, point);
        /* if (!result)
             CGAL_Debug(0);*/
        //auto point = *boost::get<IK::Point_3>(&*result);
        return point;

    }
    //inline void PlaneFromFourPoints(IK::Point_3& a, IK::Point_3& b, IK::Point_3& c, IK::Point_3& d, IK::Vector_3(&planeAxes)[4]) {
    //    // a----b
    //    // c----d
    //    MidVectorFromPoints(a, d, planeAxes[0]);

    //    planeAxes[1] = b - a;
    //    CGAL_VectorUtil::Unitize(planeAxes[1]);

    //    planeAxes[2] = a - c;
    //    CGAL_VectorUtil::Unitize(planeAxes[2]);

    //    planeAxes[3] = CGAL::cross_product(planeAxes[1], planeAxes[2]);
    //    CGAL_VectorUtil::Unitize(planeAxes[3]);

    //    planeAxes[2] = CGAL::cross_product(planeAxes[3], planeAxes[1]);
    //    CGAL_VectorUtil::Unitize(planeAxes[2]);
    //}


    inline IK::Vector_3 PointAt2(IK::Vector_3(&input)[4], double s, double t)
    {
        return (input[0] + s * input[1] + t * input[2]);
    }

    inline IK::Vector_3 PointAt(IK::Vector_3(&input)[4], double s, double t, double c)
    {

        return (input[0] + s * input[1] + t * input[2] + c * input[3]);
    }

    inline CGAL_Polyline PlaneToLine(IK::Point_3& point, IK::Plane_3& plane, double x = 1, double y = 1, double z = 1) {



        CGAL_Polyline axes(2);
        IK::Point_3 center = point;
        IK::Vector_3 n = plane.orthogonal_vector();
        CGAL_VectorUtil::Unitize(n);
        n *= x;
        IK::Point_3 normal = point + n;// +
        axes[0] = center;
        axes[1] = normal;

        //axes[0] = (CGAL_VectorUtil::ToPoint(input[0]));
        //axes[1] = (CGAL_VectorUtil::ToPoint(input[0]) + input[1] * x * 1.25);
        //axes[2] = (CGAL_VectorUtil::ToPoint(input[0]));
        //axes[3] = (CGAL_VectorUtil::ToPoint(input[0]) + input[2] * y);
        //axes[4] = (CGAL_VectorUtil::ToPoint(input[0]));
        //axes[5] = (CGAL_VectorUtil::ToPoint(input[0]) + input[3] * z);

        return axes;

    }


    inline CGAL_Polyline PlaneToPolyline(IK::Plane_3& plane, double x = 1, double y = 1, double z = 1) {

        auto point = plane.point();
        auto X0 = plane.base1();
        auto Y0 = plane.base2();
        auto Z0 = plane.orthogonal_vector();
        CGAL_VectorUtil::Unitize(X0);
        CGAL_VectorUtil::Unitize(Y0);
        CGAL_VectorUtil::Unitize(Z0);

        CGAL_Polyline axes(6);

        axes[0] = (point);
        axes[1] = (point + X0 * x * 1.25);
        axes[2] = (point);
        axes[3] = (point + Y0 * y);
        axes[4] = (point);
        axes[5] = (point + Z0 * z);

        return axes;

    }



    inline CGAL_Polyline PlaneToPolyline(IK::Vector_3(&input)[4], double x = 1, double y = 1, double z = 1) {

        CGAL_Polyline axes(6);

        axes[0] = (CGAL_VectorUtil::ToPoint(input[0]));
        axes[1] = (CGAL_VectorUtil::ToPoint(input[0]) + input[1] * x * 1.25);
        axes[2] = (CGAL_VectorUtil::ToPoint(input[0]));
        axes[3] = (CGAL_VectorUtil::ToPoint(input[0]) + input[2] * y);
        axes[4] = (CGAL_VectorUtil::ToPoint(input[0]));
        axes[5] = (CGAL_VectorUtil::ToPoint(input[0]) + input[3] * z);

        return axes;

    }

    inline void Assign(IK::Vector_3(&source)[4], IK::Vector_3(&target)[4], int n) {
        for (int i = 0; i < n; i++)
            target[i] = source[i];
    }


    //inline ON_Plane PlaneFromLine(ON_Line& l, double t = 0) {
    //    ON_Plane pl(l.PointAt(t), l.Direction());
    //    return pl;
    //}

    //inline bool QuadFromLineAndTopBottomPlanes(ON_Plane& collisionFace, ON_Plane& pl0, ON_Plane& pl1, ON_Line& l, ON_Polyline& quad) {

    //    //Get two perpendicular planes of a line
    //    ON_Plane linePlane0 = PlaneFromLine(l, 0);
    //    ON_Plane linePlane1 = PlaneFromLine(l, 1);

    //    //Intersect 3 planes
    //    ON_3dPoint p0, p1, p2, p3;
    //    ON_Intersect(pl0, linePlane0, collisionFace, p0);
    //    ON_Intersect(linePlane0, pl1, collisionFace, p1);
    //    ON_Intersect(pl1, linePlane1, collisionFace, p2);
    //    ON_Intersect(linePlane1, pl0, collisionFace, p3);

    //    //Construction polyline
    //    ON_3dPoint points[5] = { p0,p1,p2,p3,p0 };
    //    //ON_Polyline quad;
    //    quad.Empty();
    //    quad.Append(5, points);

    //    return true;

    //}

    inline void AveragePlaneOrigin(IK::Vector_3(&pl0)[4], IK::Vector_3(&pl1)[4], IK::Vector_3(&result)[4]) {

        IK::Vector_3 mid;
        CGAL_VectorUtil::MidVector(pl0[0], pl0[1], mid);

        result[0] = mid;
        result[1] = pl0[1];
        result[2] = pl0[2];
        result[3] = pl0[3];

    }




    inline bool IsSameDirection(IK::Plane_3 pl0, IK::Plane_3 pl1, bool canBeFlipped = true, double tolerance = 0.0001) {

        // CGAL_Debug(CGAL_VectorUtil::IsParallelTo(pl0.orthogonal_vector(), pl1.orthogonal_vector(), tolerance));
        if (canBeFlipped) {
            return CGAL_VectorUtil::IsParallelTo(pl0.orthogonal_vector(), pl1.orthogonal_vector(), tolerance) != 0;
        }
        else {
            return CGAL_VectorUtil::IsParallelTo(pl0.orthogonal_vector(), pl1.orthogonal_vector(), tolerance) == -1;
        }

    }

    inline void ClosestPointTo(IK::Vector_3(&plane)[4], IK::Vector_3& p, IK::Vector_3& result)
    {
        const IK::Vector_3 v(p.x() - plane[0].x(), p.y() - plane[0].y(), p.z() - plane[0].z());
        double s = v * plane[1];
        double t = v * plane[2];
        result = PointAt2(plane, s, t);
    }


    inline bool  IsSamePosition(IK::Plane_3 pl0, IK::Plane_3 pl1, double tolerance = 0.0001) {

        //CGAL_Debug(CGAL::squared_distance(pl0, pl1));
        //return CGAL::squared_distance(pl0, pl1) < tolerance;



        IK::Point_3 p0 = pl0.point();
        IK::Point_3 p1 = pl1.point();

        IK::Point_3 cp0 = pl0.projection(p1);
        IK::Point_3 cp1 = pl1.projection(p0);



        return CGAL_VectorUtil::DistanceSquare(cp0, p1) < tolerance && CGAL_VectorUtil::DistanceSquare(cp1, p0) < tolerance;
    }



    inline bool IsCoplanar(IK::Plane_3 pl0, IK::Plane_3 pl1, bool canBeFlipped = true, double tolerance = 0.01) {

        return IsSameDirection(pl0, pl1, canBeFlipped, tolerance) && IsSamePosition(pl0, pl1, tolerance);

    }

    inline bool QuadFromLineAndTopBottomPlanes(IK::Plane_3& collision_face, IK::Segment_3& l, IK::Plane_3& pl0, IK::Plane_3& pl1, CGAL_Polyline& quad) {

        //bool exact = false;

        //if (exact) {
        //    //Get two perpendicular planes of a line
        //    IK::Vector_3 dir = l.to_vector();

        //    IK_to_EK to_exact;
        //    EK::Plane_3 line_pl0_EK(to_exact(l[0]), to_exact(dir));
        //    EK::Plane_3 line_pl1_EK(to_exact(l[1]), to_exact(dir));

        //    EK::Plane_3 pl0_EK = to_exact(pl0);
        //    EK::Plane_3 pl1_EK = to_exact(pl1);
        //    EK::Plane_3 collision_face_EK = to_exact(collision_face);

        //    //Intersect planes

        //    const auto  result0 = CGAL::intersection(line_pl0_EK, pl0_EK, collision_face_EK);
        //    const auto  result1 = CGAL::intersection(line_pl0_EK, pl1_EK, collision_face_EK);
        //    const auto  result2 = CGAL::intersection(line_pl1_EK, pl1_EK, collision_face_EK);
        //    const auto  result3 = CGAL::intersection(line_pl1_EK, pl0_EK, collision_face_EK);
        //    const EK::Point_3& p0 = boost::get<EK::Point_3>(*result0);
        //    const EK::Point_3& p1 = boost::get<EK::Point_3>(*result1);
        //    const EK::Point_3& p2 = boost::get<EK::Point_3>(*result2);
        //    const EK::Point_3& p3 = boost::get<EK::Point_3>(*result3);

        //    EK_to_IK to_inexact;

        //    quad = { to_inexact(p0) ,to_inexact(p1),to_inexact(p2),to_inexact(p3),to_inexact(p0) };//
        //}
        //else if(false){
        //   

        //
        //    IK::Vector_3 dir = l.to_vector();
        //    IK::Plane_3 line_pl0_EK(l[0], dir);
        //    IK::Plane_3 line_pl1_EK(l[1], dir);

        //    const auto  result0 = CGAL::intersection(line_pl0_EK, pl0, collision_face);
        //    const auto  result1 = CGAL::intersection(line_pl0_EK, pl1, collision_face);
        //    const auto  result2 = CGAL::intersection(line_pl1_EK, pl1, collision_face);
        //    const auto  result3 = CGAL::intersection(line_pl1_EK, pl0, collision_face);
        //    const IK::Point_3& p0 = boost::get<IK::Point_3>(*result0);
        //    const IK::Point_3& p1 = boost::get<IK::Point_3>(*result1);
        //    const IK::Point_3& p2 = boost::get<IK::Point_3>(*result2);
        //    const IK::Point_3& p3 = boost::get<IK::Point_3>(*result3);


        //    quad = { p0 ,p1,p2,p3,p0 };//

        //}
        //else {
        IK::Vector_3 dir = l.to_vector();
        IK::Plane_3 line_pl0_EK(l[0], dir);
        IK::Plane_3 line_pl1_EK(l[1], dir);
        IK::Point_3 p0; CGAL_IntersectionUtil::Intersect(line_pl0_EK, pl0, collision_face, p0);
        IK::Point_3 p1; CGAL_IntersectionUtil::Intersect(line_pl0_EK, pl1, collision_face, p1);
        IK::Point_3 p2; CGAL_IntersectionUtil::Intersect(line_pl1_EK, pl1, collision_face, p2);
        IK::Point_3 p3; CGAL_IntersectionUtil::Intersect(line_pl1_EK, pl0, collision_face, p3);
        quad = { p0 ,p1,p2,p3,p0 };
        /*     }*/

        return true;

    }

    inline bool Plane4LinesIntersection(IK::Plane_3& plane, IK::Segment_3& l0, IK::Segment_3& l1, IK::Segment_3& l2, IK::Segment_3& l3, CGAL_Polyline& output) {

        //printf("CPP %f \n", l0.squared_length());
        //printf("CPP %f  %f %f %f \n", plane.a(), plane.b(), plane.c(), plane.d() );

        IK::Point_3 p0; CGAL_IntersectionUtil::Intersect(l0, plane, p0);
        IK::Point_3 p1; CGAL_IntersectionUtil::Intersect(l1, plane, p1);
        IK::Point_3 p2; CGAL_IntersectionUtil::Intersect(l2, plane, p2);
        IK::Point_3 p3; CGAL_IntersectionUtil::Intersect(l3, plane, p3);
        output = {p0,p1,p2,p3,p0};
        //auto result0 = CGAL::intersection(l0.supporting_line(), plane);
        ////if (!result0) printf("CPP result0");

        //auto result1 = CGAL::intersection(l1.supporting_line(), plane);
        ////if (!result1)printf("CPP result1");


        //auto result2 = CGAL::intersection(l2.supporting_line(), plane);
        ////if (!result2) printf("CPP result2");


        //auto result3 = CGAL::intersection(l3.supporting_line(), plane);
        ////if (!result3)  printf("CPP result3");



        //output = {
        //*boost::get<IK::Point_3>(&*result0),
        //*boost::get<IK::Point_3>(&*result1),
        //*boost::get<IK::Point_3>(&*result2),
        //*boost::get<IK::Point_3>(&*result3),
        //*boost::get<IK::Point_3>(&*result0)
        //};




        return true;

    }

    inline bool PlaneLineIntersection(IK::Plane_3& plane, IK::Segment_3& l0, IK::Point_3& output) {

        return CGAL_IntersectionUtil::Intersect(l0, plane, output);
        //auto result0 = CGAL::intersection(l0, plane);
        //output = *boost::get<IK::Point_3>(&*result0);
        //return true;

    }
}


//#pragma once
//#include "compas.h"
//#include "CGAL_IntersectionUtil.h"
//
//
//namespace CGAL_PlaneUtil {
//
//
//
//
//
//	inline IK::Vector_3 vector_two_planes(IK::Vector_3& v, IK::Plane_3& plane0, IK::Plane_3& plane1) {
//		//printf("vector_two_planes \n");
//		IK::Line_3 line(IK::Point_3(0, 0, 0), v);
//
//		auto result0 = CGAL::intersection(plane0, line);
//		auto result1 = CGAL::intersection(plane1, line);
//
//		auto p0 = *boost::get<IK::Point_3>(&*result0);
//		auto p1 = *boost::get<IK::Point_3>(&*result1);
//
//		return p1 - p0;
//
//	}
//
//	inline IK::Vector_3 orthogonal_vector_between_two_plane_pairs(IK::Plane_3& plane_pair0_0, IK::Plane_3& plane_pair1_0, IK::Plane_3& plane_pair1_1) {
//		//printf(" orthogonal_vector_between_two_plane_pairs \n");
//		//printf("start\n");
//
//		auto result0 = CGAL::intersection(plane_pair0_0, plane_pair1_0);
//		auto result1 = CGAL::intersection(plane_pair0_0, plane_pair1_1);
//
//		auto l0 = *boost::get<IK::Line_3>(&*result0);
//		auto l1 = *boost::get<IK::Line_3>(&*result1);
//		//printf("finish\n");
//
//		//IK::Point_3 p1 = l1.point();
//		//IK::Point_3 p0 = l0.projection(l1.point());
//
//		//IK::Vector_3 v(p1.hx() - p0.hx(), p1.hy() - p0.hy(), p1.hz() - p0.hz() );
//		//CGAL_Debug(v);
//		return l1.point() - l0.projection(l1.point());
//
//	}
//
//	inline IK::Plane_3 offset(IK::Plane_3& mainPlane, double d) {
//
//		IK::Vector_3 normal = mainPlane.orthogonal_vector();
//		CGAL_VectorUtil::Unitize(normal);
//		normal *= d;
//
//		return IK::Plane_3(mainPlane.point() + normal, normal);
//	}
//
//	inline CGAL_Polyline plane_4_planes(IK::Plane_3& mainPlane, IK::Plane_3(&sequence_of_planes)[4]) {
//
//		auto result0 = CGAL::intersection(sequence_of_planes[0], sequence_of_planes[1], mainPlane);
//		auto result1 = CGAL::intersection(sequence_of_planes[1], sequence_of_planes[2], mainPlane);
//		auto result2 = CGAL::intersection(sequence_of_planes[2], sequence_of_planes[3], mainPlane);
//		auto result3 = CGAL::intersection(sequence_of_planes[3], sequence_of_planes[0], mainPlane);
//
//		return {
//		 *boost::get<IK::Point_3>(&*result0),
//		*boost::get<IK::Point_3>(&*result1),
//		*boost::get<IK::Point_3>(&*result2),
//		*boost::get<IK::Point_3>(&*result3),
//		*boost::get<IK::Point_3>(&*result0)
//		};
//
//
//
//	}
//
//	inline IK::Point_3 LinePlane(IK::Line_3& line, IK::Plane_3& plane) {
//
//		auto result = CGAL::intersection(line, plane);
//		/*    if (!result)
//				CGAL_Debug(0);*/
//		auto point = *boost::get<IK::Point_3>(&*result);
//		return point;
//
//	}
//
//	inline IK::Point_3 SegmentPlane(IK::Segment_3& segment, IK::Plane_3& plane) {
//
//		auto result = CGAL::intersection(segment, plane);
//		/* if (!result)
//			 CGAL_Debug(0);*/
//		auto point = *boost::get<IK::Point_3>(&*result);
//		return point;
//
//	}
//	//inline void PlaneFromFourPoints(IK::Point_3& a, IK::Point_3& b, IK::Point_3& c, IK::Point_3& d, IK::Vector_3(&planeAxes)[4]) {
//	//    // a----b
//	//    // c----d
//	//    MidVectorFromPoints(a, d, planeAxes[0]);
//
//	//    planeAxes[1] = b - a;
//	//    CGAL_VectorUtil::Unitize(planeAxes[1]);
//
//	//    planeAxes[2] = a - c;
//	//    CGAL_VectorUtil::Unitize(planeAxes[2]);
//
//	//    planeAxes[3] = CGAL::cross_product(planeAxes[1], planeAxes[2]);
//	//    CGAL_VectorUtil::Unitize(planeAxes[3]);
//
//	//    planeAxes[2] = CGAL::cross_product(planeAxes[3], planeAxes[1]);
//	//    CGAL_VectorUtil::Unitize(planeAxes[2]);
//	//}
//
//
//	inline IK::Vector_3 PointAt2(IK::Vector_3(&input)[4], double s, double t)
//	{
//		return (input[0] + s * input[1] + t * input[2]);
//	}
//
//	inline IK::Vector_3 PointAt(IK::Vector_3(&input)[4], double s, double t, double c)
//	{
//
//		return (input[0] + s * input[1] + t * input[2] + c * input[3]);
//	}
//
//	inline CGAL_Polyline PlaneToLine(IK::Point_3& point, IK::Plane_3& plane, double x = 1, double y = 1, double z = 1) {
//
//
//
//		CGAL_Polyline axes(2);
//		IK::Point_3 center = point;
//		IK::Vector_3 n = plane.orthogonal_vector();
//		CGAL_VectorUtil::Unitize(n);
//		n *= x;
//		IK::Point_3 normal = point + n;// +
//		axes[0] = center;
//		axes[1] = normal;
//
//		//axes[0] = (CGAL_VectorUtil::ToPoint(input[0]));
//		//axes[1] = (CGAL_VectorUtil::ToPoint(input[0]) + input[1] * x * 1.25);
//		//axes[2] = (CGAL_VectorUtil::ToPoint(input[0]));
//		//axes[3] = (CGAL_VectorUtil::ToPoint(input[0]) + input[2] * y);
//		//axes[4] = (CGAL_VectorUtil::ToPoint(input[0]));
//		//axes[5] = (CGAL_VectorUtil::ToPoint(input[0]) + input[3] * z);
//
//		return axes;
//
//	}
//
//
//	inline CGAL_Polyline PlaneToPolyline(IK::Plane_3& plane, double x = 1, double y = 1, double z = 1) {
//
//		auto point = plane.point();
//		auto X0 = plane.base1();
//		auto Y0 = plane.base2();
//		auto Z0 = plane.orthogonal_vector();
//		CGAL_VectorUtil::Unitize(X0);
//		CGAL_VectorUtil::Unitize(Y0);
//		CGAL_VectorUtil::Unitize(Z0);
//
//		CGAL_Polyline axes(6);
//
//		axes[0] = (point);
//		axes[1] = (point + X0 * x * 1.25);
//		axes[2] = (point);
//		axes[3] = (point + Y0 * y);
//		axes[4] = (point);
//		axes[5] = (point + Z0 * z);
//
//		return axes;
//
//	}
//
//
//
//	inline CGAL_Polyline PlaneToPolyline(IK::Vector_3(&input)[4], double x = 1, double y = 1, double z = 1) {
//
//		CGAL_Polyline axes(6);
//
//		axes[0] = (CGAL_VectorUtil::ToPoint(input[0]));
//		axes[1] = (CGAL_VectorUtil::ToPoint(input[0]) + input[1] * x * 1.25);
//		axes[2] = (CGAL_VectorUtil::ToPoint(input[0]));
//		axes[3] = (CGAL_VectorUtil::ToPoint(input[0]) + input[2] * y);
//		axes[4] = (CGAL_VectorUtil::ToPoint(input[0]));
//		axes[5] = (CGAL_VectorUtil::ToPoint(input[0]) + input[3] * z);
//
//		return axes;
//
//	}
//
//	inline void Assign(IK::Vector_3(&source)[4], IK::Vector_3(&target)[4], int n) {
//		for (int i = 0; i < n; i++)
//			target[i] = source[i];
//	}
//
//
//	//inline ON_Plane PlaneFromLine(ON_Line& l, double t = 0) {
//	//    ON_Plane pl(l.PointAt(t), l.Direction());
//	//    return pl;
//	//}
//
//	//inline bool QuadFromLineAndTopBottomPlanes(ON_Plane& collisionFace, ON_Plane& pl0, ON_Plane& pl1, ON_Line& l, ON_Polyline& quad) {
//
//	//    //Get two perpendicular planes of a line
//	//    ON_Plane linePlane0 = PlaneFromLine(l, 0);
//	//    ON_Plane linePlane1 = PlaneFromLine(l, 1);
//
//	//    //Intersect 3 planes
//	//    ON_3dPoint p0, p1, p2, p3;
//	//    ON_Intersect(pl0, linePlane0, collisionFace, p0);
//	//    ON_Intersect(linePlane0, pl1, collisionFace, p1);
//	//    ON_Intersect(pl1, linePlane1, collisionFace, p2);
//	//    ON_Intersect(linePlane1, pl0, collisionFace, p3);
//
//	//    //Construction polyline
//	//    ON_3dPoint points[5] = { p0,p1,p2,p3,p0 };
//	//    //ON_Polyline quad;
//	//    quad.Empty();
//	//    quad.Append(5, points);
//
//	//    return true;
//
//	//}
//
//	inline void AveragePlaneOrigin(IK::Vector_3(&pl0)[4], IK::Vector_3(&pl1)[4], IK::Vector_3(&result)[4]) {
//
//		IK::Vector_3 mid;
//		CGAL_VectorUtil::MidVector(pl0[0], pl0[1], mid);
//
//		result[0] = mid;
//		result[1] = pl0[1];
//		result[2] = pl0[2];
//		result[3] = pl0[3];
//
//	}
//
//
//
//
//	inline bool IsSameDirection(IK::Plane_3 pl0, IK::Plane_3 pl1, bool canBeFlipped = true, double tolerance = 0.0001) {
//
//		// CGAL_Debug(CGAL_VectorUtil::IsParallelTo(pl0.orthogonal_vector(), pl1.orthogonal_vector(), tolerance));
//		if (canBeFlipped) {
//			return CGAL_VectorUtil::IsParallelTo(pl0.orthogonal_vector(), pl1.orthogonal_vector(), tolerance) != 0;
//		}
//		else {
//			return CGAL_VectorUtil::IsParallelTo(pl0.orthogonal_vector(), pl1.orthogonal_vector(), tolerance) == -1;
//		}
//
//	}
//
//	inline void ClosestPointTo(IK::Vector_3(&plane)[4], IK::Vector_3& p, IK::Vector_3& result)
//	{
//		const IK::Vector_3 v(p.x() - plane[0].x(), p.y() - plane[0].y(), p.z() - plane[0].z());
//		double s = v * plane[1];
//		double t = v * plane[2];
//		result = PointAt2(plane, s, t);
//	}
//
//
//	inline bool  IsSamePosition(IK::Plane_3 pl0, IK::Plane_3 pl1, double tolerance = 0.0001) {
//
//		//CGAL_Debug(CGAL::squared_distance(pl0, pl1));
//		//return CGAL::squared_distance(pl0, pl1) < tolerance;
//
//
//
//		IK::Point_3 p0 = pl0.point();
//		IK::Point_3 p1 = pl1.point();
//
//		IK::Point_3 cp0 = pl0.projection(p1);
//		IK::Point_3 cp1 = pl1.projection(p0);
//
//
//
//		return CGAL_VectorUtil::DistanceSquare(cp0, p1) < tolerance && CGAL_VectorUtil::DistanceSquare(cp1, p0) < tolerance;
//	}
//
//
//
//	inline bool IsCoplanar(IK::Plane_3 pl0, IK::Plane_3 pl1, bool canBeFlipped = true, double tolerance = 0.01) {
//
//		return IsSameDirection(pl0, pl1, canBeFlipped, tolerance) && IsSamePosition(pl0, pl1, tolerance);
//
//	}
//
//	inline bool QuadFromLineAndTopBottomPlanes(IK::Plane_3& collision_face, IK::Segment_3& l, IK::Plane_3& pl0, IK::Plane_3& pl1, CGAL_Polyline& quad) {
//
//		//bool exact = false;
//
//		//if (exact) {
//		//    //Get two perpendicular planes of a line
//		//    IK::Vector_3 dir = l.to_vector();
//
//		//    IK_to_EK to_exact;
//		//    EK::Plane_3 line_pl0_EK(to_exact(l[0]), to_exact(dir));
//		//    EK::Plane_3 line_pl1_EK(to_exact(l[1]), to_exact(dir));
//
//		//    EK::Plane_3 pl0_EK = to_exact(pl0);
//		//    EK::Plane_3 pl1_EK = to_exact(pl1);
//		//    EK::Plane_3 collision_face_EK = to_exact(collision_face);
//
//		//    //Intersect planes
//
//		//    const auto  result0 = CGAL::intersection(line_pl0_EK, pl0_EK, collision_face_EK);
//		//    const auto  result1 = CGAL::intersection(line_pl0_EK, pl1_EK, collision_face_EK);
//		//    const auto  result2 = CGAL::intersection(line_pl1_EK, pl1_EK, collision_face_EK);
//		//    const auto  result3 = CGAL::intersection(line_pl1_EK, pl0_EK, collision_face_EK);
//		//    const EK::Point_3& p0 = boost::get<EK::Point_3>(*result0);
//		//    const EK::Point_3& p1 = boost::get<EK::Point_3>(*result1);
//		//    const EK::Point_3& p2 = boost::get<EK::Point_3>(*result2);
//		//    const EK::Point_3& p3 = boost::get<EK::Point_3>(*result3);
//
//		//    EK_to_IK to_inexact;
//
//		//    quad = { to_inexact(p0) ,to_inexact(p1),to_inexact(p2),to_inexact(p3),to_inexact(p0) };//
//		//}
//		//else if(false){
//		//   
//
//		//
//		//    IK::Vector_3 dir = l.to_vector();
//		//    IK::Plane_3 line_pl0_EK(l[0], dir);
//		//    IK::Plane_3 line_pl1_EK(l[1], dir);
//
//		//    const auto  result0 = CGAL::intersection(line_pl0_EK, pl0, collision_face);
//		//    const auto  result1 = CGAL::intersection(line_pl0_EK, pl1, collision_face);
//		//    const auto  result2 = CGAL::intersection(line_pl1_EK, pl1, collision_face);
//		//    const auto  result3 = CGAL::intersection(line_pl1_EK, pl0, collision_face);
//		//    const IK::Point_3& p0 = boost::get<IK::Point_3>(*result0);
//		//    const IK::Point_3& p1 = boost::get<IK::Point_3>(*result1);
//		//    const IK::Point_3& p2 = boost::get<IK::Point_3>(*result2);
//		//    const IK::Point_3& p3 = boost::get<IK::Point_3>(*result3);
//
//
//		//    quad = { p0 ,p1,p2,p3,p0 };//
//
//		//}
//		//else {
//		IK::Vector_3 dir = l.to_vector();
//		IK::Plane_3 line_pl0_EK(l[0], dir);
//		IK::Plane_3 line_pl1_EK(l[1], dir);
//		IK::Point_3 p0; CGAL_IntersectionUtil::Intersect(line_pl0_EK, pl0, collision_face, p0);
//		IK::Point_3 p1; CGAL_IntersectionUtil::Intersect(line_pl0_EK, pl1, collision_face, p1);
//		IK::Point_3 p2; CGAL_IntersectionUtil::Intersect(line_pl1_EK, pl1, collision_face, p2);
//		IK::Point_3 p3; CGAL_IntersectionUtil::Intersect(line_pl1_EK, pl0, collision_face, p3);
//		quad = { p0 ,p1,p2,p3,p0 };
//		/*     }*/
//
//		return true;
//
//	}
//
//	inline bool Plane4LinesIntersection(IK::Plane_3& plane, IK::Segment_3& l0, IK::Segment_3& l1, IK::Segment_3& l2, IK::Segment_3& l3, CGAL_Polyline& output) {
//
//		//printf("CPP %f \n", l0.squared_length());
//		//printf("CPP %f  %f %f %f \n", plane.a(), plane.b(), plane.c(), plane.d() );
//
//
//
//		auto result0 = CGAL::intersection(l0.supporting_line(), plane);
//		//if (!result0) printf("CPP result0");
//
//		auto result1 = CGAL::intersection(l1.supporting_line(), plane);
//		//if (!result1)printf("CPP result1");
//
//
//		auto result2 = CGAL::intersection(l2.supporting_line(), plane);
//		//if (!result2) printf("CPP result2");
//
//
//		auto result3 = CGAL::intersection(l3.supporting_line(), plane);
//		//if (!result3)  printf("CPP result3");
//
//
//
//		output = {
//		*boost::get<IK::Point_3>(&*result0),
//		*boost::get<IK::Point_3>(&*result1),
//		*boost::get<IK::Point_3>(&*result2),
//		*boost::get<IK::Point_3>(&*result3),
//		*boost::get<IK::Point_3>(&*result0)
//		};
//
//
//
//
//		return true;
//
//	}
//
//	inline bool PlaneLineIntersection(IK::Plane_3& plane, IK::Segment_3& l0, IK::Point_3& output) {
//
//		auto result0 = CGAL::intersection(l0, plane);
//		output = *boost::get<IK::Point_3>(&*result0);
//		return true;
//
//	}
//}