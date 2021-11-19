#pragma once
#include "stdafx.h"
#include "AxisPlane.h"


AxisPlane::AxisPlane()
    : origin(0.0, 0.0, 0.0),
    xaxis(1.0, 0.0, 0.0), yaxis(0.0, 1.0, 0.0), zaxis(0.0, 0.0, 1.0)
{
    //plane_equation.x = plane_equation.y = plane_equation.d = 0.0;
    //plane_equation.z = 1.0;
}
//
AxisPlane::AxisPlane(
    const IK::Point_3& P, // point on the plane
    const IK::Vector_3& N  // non-zero normal to the plane
)
{
    CreateFromNormal(P, N);
}

AxisPlane::AxisPlane(
    const IK::Point_3& P, // point on the plane
    const IK::Vector_3& X, // non-zero vector in plane
    const IK::Vector_3& Y  // another vector in the plane not parallel to X
)
{
    CreateFromFrame(P, X, Y);
}

AxisPlane::AxisPlane(
    const IK::Point_3& P,  // point on the plane
    const IK::Point_3& Q,  // point on the plane
    const IK::Point_3& R   // point on the plane
)
{
    CreateFromPoints(P, Q, R);
}
//
////AxisPlane::AxisPlane(
////    const double e[4]    // equation of plane
////)
////{
////    CreateFromEquation(e);
////}
//
////AxisPlane::AxisPlane(
////    const AxisPlaneEquation& plane_equation
////)
////{
////    CreateFromEquation(plane_equation);
////}
//
AxisPlane::~AxisPlane()
{}

IK::Point_3 AxisPlane::PointAt(double s, double t) const
{
    return (origin + s * xaxis + t * yaxis);
}

IK::Point_3 AxisPlane::PointAt(double s, double t, double c) const
{
    return (origin + s * xaxis + t * yaxis + c * zaxis);
}
//
//
////ON_Line AxisPlane::IsoLine( // iso parametric line
////    int dir,              // 0 first parameter varies and second parameter is constant
////                      //   e.g., line(t) = plane(t,c)
////                      // 1 first parameter is constant and second parameter varies
////                      //   e.g., line(t) = plane(c,t)
////    double c           // c = value of constant parameter 
////) const
////{
////    ON_Line line;
////    if (dir) {
////        line.from = PointAt(0.0, c);
////        line.to = PointAt(1.0, c);
////    }
////    else {
////        line.from = PointAt(c, 0.0);
////        line.to = PointAt(c, 1.0);
////    }
////    return line;
////}
//
double AxisPlane::DistanceTo(const IK::Point_3& point) const
{
    // signed distance
    // N.B. equation may not be normalized
    return (point - origin) * zaxis;
}
//
////bool AxisPlane::GetDistanceToBoundingBox(const ON_BoundingBox& Box,
////    double* min, double* max) const
////{
////    //min and max signed distance.  Returns false if plane normal has zero length.
////
////    IK::Vector_3 UnitNormal = Normal();
////    if (!UnitNormal.Unitize())
////        return false;
////
////    double mind, maxd;
////    mind = maxd = (Box.Min() - Origin()) * UnitNormal;
////    int i0, i1, i2;
////    for (i0 = 0; i0 < 2; i0++)
////    {
////        for (i1 = 0; i1 < 2; i1++)
////        {
////            for (i2 = 0; i2 < 2; i2++)
////            {
////                if (i0 || i1 || i2)
////                {
////                    IK::Point_3 P;
////                    P[0] = (i0) ? Box.Max()[0] : Box.Min()[0];
////                    P[1] = (i1) ? Box.Max()[1] : Box.Min()[1];
////                    P[2] = (i2) ? Box.Max()[2] : Box.Min()[2];
////                    double d = (P - Origin()) * UnitNormal;
////                    //double dd = P.DistanceTo(ClosestPointTo(P));
////                    if (d < mind)
////                        mind = d;
////                    else if (d > maxd)
////                        maxd = d;
////                }
////            }
////        }
////    }
////    *min = mind;
////    *max = maxd;
////    return true;
////}
//
//
////double AxisPlane::EquationAt( const IK::Point_3& p) const
////{
////  return plane_equation.ValueAt(p);
////}
//
////double AxisPlane::EquationAt( const ON_4dPoint& p) const
////{
////  return plane_equation.ValueAt(p);
////}
//
////bool AxisPlane::UpdateEquation()
////{
////    // computes equation[] from origin and zaxis.
////    return plane_equation.Create(origin, zaxis);
////}
//
const IK::Point_3& AxisPlane::Origin() const
{
    return origin;
}

