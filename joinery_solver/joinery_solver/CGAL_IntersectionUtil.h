#pragma once

#include "compas.h"

//PlanePlanePlane
//PlanePlane
//LinePlane
// LineLine not implemented
//https://github.com/mcneel/opennurbs/blob/c20e599d1ff8f08a55d3dddf5b39e37e8b5cac06/opennurbs_intersect.cpp
namespace CGAL_IntersectionUtil {




    inline int Solve3x3(

        const double(&row0)[3], const double(&row1)[3], const double(&row2)[3],
        double d0, double d1, double d2,
        double* x_addr, double* y_addr, double* z_addr,
        double* pivot_ratio)
    {
        /* Solve a 3x3 linear system using Gauss-Jordan elimination
         * with full pivoting.
         */
        int i, j;
        double* p0;
        double* p1;
        double* p2;
        double x, y, workarray[12], maxpiv, minpiv;

        const int sizeof_row = 3 * sizeof(row0[0]);

        *pivot_ratio = *x_addr = *y_addr = *z_addr = 0.0;
        x = fabs(row0[0]); i = j = 0;
        y = fabs(row0[1]); if (y > x) { x = y; j = 1; }
        y = fabs(row0[2]); if (y > x) { x = y; j = 2; }
        y = fabs(row1[0]); if (y > x) { x = y; i = 1; j = 0; }
        y = fabs(row1[1]); if (y > x) { x = y; i = 1; j = 1; }
        y = fabs(row1[2]); if (y > x) { x = y; i = 1; j = 2; }
        y = fabs(row2[0]); if (y > x) { x = y; i = 2; j = 0; }
        y = fabs(row2[1]); if (y > x) { x = y; i = 2; j = 1; }
        y = fabs(row2[2]); if (y > x) { x = y; i = 2; j = 2; }
        if (x == 0.0)
            return 0;
        maxpiv = minpiv = fabs(x);
        p0 = workarray;
        switch (i) {
        case 1: /* swap rows 0 and 1 */
            memcpy(p0, row1, sizeof_row); p0[3] = d1; p0 += 4;
            memcpy(p0, row0, sizeof_row); p0[3] = d0; p0 += 4;
            memcpy(p0, row2, sizeof_row); p0[3] = d2;
            break;
        case 2: /* swap rows 0 and 2 */
            memcpy(p0, row2, sizeof_row); p0[3] = d2; p0 += 4;
            memcpy(p0, row1, sizeof_row); p0[3] = d1; p0 += 4;
            memcpy(p0, row0, sizeof_row); p0[3] = d0;
            break;
        default:
            memcpy(p0, row0, sizeof_row); p0[3] = d0; p0 += 4;
            memcpy(p0, row1, sizeof_row); p0[3] = d1; p0 += 4;
            memcpy(p0, row2, sizeof_row); p0[3] = d2;
            break;
        }
        switch (j) {
        case 1: /* swap columns 0 and 1 */
            p0 = x_addr; x_addr = y_addr; y_addr = p0;
            p0 = &workarray[0];
            x = p0[0]; p0[0] = p0[1]; p0[1] = x; p0 += 4;
            x = p0[0]; p0[0] = p0[1]; p0[1] = x; p0 += 4;
            x = p0[0]; p0[0] = p0[1]; p0[1] = x;
            break;
        case 2: /* swap columns 0 and 2 */
            p0 = x_addr; x_addr = z_addr; z_addr = p0;
            p0 = &workarray[0];
            x = p0[0]; p0[0] = p0[2]; p0[2] = x; p0 += 4;
            x = p0[0]; p0[0] = p0[2]; p0[2] = x; p0 += 4;
            x = p0[0]; p0[0] = p0[2]; p0[2] = x;
            break;
        }

        x = 1.0 / workarray[0];
        /* debugger set workarray[0] = 1 */
        p0 = p1 = workarray + 1;
        *p1++ *= x; *p1++ *= x; *p1++ *= x;
        x = -(*p1++);
        /* debugger set workarray[4] = 0 */
        if (x == 0.0)
            p1 += 3;
        else
        {
            *p1++ += x * (*p0++); *p1++ += x * (*p0++); *p1++ += x * (*p0); p0 -= 2;
        }
        x = -(*p1++);
        /* debugger set workarray[8] = 0 */
        if (x != 0.0)
        {
            *p1++ += x * (*p0++); *p1++ += x * (*p0++); *p1++ += x * (*p0); p0 -= 2;
        }

        x = fabs(workarray[5]); i = j = 0;
        y = fabs(workarray[6]); if (y > x) { x = y; j = 1; }
        y = fabs(workarray[9]); if (y > x) { x = y; i = 1; j = 0; }
        y = fabs(workarray[10]); if (y > x) { x = y; i = j = 1; }
        if (x == 0.0)
            return 1; // rank = 1;
        y = fabs(x);
        if (y > maxpiv) maxpiv = y; else if (y < minpiv) minpiv = y;
        if (j) {
            /* swap columns 1 and 2 */
            p0 = workarray + 1;
            p1 = p0 + 1;
            x = *p0; *p0 = *p1; *p1 = x; p0 += 4; p1 += 4;
            x = *p0; *p0 = *p1; *p1 = x; p0 += 4; p1 += 4;
            x = *p0; *p0 = *p1; *p1 = x; p0 += 4; p1 += 4;
            p0 = y_addr; y_addr = z_addr; z_addr = p0;
        }

        if (i) {
            /* pivot is in row 2 */
            p0 = workarray + 1;
            p1 = p0 + 8;
            p2 = p0 + 4;
        }
        else {
            /* pivot is in row 1 */
            p0 = workarray + 1;
            p1 = p0 + 4;
            p2 = p0 + 8;
        }

        /* debugger set workarray[5+4*i] = 1 */
        x = 1.0 / (*p1++); *p1++ *= x; *p1 *= x; p1--;
        x = -(*p0++);
        /* debugger set p0[-1] = 0 */
        if (x != 0.0) { *p0++ += x * (*p1++); *p0 += x * (*p1); p0--; p1--; }
        x = -(*p2++);
        /* debugger set p2[-1] = 0 */
        if (x != 0.0) { *p2++ += x * (*p1++); *p2 += x * (*p1); p2--; p1--; }
        x = *p2++;
        if (x == 0.0)
            return 2; // rank = 2;
        y = fabs(x);
        if (y > maxpiv) maxpiv = y; else if (y < minpiv) minpiv = y;
        /* debugger set p2[-1] = 1 */
        *p2 /= x;
        x = -(*p1++);  if (x != 0.0) *p1 += x * (*p2);
        /* debugger set p1[-1] = 0 */
        x = -(*p0++);  if (x != 0.0) *p0 += x * (*p2);
        /* debugger set p0[-1] = 0 */
        *x_addr = workarray[3];
        if (i) {
            *y_addr = workarray[11];
            *z_addr = workarray[7];
        }
        else {
            *y_addr = workarray[7];
            *z_addr = workarray[11];
        }
        *pivot_ratio = minpiv / maxpiv;
        return 3;
    }

