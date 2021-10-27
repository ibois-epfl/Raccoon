//#pragma once
//#include "compas.h"
//
//class CGAL_PlaneEquation
//{
//
//    public:
//        // C++ defaults for construction, destruction, copys, and operator=
//        // work fine.
//
//        static const CGAL_PlaneEquation UnsetPlaneEquation; // (ON_UNSET_VALUE,ON_UNSET_VALUE,ON_UNSET_VALUE,ON_UNSET_VALUE)
//        static const CGAL_PlaneEquation ZeroPlaneEquation; // (0.0,0.0,0.0,0.0)
//        static const CGAL_PlaneEquation NanPlaneEquation; // (ON_DBL_QNAN,ON_DBL_QNAN,ON_DBL_QNAN,ON_DBL_QNAN)
//
//        static const CGAL_PlaneEquation WorldXY; // (0,0,1,0)
//        static const CGAL_PlaneEquation WorldYZ; // (1,0,0,0)
//        static const CGAL_PlaneEquation WorldZX; // (0,1,0,0)
//
//        /*
//        Returns:
//          If the three points are valid and not colinear, a unitized plane equation is returned.
//          Otherwise CGAL_PlaneEquation::NanPlaneEquation is returned.
//        */
//        static const CGAL_PlaneEquation CreateFromThreePoints(
//            CGAL_Point pointA,
//            CGAL_Point pointB,
//            CGAL_Point pointC
//        );
//
//        /*
//        Returns:
//          If point is valid and normal is nonzero, a unitized plane equation is returned.
//          Otherwise CGAL_PlaneEquation::NanPlaneEquation is returned.
//        */
//        static const CGAL_PlaneEquation CreateFromPointAndNormal(
//            CGAL_Point point,
//            CGAL_Vector normal
//        );
//
//        CGAL_PlaneEquation();
//
//        CGAL_PlaneEquation(
//            double xx, double yy, double zz, double dd
//        );
//
//        static void Set(
//            CGAL_PlaneEquation& plane_equation,
//            double x, double y, double z, double d
//        );
//
//        double MaximumCoefficient() const;
//
//        /*
//        Returns:
//          The plane equation whose coefficient values are
//          the negative of the coefficent values in this,
//          provided the coeffient value is valid. Any invalid
//          coefficent values are copied.
//        */
//        CGAL_PlaneEquation NegatedPlaneEquation() const;
//
//
//        /*
//        Returns:
//          The plane equation whose first three coefficient values
//          are a unit vector.
//        */
//        CGAL_PlaneEquation UnitizedPlaneEquation() const;
//
//
//
//        /*
//        Description:
//          returns true if x, y, z, d are valid, finite doubles.
//        Remarks:
//          this function will return true if x, y and z are all zero.
//        See Also:
//          CGAL_PlaneEquation::IsSet().
//        */
//        bool IsValid() const;
//
//        /*
//        Description:
//          returns true if x, y, z, d are valid, finite doubles and at least one of x, y or z is not zero.
//        */
//        bool IsSet() const;
//
//        /*
//        Description:
//          returns true if x, y, z, d are valid, finite doubles and x^2 + y^2 + z^2 = 1.
//        */
//        bool IsUnitized() const;
//
//        /*
//        Description:
//          Sets (x,y,z) to a unitized N and then sets
//          d = -(x*P.x + y*P.y + z*P.z).
//        Parameters:
//          P - [in] point on the plane
//          N - [in] vector perpendicular to the plane
//        Returns:
//           true if input is valid.
//        */
//        bool Create(CGAL_Point P, CGAL_Vector N);
//
//        // index operators mimic double[4] behavior
//        // Return null refs or ON_UNSET_VALUE for out-of-range indices
//        double& operator[](int);
//        double& operator[](unsigned int);
//        double operator[](int) const;
//        double operator[](unsigned int) const;
//
//        /*
//        Returns:
//          Direction (x,y,z)
//        */
//        CGAL_Vector Direction() const;
//
//        double DirectionLength() const;
//
//        /*
//        Returns:
//          Unitized direction or zero vector if not set.
//        */
//        CGAL_Vector UnitNormal() const;
//
//        /*
//          Returns  1: this and other vectors are parallel
//                  -1: this and other vectors are anti-parallel
//                   0: this and other vectors are not parallel
//                      or at least one of the vectors is zero
//        */
//        int IsParallelTo(
//            const CGAL_PlaneEquation& other,                      // other plane equation
//            double angle_tolerance = ON_DEFAULT_ANGLE_TOLERANCE // optional angle tolerance (radians)
//        ) const;
//
//        /*
//        Description:
//          Evaluate the plane at a point.
//        Parameters:
//          P - [in]
//        Returns:
//          x*P.x + y*P.y + z*P.z + d;
//        */
//        double ValueAt(CGAL_Point P) const;
//        //double ValueAt(ON_4dPoint P) const;
//        double ValueAt(CGAL_Vector P) const;
//        double ValueAt(double x, double y, double z) const;
//
//        ///*
//        //Returns:
//        //  ON_Interval::EmptyInterval if input is not valid.
//        //*/
//        //ON_Interval ValueRange(
//        //    size_t point_list_count,
//        //    const CGAL_Point* point_list
//        //) const;
//
//        ///*
//        //Returns:
//        //  ON_Interval::EmptyInterval if input is not valid.
//        //*/
//        //ON_Interval ValueRange(
//        //    const ON_SimpleArray< CGAL_Point >& point_list
//        //) const;
//
//        ///*
//        //Returns:
//        //  ON_Interval::EmptyInterval if input is not valid.
//        //*/
//        //ON_Interval ValueRange(
//        //    size_t point_list_count,
//        //    const ON_3fPoint* point_list
//        //) const;
//
//        ///*
//        //Returns:
//        //  ON_Interval::EmptyInterval if input is not valid.
//        //*/
//        //ON_Interval ValueRange(
//        //    const ON_SimpleArray< ON_3fPoint >& point_list
//        //) const;
//
//        ///*
//        //Returns:
//        //  ON_Interval::EmptyInterval if input is not valid.
//        //*/
//        //ON_Interval ValueRange(
//        //    const class CGAL_PointListRef& point_list
//        //) const;
//
//        ///*
//        //Returns:
//        //  ON_Interval::EmptyInterval if input is not valid.
//        //*/
//        //ON_Interval ValueRange(
//        //    size_t point_index_count,
//        //    const unsigned int* point_index_list,
//        //    const class CGAL_PointListRef& point_list
//        //) const;
//
//        //ON_Interval ValueRange(
//        //    size_t point_index_count,
//        //    size_t point_index_stride,
//        //    const unsigned int* point_index_list,
//        //    const class CGAL_PointListRef& point_list
//        //) const;
//
//        /*
//        Description:
//          Evaluate the plane at a list of point values.
//        Parameters:
//          Pcount - [in]
//            number of points
//          P - [in]
//            points
//          value - [in]
//            If not null, value[] must be an array of length at least Pcount.
//            The values will be stored in this array.  If null, the an array
//            will be allocated with onmalloc() and returned.
//          value_range - [out]
//            If not null, the range of values will be returned here.
//        Returns:
//          An array of Pcount values.  If the input parameter value was null,
//          then the array is allocated on the heap using onmalloc() and the
//          caller is responsible for calling onfree() when finished.  If the
//          input is not valid, null is returned.
//        */
//
//
//        double* ValueAt(
//            int Pcount,
//            const CGAL_Point* P,
//            double* value,
//            double value_range[2]
//        ) const;
//
//        /*
//        Description:
//          This function calculates and evalutes points that
//          would be exactly on the plane if double precision
//          aritmetic were mathematically perfect and returns
//          the largest value of the evaluations.
//        */
//        double ZeroTolerance() const;
//
//        /*
//        Description:
//          Transform the plane equation so that, if e0 is the initial
//          equation, e1 is transformed equation and P is a point,
//          then e0.ValueAt(P) = e1.ValueAt(xform*P).
//        Parameters:
//          xform - [in]
//            Invertable transformation.
//        Returns:
//          True if the plane equation was successfully transformed.
//          False if xform is not invertable or the equation is not
//          valid.
//        Remarks:
//          This function has to invert xform.  If you have apply the
//          same transformation to a bunch of planes, then it will be
//          more efficient to calculate xform's inverse transpose
//          and apply the resultingt transformation to the equation's
//          coefficients as if they were 4d point coordinates.
//        */
//        bool Transform(const CGAL_Transformation& xform);
//
//        /*
//        Description:
//          Get point on plane that is closest to a given point.
//        Parameters:
//          point - [in]
//        Returns:
//          A 3d point on the plane that is closest to the input point.
//        */
//        CGAL_Point ClosestPointTo(CGAL_Point point) const;
//
//        /*
//        Description:
//          Get the minimum value of the plane equation
//          on a bounding box.
//        Parameters:
//          bbox - [in]
//        Returns:
//          Minimum value of the plane equation on the bounding box.
//        */
//        //double MinimumValueAt(const ON_BoundingBox& bbox) const;
//
//        /*
//        Description:
//          Get the maximum value of the plane equation
//          on a bounding box.
//        Parameters:
//          bbox - [in]
//        Returns:
//          Maximum value of the plane equation on the bounding box.
//        */
//        //double MaximumValueAt(const ON_BoundingBox& bbox) const;
//
//
//        /*
//        Description:
//          Get the maximum value of the plane equation on a set of 3d points.
//        Parameters:
//          bRational - [in]
//            False if the points are euclidean (x,y,z)
//            True if the points are homogenous rational (x,y,z,w)
//            (x/w,y/w,z/w) is used to evaluate the value.
//          point_count - [in]
//          point_stride - [in]
//            i-th point's x coordinate = points[i*point_stride]
//          points - [in]
//            coordinates of points
//          stop_value - [in]
//            If stop_value is valid and not ON_UNSET_VALUE, then the
//            evaulation stops if a value > stop_value is found.
//            If stop_value = ON_UNSET_VALUE, then stop_value is ignored.
//        Returns:
//          Maximum value of the plane equation on the point list.
//          If the input is not valid, then ON_UNSET_VALUE is returned.
//        */
//        double MaximumValueAt(
//            bool bRational,
//            int point_count,
//            int point_stride,
//            const double* points,
//            double stop_value
//        ) const;
//
//        /*
//        Description:
//          Get the minimum value of the plane equation on a set of 3d points.
//        Parameters:
//          bRational - [in]
//            False if the points are euclidean (x,y,z)
//            True if the points are homogenous rational (x,y,z,w)
//            (x/w,y/w,z/w) is used to evaluate the value.
//          point_count - [in]
//          point_stride - [in]
//            i-th point's x coordinate = points[i*point_stride]
//          points - [in]
//            coordinates of points
//          stop_value - [in]
//            If stop_value is valid and not ON_UNSET_VALUE, then the
//            evaulation stops if a value < stop_value is found.
//            If stop_value = ON_UNSET_VALUE, then stop_value is ignored.
//        Returns:
//          Maximum value of the plane equation on the point list.
//          If the input is not valid, then ON_UNSET_VALUE is returned.
//        */
//        double MinimumValueAt(
//            bool bRational,
//            int point_count,
//            int point_stride,
//            const double* points,
//            double stop_value
//        ) const;
//
//        /*
//        Description:
//          Get the maximum absolute value of the plane equation
//          on a set of 3d points.
//        Parameters:
//          bRational - [in]
//            False if the points are euclidean (x,y,z)
//            True if the points are homogenous rational (x,y,z,w)
//            (x/w,y/w,z/w) is used to evaluate the value.
//          point_count - [in]
//          point_stride - [in]
//            i-th point's x coordinate = points[i*point_stride]
//          points - [in]
//            coordinates of points
//          stop_value - [in]
//            If stop_value >= 0.0, then the evaulation stops if an
//            absolute value > stop_value is found. If stop_value < 0.0
//            or stop_value is invalid, then stop_value is ignored.
//        Returns:
//          Maximum value of the plane equation on the point list.
//          If the input is not valid, then ON_UNSET_VALUE is returned.
//        */
//        double MaximumAbsoluteValueAt(
//            bool bRational,
//            int point_count,
//            int point_stride,
//            const double* points,
//            double stop_value
//        ) const;
//
//        /*
//        Description:
//          Test points on a bezier curve to see if they are near the plane.
//        Parameters:
//          bezcrv - [in]
//          s0 - [in]
//          s1 - [in] the interval from s0 to s1 is tested (s0 < s1)
//          sample_count - [in] number of interior points to test.
//                      Numbers like 1, 3, 7, 15, ... work best.
//          endpoint_tolerance - [in] If >= 0, then the end points are
//                    tested to see if the distance from the endpoints
//                    is <= endpoint_tolerance.
//          interior_tolerance - [in] (>=0 and >=endpoint_tolerance)
//                    This tolerance is used to test the interior sample points.
//          smin - [put]  If not nullptr, *smin = bezier parameter of nearest
//                        test point.
//          smax - [put]  If not nullptr, *smax = bezier parameter of farthest
//                        test point.  If false is returned, this is the
//                        parameter of the test point that failed.
//        Returns:
//          True if all the tested points passed the tolerance test.
//          False if at least one tested point failed the tolerance test.
//          (The test terminates when the first failure is encountered.)
//        */
//        bool IsNearerThan(
//            const class ON_BezierCurve& bezcrv,
//            double s0,
//            double s1,
//            int sample_count,
//            double endpoint_tolerance,
//            double interior_tolerance,
//            double* smin,
//            double* smax
//        ) const;
//
//        bool operator==(const CGAL_PlaneEquation&) const;
//        bool operator!=(const CGAL_PlaneEquation&) const;
//
//        double x;
//        double y;
//        double z;
//        double d; // 4th coefficient of the plane equation.
//
//        void Dump(class ON_TextLog&) const;
//    
//
//};
//