const IK::Vector_3& AxisPlane::Xaxis() const
{
    return xaxis;
}

const IK::Vector_3& AxisPlane::Yaxis() const
{
    return yaxis;
}

const IK::Vector_3& AxisPlane::Normal() const
{
    return zaxis;
}

void AxisPlane::SetOrigin(const IK::Point_3& origin_point)
{
    origin = origin_point;
    //UpdateEquation();
}


bool AxisPlane::ClosestPointTo(IK::Point_3 p, double* s, double* t) const
{
    const IK::Vector_3 v = p - origin;
    if (s)
        *s = v * xaxis;
    if (t)
        *t = v * yaxis;
    return true;
}

IK::Point_3 AxisPlane::ClosestPointTo(IK::Point_3 p) const
{
    double s, t;
    ClosestPointTo(p, &s, &t);
    return PointAt(s, t);
}


bool AxisPlane::operator==(const AxisPlane& other) const
{
    return (origin == other.origin && xaxis == other.xaxis && yaxis == other.yaxis && zaxis == other.zaxis)
        ? true : false;
}

bool AxisPlane::operator!=(const AxisPlane& other) const
{
    return AxisPlane::operator==(other) ? false : true;
}


bool AxisPlane::CreateFromNormal(
    const IK::Point_3& P, // point on the plane
    const IK::Vector_3& N  // non-zero normal to the plane
)
{
    origin = P;
    zaxis = N;

   
    
    bool b = CGAL_VectorUtil::Unitize(zaxis);
    CGAL_VectorUtil::PerpendicularTo(zaxis, xaxis);
    CGAL_VectorUtil::Unitize(xaxis);
    yaxis = CGAL::cross_product(zaxis, xaxis);
    CGAL_VectorUtil::Unitize(yaxis);

    //UpdateEquation();

    return b;
}

bool AxisPlane::CreateFromFrame(
    const IK::Point_3& P, // point on the plane
    const IK::Vector_3& X, // non-zero vector in plane
    const IK::Vector_3& Y  // another non-zero vector in the plane
)
{
    const IK::Point_3 localP(P);
    const IK::Vector_3 localX(X);
    const IK::Vector_3 localY(Y);


    origin = localP;

    xaxis = localX;
    CGAL_VectorUtil::Unitize(xaxis);
    yaxis = localY - CGAL_VectorUtil::DotProduct(localY, xaxis) * xaxis;
    CGAL_VectorUtil::Unitize(yaxis);
    zaxis = CGAL::cross_product(xaxis, yaxis);
    bool b = CGAL_VectorUtil::Unitize(zaxis);
    //UpdateEquation();
    if (b)
    {
        // 11 February 2004 Dale Lear
        //     Add more validation checks.
        b = IsValid();
        if (b)
        {
            // make sure zaxis is perp to localY
            if (fabs(localY * zaxis) > ON_SQRT_EPSILON * CGAL_VectorUtil::LengthVecConst(localY))
                b = false;
        }
    }
    return b;
}
//
////bool AxisPlane::CreateFromEquation(
////    const class AxisPlaneEquation& eqn
////)
////{
////    bool b = false;
////
////    plane_equation = eqn;
////
////    zaxis.x = plane_equation.x;
////    zaxis.y = plane_equation.y;
////    zaxis.z = plane_equation.z;
////
////    double d = zaxis.Length();
////    if (d > 0.0) {
////        d = 1.0 / d;
////        zaxis *= d;
////        origin = (-d * plane_equation.d) * zaxis;
////        b = true;
////    }
////    xaxis.PerpendicularTo(zaxis);
////    xaxis.Unitize();
////    yaxis = ON_CrossProduct(zaxis, xaxis);
////    yaxis.Unitize();
////    return b;
////}
////
////bool AxisPlane::CreateFromEquation(
////    const double e[4]    // equation of plane
////)
////{
////    AxisPlaneEquation eqn(e[0], e[1], e[2], e[3]);
////    return CreateFromEquation(eqn);
////}
//
bool AxisPlane::CreateFromPoints(
    const IK::Point_3& P,  // point on the plane
    const IK::Point_3& Q,  // point on the plane
    const IK::Point_3& R   // point on the plane
)
{
    origin = P;
    bool rc = CGAL_VectorUtil::PerpendicularToPoints(P, Q, R, zaxis);
    xaxis = Q - P;
    CGAL_VectorUtil::Unitize(xaxis);
    yaxis = CGAL::cross_product(zaxis, xaxis);
    CGAL_VectorUtil::Unitize(yaxis);

   // if (!plane_equation.Create(origin, zaxis))
     //   rc = false;

    return rc;
}