    inline bool Intersect(IK::Plane_3& R, IK::Plane_3& S, IK::Plane_3& T,
        IK::Point_3& P)
    {
        double pr = 0.0;
        double x, y, z;

        const double R_[3] = { R.a() ,R.b() ,R.c() };
        const double S_[3] = { S.a() ,S.b() ,S.c() };
        const double T_[3] = { T.a() ,T.b() ,T.c() };

        const int rank = Solve3x3(
            //&R.plane_equation.x, &S.plane_equation.x, &T.plane_equation.x,
            R_, S_, T_,
            -R.d(), -S.d(), -T.d(),
            &x, &y, &z, &pr);
        P = IK::Point_3(x, y, z);
        return (rank == 3) ? true : false;
    }

    inline bool Intersect( IK::Plane_3& R,  IK::Plane_3& S, IK::Line_3& L)
    {
        IK::Vector_3 d = CGAL::cross_product(S.orthogonal_vector(), R.orthogonal_vector());
        IK::Point_3 p = CGAL::midpoint(R.point(), S.point());
        IK::Plane_3 T(p, d);

        IK::Point_3 l0;
        bool rc = Intersect(R, S, T, l0);
        L = IK::Line_3(l0, l0 + d);

        return rc;
    }

    inline double ValueAt(IK::Plane_3& Pl,  IK::Point_3& P) 
    {
        return (Pl.a() * P.hx() + Pl.b() * P.hy() + Pl.c() * P.hz() + Pl.d());
    }

    inline bool Intersect(
        IK::Segment_3& L,
        IK::Plane_3& Pl,
        IK::Point_3& P
        //double* line_parameter
    )
    {
        bool rc = false;
        double a, b, d, fd, t;

        auto pt0 = L[0];
        auto pt1 = L[1];
        a = ValueAt(Pl, pt0);
        b = ValueAt(Pl, pt1);
        d = a - b;
        if (d == 0.0)
        {
            if (fabs(a) < fabs(b))
                t = 0.0;
            else if (fabs(b) < fabs(a))
                t = 1.0;
            else
                t = 0.5;
        }
        else
        {
            d = 1.0 / d;
            fd = fabs(d);
            if (fd > 1.0 && (fabs(a) >= ON_DBL_MAX / fd || fabs(b) >= ON_DBL_MAX / fd))
            {
                // double overflow - line may be (nearly) parallel to plane
                t = 0.5;
            }
            else
            {
                t = a / (a - b); // = a*d;  a/(a-b) has more precision than a*d
                rc = true;
            }
        }

        
        //if (line_parameter)
        //    *line_parameter = t;

                //s[0].z()
        // 26 Feb 2003 Dale Lear
        //     Changed 
        //          return (1-t)*from + t*to;
        //     to the following so that axis aligned lines will
        //     return exact answers for large values of t.  
        //     See RR 9683.

        const double s = 1.0 - t;

        P = IK::Point_3(
            (L[0].x() == L[1].x()) ? L[0].x() : s * L[0].x() + t * L[1].x(),
            (L[0].y() == L[1].y()) ? L[0].y() : s * L[0].y() + t * L[1].y(),
            (L[0].z() == L[1].z()) ? L[0].z() : s * L[0].z() + t * L[1].z()
        );

        return rc;
    }




}