bool AxisPlane::Transform(CGAL::Aff_transformation_3<IK>& t)

{


    //if (xform.IsIdentity())
    //{
    //    // 27 October 2011 Dale Lear
    //    //    If the plane is valid and the transformation
    //    //    is the identity, then NO changes should be
    //    //    made to the plane's values.  In particular
    //    //    calling CreateFromFrame() can introduce
    //    //    slight fuzz.
    //    //
    //    //    Please discuss any changes with me.
    //    // 
    //    // 
    //    // 
    //    return true;
    //    //return IsValid();
    //}


    IK::Point_3 origin_pt = t.transform(origin);//xform * origin;

    // use care tranforming vectors to get


    // maximum precision and the right answer

    bool bUseVectorXform = (0.0 == t.cartesian(3, 0)
        && 0.0 == t.cartesian(3, 1)
        && 0.0 == t.cartesian(3, 2)
        && 1.0 == t.cartesian(3, 3)
        );


    //IK::Vector_3 xaxis_vec = t.transform(xaxis);
    IK::Vector_3 xaxis_vec = bUseVectorXform ? t.transform(xaxis) : (t.transform(origin + xaxis)) - origin_pt;
    IK::Vector_3 yaxis_vec = bUseVectorXform ? t.transform(yaxis) : (t.transform(origin + yaxis)) - origin_pt;

    return CreateFromFrame(origin_pt, xaxis_vec, yaxis_vec);
}
//
//
////
////bool AxisPlane::SwapCoordinates(int i, int j)
////{
////    bool rc = false;
////    if (0 <= i && i < 3 && 0 <= j && j < 3) {
////        ON_Xform xform(ON_Xform::IdentityTransformation);
////        xform[i][i] = 0.0;
////        xform[j][j] = 0.0;
////        xform[i][j] = 1.0;
////        xform[j][i] = 1.0;
////        rc = Transform(xform);
////    }
////    return rc;
////}
////
////
////// rotate plane about its origin_pt
////bool AxisPlane::Rotate(
////    double s,                 // sin(angle)
////    double c,                 // cos(angle)
////    const IK::Vector_3& axis // axis of rotation
////)
////{
////    return Rotate(s, c, axis, origin);
////}
////
////bool AxisPlane::Rotate(
////    double angle,           // angle in radians
////    const IK::Vector_3& axis // axis of rotation
////)
////{
////    return Rotate(sin(angle), cos(angle), axis);
////}
////
////// rotate plane about a point and axis
////bool AxisPlane::Rotate(
////    double sin_angle,          // sin(angle)
////    double cos_angle,          // cos(angle)
////    const IK::Vector_3& axis, // axis of rotation
////    const IK::Point_3& center // center of rotation
////)
////{
////    bool rc = false;
////    ON_Xform rot;
////    if (center == origin) {
////        rot.Rotation(sin_angle, cos_angle, axis, IK::Point_3::Origin);
////        xaxis = rot * xaxis;
////        yaxis = rot * yaxis;
////        if (!(axis == zaxis))
////            zaxis = rot * zaxis;
////        rc = UpdateEquation();
////    }
////    else {
////        rot.Rotation(sin_angle, cos_angle, axis, center);
////        rc = Transform(rot);
////    }
////    return rc;
////}
////
////bool AxisPlane::Rotate(
////    double a,                   // angle in radians
////    const IK::Vector_3& axis,  // axis of rotation
////    const IK::Point_3& origin_pt  // center of rotation
////)
////{
////    return Rotate(sin(a), cos(a), axis, origin_pt);
////}
////
////bool AxisPlane::Translate(
////    const IK::Vector_3& delta
////)
////{
////    const ON_Xform tr(ON_Xform::TranslationTransformation(delta));
////    return Transform(tr);
////}
//
bool AxisPlane::Flip()
{
    IK::Vector_3 v = xaxis;
    xaxis = yaxis;
    yaxis = v;
    zaxis = -zaxis;
    //UpdateEquation();
    return true;
}

bool AxisPlane::ON_IsOrthogonalFrame(const IK::Vector_3& X, const IK::Vector_3& Y, const IK::Vector_3& Z)
{
    // returns true if X, Y, Z is an orthogonal frame
    //if (!X.IsValid() || !Y.IsValid() || !Z.IsValid())
    //    return false;
    

    double lx = CGAL_VectorUtil::LengthVecConst(X);
    double ly = CGAL_VectorUtil::LengthVecConst(Y);
    double lz = CGAL_VectorUtil::LengthVecConst(Z);
    if (lx <= ON_SQRT_EPSILON)
        return false;
    if (ly <= ON_SQRT_EPSILON)
        return false;
    if (lz <= ON_SQRT_EPSILON)
        return false;
    lx = 1.0 / lx;
    ly = 1.0 / ly;
    lz = 1.0 / lz;
    double xy = (X.x() * Y.x() + X.y() * Y.y() + X.z() * Y.z()) * lx * ly;
    double yz = (Y.x() * Z.x() + Y.y() * Z.y() + Y.z() * Z.z()) * ly * lz;
    double zx = (Z.x() * X.x() + Z.y() * X.y() + Z.z() * X.z()) * lz * lx;
    if (fabs(xy) > ON_SQRT_EPSILON
        || fabs(yz) > ON_SQRT_EPSILON
        || fabs(zx) > ON_SQRT_EPSILON
        )
    {
        double t = 0.0000152587890625;
        if (fabs(xy) >= t || fabs(yz) >= t || fabs(zx) >= t)
            return false;

        // do a more careful (and time consuming check)
        // This fixes RR 22219 and 22276
        IK::Vector_3 V;
        V = (lx * ly) * CGAL::cross_product(X, Y);
        t = fabs((V.x() * Z.x() + V.y() * Z.y() + V.z() * Z.z()) * lz);
        if (fabs(t - 1.0) > ON_SQRT_EPSILON)
            return false;

        V = (ly * lz) * CGAL::cross_product(Y, Z);
        t = fabs((V.x() * X.x() + V.y() * X.y() + V.z() * X.z()) * lx);
        if (fabs(t - 1.0) > ON_SQRT_EPSILON)
            return false;

        V = (lz * lx) * CGAL::cross_product(Z, X);
        t = fabs((V.x() * Y.x() + V.y() * Y.y() + V.z() * Y.z()) * ly);
        if (fabs(t - 1.0) > ON_SQRT_EPSILON)
            return false;
    }
    return true;
}

bool AxisPlane::ON_IsOrthonormalFrame(const IK::Vector_3& X, const IK::Vector_3& Y, const IK::Vector_3& Z)
{
    // returns true if X, Y, Z is an orthonormal frame
    if (!ON_IsOrthogonalFrame(X, Y, Z))
        return false;
    double x = CGAL_VectorUtil::LengthVecConst(X);
    if (fabs(x - 1.0) > ON_SQRT_EPSILON)
        return false;
    x = CGAL_VectorUtil::LengthVecConst(Y);
    if (fabs(x - 1.0) > ON_SQRT_EPSILON)
        return false;
    x = CGAL_VectorUtil::LengthVecConst(Z);
    if (fabs(x - 1.0) > ON_SQRT_EPSILON)
        return false;

    return true;
}

bool AxisPlane::ON_IsRightHandFrame(const IK::Vector_3& X, const IK::Vector_3& Y, const IK::Vector_3&Z)
{
    // returns true if X, Y, Z is an orthonormal right hand frame
    if (!ON_IsOrthonormalFrame(X, Y, Z))
        return false;

    double x = CGAL_VectorUtil::DotProduct(CGAL::cross_product(X, Y), Z);
    if (x <= ON_SQRT_EPSILON)
        return false;

    return true;
}

void AxisPlane::AveragePlane(AxisPlane& averagePlane, IK::Point_3& p0, IK::Point_3& p1) {


    averagePlane = AxisPlane(origin, xaxis, yaxis );
    IK::Point_3 p;
    CGAL_VectorUtil::MidPoint(p0, p1, p);
    averagePlane.SetOrigin(p);
    //return averagePlane;
}

bool AxisPlane::IsValid() //const IK::Vector_3& X, const IK::Vector_3& Y, const IK::Vector_3& Z
{
    //if (!plane_equation.IsValid())
    //    return false;


    //double x = plane_equation.ValueAt(origin);
    //if (fabs(x) > ON_ZERO_TOLERANCE)
    //{
    //    double tol = fabs(origin.MaximumCoordinate()) + fabs(plane_equation.d);
    //    if (tol > 1000.0)// && origin.IsValid()
    //    {
    //        // 8 September 2003 Chuck and Dale:
    //        //   Fixing discrepancy between optimized and debug behavior.
    //        //   In this case, the ON_ZERO_TOLERANCE test worked in release
    //        //   and failed in debug. The principal behind this fix is valid
    //        //   for release builds too.
    //        //   For large point coordinates or planes far from the origin,
    //        //   the best we can hope for is to kill the first 15 or so decimal
    //        //   places.
    //        tol *= (ON_EPSILON * 10.0);
    //        if (fabs(x) > tol)
    //            return false;
    //    }
    //    else
    //        return false;
    //}

    //IK::Vector_3 xx(Xaxis());
    //IK::Vector_3 yy(yaxis);
    //IK::Vector_3 zz(zaxis);
    
    if (!ON_IsRightHandFrame(xaxis,yaxis,zaxis))
        return false;

    //const IK::Vector_3 N = plane_equation.UnitNormal();
    //x = CGAL_VectorUtil::DotProduct(N, zaxis);
    //if (fabs(x - 1.0) > ON_SQRT_EPSILON)
    //    return false;

    return true;
}

//
////
////int ON_ArePointsOnPlane( // returns 0=no, 1 = yes, 2 = pointset is (to tolerance) a single point on the line
////    int dim,     // 2 or 3
////    bool is_rat,
////    int count,
////    int stride, const double* point,
////    const ON_BoundingBox& bbox, // if needed, use ON_GetBoundingBox(dim,is_rat,count,stride,point)
////    const AxisPlane& plane,  // line to test
////    double tolerance
////)
////{
////    double w;
////    int i, j, k;
////
////    if (count < 1)
////        return 0;
////    if (!plane.IsValid())
////    {
////        ON_ERROR("plane parameter is not valid");
////        return 0;
////    }
////    if (!bbox.IsValid())
////    {
////        ON_ERROR("bbox parameter is not valid");
////        return 0;
////    }
////    if (!ON_IsValid(tolerance) || tolerance < 0.0)
////    {
////        ON_ERROR("tolerance must be >= 0.0");
////        return 0;
////    }
////    if (dim < 2 || dim > 3)
////    {
////        ON_ERROR("dim must be 2 or 3");
////        return 0;
////    }
////    if (stride < (is_rat ? (dim + 1) : dim))
////    {
////        ON_ERROR("stride parameter is too small");
////        return 0;
////    }
////    if (0 == point)
////    {
////        ON_ERROR("point parameter is null");
////        return 0;
////    }
////
////    int rc = 0;
////
////    if (tolerance == 0.0) {
////        tolerance = bbox.Tolerance();
////    }
////
////    IK::Point_3 Q;
////
////    // test bounding box to quickly detect the common coordinate axis cases
////    rc = (count == 1 || bbox.Diagonal().Length() <= tolerance) ? 2 : 1;
////    for (i = 0; rc && i < 2; i++) {
////        Q.x = bbox[i].x;
////        for (j = 0; rc && j < 2; j++) {
////            Q.y = bbox[j].y;
////            for (k = 0; rc && k < 2; k++) {
////                Q.z = bbox[k].z;
////                if (Q.DistanceTo(plane.ClosestPointTo(Q)) > tolerance)
////                    rc = 0;
////            }
////        }
////    }
////
////    if (!rc) {
////        // test points one by one
////        Q = IK::Point_3::Origin;
////        rc = (count == 1 || bbox.Diagonal().Length() <= tolerance) ? 2 : 1;
////        if (is_rat) {
////            for (i = 0; i < count; i++) {
////                w = point[dim];
////                if (w == 0.0) {
////                    ON_ERROR("rational point has zero weight");
////                    return 0;
////                }
////                ON_ArrayScale(dim, 1.0 / w, point, &Q.x);
////                if (Q.DistanceTo(plane.ClosestPointTo(Q)) > tolerance) {
////                    rc = 0;
////                    break;
////                }
////                point += stride;
////            }
////        }
////        else {
////            for (i = 0; i < count; i++) {
////                memcpy(&Q.x, point, dim * sizeof(Q.x));
////                if (Q.DistanceTo(plane.ClosestPointTo(Q)) > tolerance) {
////                    rc = 0;
////                    break;
////                }
////                point += stride;
////            }
////        }
////    }
////
////    return rc;
////}