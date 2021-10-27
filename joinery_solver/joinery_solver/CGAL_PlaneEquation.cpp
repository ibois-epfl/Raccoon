//#include "CGAL_PlaneEquation.h"
//#include "compas.h"
//
//
//bool CGAL_PlaneEquation::Create(CGAL_Point P, CGAL_Vector N)
//{
//    bool rc = false;
//    if (P.IsValid() && N.IsValid())
//    {
//        x = N.x;
//        y = N.y;
//        z = N.z;
//        CGAL_Vector* v = (CGAL_Vector*)&x;
//        rc = (fabs(1.0 - v->Length()) > ON_ZERO_TOLERANCE) ? v->Unitize() : true;
//        d = -(x * P.x + y * P.y + z * P.z);
//    }
//    return rc;
//}
//
//const CGAL_PlaneEquation CGAL_PlaneEquation::CreateFromThreePoints(
//    CGAL_Point pointA,
//    CGAL_Point pointB,
//    CGAL_Point pointC
//)
//{
//    if (pointA.IsValid() && pointB.IsValid() && pointC.IsValid())
//    {
//        const CGAL_Vector X = pointB - pointA;
//        const CGAL_Vector Y = pointC - pointA;
//        return CGAL_PlaneEquation::CreateFromPointAndNormal(pointA, CGAL_Vector::CrossProduct(X, Y));
//    }
//    return CGAL_PlaneEquation::NanPlaneEquation;
//}
//
//const CGAL_PlaneEquation CGAL_PlaneEquation::CreateFromPointAndNormal(
//    CGAL_Point point,
//    CGAL_Vector normal
//)
//{
//    if (point.IsValid() && normal.IsValid())
//    {
//        const CGAL_Vector N = normal.UnitVector();
//        if (false == normal.IsUnitVector() || fabs(1.0 - N.Length()) < fabs(1.0 - normal.Length()) * (1.0 - ON_ZERO_TOLERANCE))
//            normal = N; // N is a better unit vector
//        if (normal.IsUnitVector())
//        {
//            CGAL_PlaneEquation e;
//            e.x = normal.x();
//            e.y = normal.y();
//            e.z = normal.z;
//            e.d = -(e.x * point.x + e.y * point.y + e.z * point.z);
//            return e;
//        }
//    }
//    return CGAL_PlaneEquation::NanPlaneEquation;
//}
//
//CGAL_PlaneEquation::CGAL_PlaneEquation()
//    : x(0.0)
//    , y(0.0)
//    , z(0.0)
//    , d(0.0)
//{}
//
//CGAL_PlaneEquation::CGAL_PlaneEquation(double xx, double yy, double zz, double dd)
//    : x(xx)
//    , y(yy)
//    , z(zz)
//    , d(dd)
//{}
//
//void CGAL_PlaneEquation::Set(CGAL_PlaneEquation& plane_equation, double x, double y, double z, double d)
//{
//    plane_equation.x = x;
//    plane_equation.y = y;
//    plane_equation.z = z;
//    plane_equation.d = d;
//}
//
//double CGAL_PlaneEquation::MaximumCoefficient() const
//{
//    double m = fabs(x);
//    if (fabs(y) > m)
//        m = fabs(y);
//    if (fabs(z) > m)
//        m = fabs(z);
//    if (fabs(d) > m)
//        m = fabs(d);
//    return m;
//}
//
//double& CGAL_PlaneEquation::operator[](int i)
//{
//    switch (i)
//    {
//    case 0:
//        return x;
//        break;
//    case 1:
//        return y;
//        break;
//    case 2:
//        return z;
//        break;
//    case 3:
//        return d;
//        break;
//    }
//
//    ON_ERROR("Invalid coefficient index.");
//
//    // send caller something that can be dereferenced without crashing.
//    double* invalid_coefficient = (double*)onmalloc_forever(sizeof(*invalid_coefficient));
//    *invalid_coefficient = ON_DBL_QNAN;
//    return *invalid_coefficient;
//}
//
//double& CGAL_PlaneEquation::operator[](unsigned int i)
//{
//    switch (i)
//    {
//    case 0:
//        return x;
//        break;
//    case 1:
//        return y;
//        break;
//    case 2:
//        return z;
//        break;
//    case 3:
//        return d;
//        break;
//    }
//
//    ON_ERROR("Invalid coefficient index.");
//
//    // send caller something that can be dereferenced without crashing.
//    double* invalid_coefficient = (double*)onmalloc_forever(sizeof(*invalid_coefficient));
//    *invalid_coefficient = ON_DBL_QNAN;
//    return *invalid_coefficient;
//}
//
//double CGAL_PlaneEquation::operator[](int i) const
//{
//    switch (i)
//    {
//    case 0:
//        return x;
//        break;
//    case 1:
//        return y;
//        break;
//    case 2:
//        return z;
//        break;
//    case 3:
//        return d;
//        break;
//    }
//
//    ON_ERROR("Invalid coefficient index.");
//    return ON_UNSET_VALUE;
//}
//
//double CGAL_PlaneEquation::operator[](unsigned int i) const
//{
//    switch (i)
//    {
//    case 0:
//        return x;
//        break;
//    case 1:
//        return y;
//        break;
//    case 2:
//        return z;
//        break;
//    case 3:
//        return d;
//        break;
//    }
//
//    ON_ERROR("Invalid coefficient index.");
//    return ON_UNSET_VALUE;
//}
//
//CGAL_PlaneEquation CGAL_PlaneEquation::NegatedPlaneEquation() const
//{
//    // do not negate ON_UNSET_VALUE or NaN
//    return CGAL_PlaneEquation(ON_IS_VALID(x) ? -x : x, ON_IS_VALID(y) ? -y : y, ON_IS_VALID(z) ? -z : z, ON_IS_VALID(d) ? -d : d);
//}
//
//
//CGAL_PlaneEquation CGAL_PlaneEquation::UnitizedPlaneEquation() const
//{
//    if (IsSet())
//    {
//        // 15 September 2003 Dale Lear
//        //     Added the ON_IS_FINITE and ON_DBL_MIN test.  See CGAL_Vector::Length()
//        //     for details.
//        double dd = DirectionLength();
//
//        if (ON_IS_FINITE(dd))
//        {
//            if (dd > ON_DBL_MIN)
//            {
//                return CGAL_PlaneEquation(x / dd, y / dd, z / dd, d / dd);
//            }
//
//            if (dd > 0.0)
//            {
//                // This code is rarely used and can be slow.
//                // It multiplies by 2^1023 in an attempt to 
//                // normalize the coordinates.
//                // If the renormalization works, then we're
//                // ok.  If the renormalization fails, we
//                // return false.
//                CGAL_PlaneEquation tmp(
//                    x * 8.9884656743115795386465259539451e+307,
//                    y * 8.9884656743115795386465259539451e+307,
//                    z * 8.9884656743115795386465259539451e+307,
//                    d * 8.9884656743115795386465259539451e+307
//                );
//                dd = tmp.DirectionLength();
//                if (ON_IS_FINITE(dd) && dd > ON_DBL_MIN)
//                {
//                    return CGAL_PlaneEquation(tmp.x / dd, tmp.y / dd, tmp.z / dd, tmp.d / dd);
//                }
//            }
//        }
//    }
//    else if (d == ON_UNSET_VALUE)
//    {
//        CGAL_Vector v = Direction();
//        if (v.IsValid() && v.Unitize())
//        {
//            return CGAL_PlaneEquation(v.x, v.y, v.z, ON_UNSET_VALUE);
//        }
//    }
//
//    return CGAL_PlaneEquation::ZeroPlaneEquation;
//}
//
//bool CGAL_PlaneEquation::IsValid() const
//{
//    return (ON_IS_VALID(x) && ON_IS_VALID(y) && ON_IS_VALID(z) && ON_IS_VALID(d));
//}
//
//bool CGAL_PlaneEquation::IsSet() const
//{
//    return (ON_IS_VALID(x) && ON_IS_VALID(y) && ON_IS_VALID(z) && ON_IS_VALID(d)
//        && (0.0 != x || 0.0 != y || 0.0 != z)
//        );
//}
//
//bool CGAL_PlaneEquation::IsUnitized() const
//{
//    return (IsSet() && CGAL_Vector(x, y, z).IsUnitVector()) ? true : false;
//}
//
//CGAL_Vector CGAL_PlaneEquation::Direction() const
//{
//    return CGAL_Vector(x, y, z);
//}
//
//double CGAL_PlaneEquation::DirectionLength() const
//{
//    return ((const CGAL_Vector*)&x)->Length();
//}
//
//
//CGAL_Vector CGAL_PlaneEquation::UnitNormal() const
//{
//    CGAL_Vector normal(x, y, z);
//    if (false == normal.IsUnitVector() && false == normal.Unitize())
//        normal = CGAL_Vector::ZeroVector;
//    return normal;
//}
//
//int CGAL_PlaneEquation::IsParallelTo(const CGAL_PlaneEquation& other, double angle_tolerance /*= ON_DEFAULT_ANGLE_TOLERANCE*/) const
//{
//    return CGAL_Vector(x, y, z).IsParallelTo(CGAL_Vector(other.x, other.y, other.z), angle_tolerance);
//}
//
//double CGAL_PlaneEquation::ZeroTolerance() const
//{
//    double zero_tolerance = 0.0;
//    CGAL_Vector N(x, y, z);
//    if (N.Unitize() && ON_IS_VALID(d))
//    {
//        const CGAL_Point P(-d * N.x, -d * N.y, -d * N.z);
//
//        zero_tolerance = fabs(ValueAt(P));
//
//        CGAL_Vector X;
//        X.PerpendicularTo(N);
//        X.Unitize();
//
//        double t = fabs(ValueAt(P + X));
//        if (t > zero_tolerance)
//            zero_tolerance = t;
//        t = fabs(ValueAt(P - X));
//        if (t > zero_tolerance)
//            zero_tolerance = t;
//        t = fabs(ValueAt(P + d * X));
//        if (t > zero_tolerance)
//            zero_tolerance = t;
//        t = fabs(ValueAt(P - d * X));
//        if (t > zero_tolerance)
//            zero_tolerance = t;
//
//        CGAL_Vector Y = ON_CrossProduct(N, X);
//        Y.Unitize();
//
//        t = fabs(ValueAt(P + Y));
//        if (t > zero_tolerance)
//            zero_tolerance = t;
//        t = fabs(ValueAt(P - Y));
//        if (t > zero_tolerance)
//            zero_tolerance = t;
//        t = fabs(ValueAt(P + d * Y));
//        if (t > zero_tolerance)
//            zero_tolerance = t;
//        t = fabs(ValueAt(P - d * Y));
//        if (t > zero_tolerance)
//            zero_tolerance = t;
//    }
//
//    return zero_tolerance;
//}
//
//bool CGAL_PlaneEquation::Transform(const ON_Xform& xform)
//{
//    bool rc = IsValid();
//    if (rc)
//    {
//        // Apply the inverse transpose to the equation parameters
//        ON_Xform T(xform);
//        rc = T.Invert();
//        if (rc)
//        {
//            const double xx = x;
//            const double yy = y;
//            const double zz = z;
//            const double dd = d;
//            x = T.m_xform[0][0] * xx + T.m_xform[1][0] * yy + T.m_xform[2][0] * zz + T.m_xform[3][0] * dd;
//            y = T.m_xform[0][1] * xx + T.m_xform[1][1] * yy + T.m_xform[2][1] * zz + T.m_xform[3][1] * dd;
//            z = T.m_xform[0][2] * xx + T.m_xform[1][2] * yy + T.m_xform[2][2] * zz + T.m_xform[3][2] * dd;
//            d = T.m_xform[0][3] * xx + T.m_xform[1][3] * yy + T.m_xform[2][3] * zz + T.m_xform[3][3] * dd;
//        }
//    }
//    return rc;
//}
//
//double CGAL_PlaneEquation::ValueAt(CGAL_Point P) const
//{
//    return (x * P.x + y * P.y + z * P.z + d);
//}
//
//double CGAL_PlaneEquation::ValueAt(ON_4dPoint P) const
//{
//    return (x * P.x + y * P.y + z * P.z + d * P.w);
//}
//
//double CGAL_PlaneEquation::ValueAt(CGAL_Vector P) const
//{
//    return (x * P.x + y * P.y + z * P.z + d);
//}
//
//double CGAL_PlaneEquation::ValueAt(double xx, double yy, double zz) const
//{
//    return (x * xx + y * yy + z * zz + d);
//}
//
//double* CGAL_PlaneEquation::ValueAt(
//    int Pcount,
//    const ON_3fPoint* P,
//    double* value,
//    double value_range[2]
//) const
//{
//    if (Pcount <= 0 || 0 == P)
//        return 0;
//
//    int i;
//    double s;
//    const double* e = &x;
//
//    if (0 == value)
//        value = (double*)onmalloc(Pcount * sizeof(*value));
//    if (0 == value)
//        return 0;
//
//    if (0 != value_range)
//    {
//        value[0] = s = e[0] * ((double)P[0].x) + e[1] * ((double)P[0].y) + e[2] * ((double)P[0].z) + e[3];
//        value_range[0] = s;
//        value_range[1] = s;
//        for (i = 1; i < Pcount; i++)
//        {
//            value[i] = s = e[0] * ((double)P[i].x) + e[1] * ((double)P[i].y) + e[2] * ((double)P[i].z) + e[3];
//            if (s < value_range[0])
//                value_range[0] = s;
//            else if (s > value_range[1])
//                value_range[1] = s;
//        }
//    }
//    else
//    {
//        for (i = 0; i < Pcount; i++)
//        {
//            value[i] = e[0] * ((double)P[i].x) + e[1] * ((double)P[i].y) + e[2] * ((double)P[i].z) + e[3];
//        }
//    }
//
//    return value;
//}
//
//double* CGAL_PlaneEquation::ValueAt(
//    int Pcount,
//    const CGAL_Point* P,
//    double* value,
//    double value_range[2]
//) const
//{
//    if (Pcount <= 0 || 0 == P)
//        return 0;
//
//    int i;
//    double s;
//    const double* e = &x;
//
//    if (0 == value)
//        value = (double*)onmalloc(Pcount * sizeof(*value));
//    if (0 == value)
//        return 0;
//
//    if (0 != value_range)
//    {
//        value[0] = s = e[0] * (P[0].x) + e[1] * (P[0].y) + e[2] * (P[0].z) + e[3];
//        value_range[0] = s;
//        value_range[1] = s;
//        for (i = 1; i < Pcount; i++)
//        {
//            value[i] = s = e[0] * (P[i].x) + e[1] * (P[i].y) + e[2] * (P[i].z) + e[3];
//            if (s < value_range[0])
//                value_range[0] = s;
//            else if (s > value_range[1])
//                value_range[1] = s;
//        }
//    }
//    else
//    {
//        for (i = 0; i < Pcount; i++)
//        {
//            value[i] = e[0] * (P[i].x) + e[1] * (P[i].y) + e[2] * (P[i].z) + e[3];
//        }
//    }
//
//    return value;
//}
//
//ON_Interval CGAL_PlaneEquation::ValueRange(
//    size_t point_list_count,
//    const CGAL_Point* point_list
//) const
//{
//    CGAL_PointListRef vertex_list;
//    vertex_list.SetFromDoubleArray(point_list_count, 3, (const double*)point_list);
//    return ValueRange(vertex_list);
//}
//
//ON_Interval CGAL_PlaneEquation::ValueRange(
//    const ON_SimpleArray< CGAL_Point >& point_list
//) const
//{
//    return ValueRange(point_list.UnsignedCount(), point_list.Array());
//}
//
//ON_Interval CGAL_PlaneEquation::ValueRange(
//    size_t point_list_count,
//    const ON_3fPoint* point_list
//) const
//{
//    CGAL_PointListRef vertex_list;
//    vertex_list.SetFromFloatArray(point_list_count, 3, (const float*)point_list);
//    return ValueRange(vertex_list);
//}
//
//ON_Interval CGAL_PlaneEquation::ValueRange(
//    const ON_SimpleArray< ON_3fPoint >& point_list
//) const
//{
//    return ValueRange(point_list.UnsignedCount(), point_list.Array());
//}
//
//ON_Interval CGAL_PlaneEquation::ValueRange(
//    const class CGAL_PointListRef& point_list
//) const
//{
//    return ValueRange(point_list.PointCount(), 0, point_list);
//}
//
//
//ON_Interval CGAL_PlaneEquation::ValueRange(
//    size_t point_index_count,
//    const unsigned int* point_index_list,
//    const class CGAL_PointListRef& point_list
//) const
//{
//    size_t point_index_stride = 1;
//
//    return ValueRange(
//        point_index_count,
//        point_index_stride,
//        point_index_list,
//        point_list
//    );
//}
//
//ON_Interval CGAL_PlaneEquation::ValueRange(
//    size_t point_index_count,
//    size_t point_index_stride,
//    const unsigned int* point_index_list,
//    const class CGAL_PointListRef& point_list
//) const
//{
//    ON_Interval value_interval(ON_Interval::EmptyInterval);
//
//    for (;;)
//    {
//        if (false == IsValid())
//            break; // invalid plane equation;
//
//        double h;
//        const unsigned int vertex_count = point_list.PointCount();
//        if (0 == vertex_count)
//            break;
//        const unsigned int point_count = (point_index_count > 0 && point_index_count < (size_t)ON_UNSET_UINT_INDEX)
//            ? ((unsigned int)point_index_count)
//            : 0;
//        if (0 == point_count)
//            break;
//
//        if (0 != point_index_list)
//        {
//            const unsigned int point_stride = (point_index_stride > 0 && point_index_stride < (size_t)ON_UNSET_UINT_INDEX)
//                ? ((unsigned int)point_index_stride)
//                : 0;
//            if (0 == point_stride)
//                break;
//            for (unsigned int i = 0; i < point_count * point_stride; i += point_stride)
//            {
//                unsigned int j = point_index_list[i];
//                if (j >= vertex_count)
//                    continue;
//                h = ValueAt(point_list[j]);
//                if (ON_IsValid(h))
//                {
//                    value_interval.Set(h, h);
//                    for (i++; i < point_count * point_stride; i += point_stride)
//                    {
//                        unsigned int j_local = point_index_list[i];
//                        if (j_local >= vertex_count)
//                            continue;
//                        h = ValueAt(point_list[j_local]);
//                        if (ON_IsValid(h))
//                        {
//                            if (h < value_interval.m_t[0])
//                                value_interval.m_t[0] = h;
//                            else if (h > value_interval.m_t[1])
//                                value_interval.m_t[1] = h;
//                        }
//                    }
//                    break;
//                }
//            }
//        }
//        else if (point_count <= vertex_count)
//        {
//            for (unsigned int i = 0; i < point_count; i++)
//            {
//                h = ValueAt(point_list[i]);
//                if (ON_IsValid(h))
//                {
//                    value_interval.Set(h, h);
//                    for (i++; i < point_count; i++)
//                    {
//                        h = ValueAt(point_list[i]);
//                        if (ON_IsValid(h))
//                        {
//                            if (h < value_interval.m_t[0])
//                                value_interval.m_t[0] = h;
//                            else if (h > value_interval.m_t[1])
//                                value_interval.m_t[1] = h;
//                        }
//                    }
//                    break;
//                }
//            }
//        }
//
//        break;
//    }
//    return value_interval;
//}
//
//CGAL_Point CGAL_PlaneEquation::ClosestPointTo(CGAL_Point P) const
//{
//    const double t = -(x * P.x + y * P.y + z * P.z + d) / (x * x + y * y + z * z);
//    return CGAL_Point(P.x + t * x, P.y + t * y, P.z + t * z);
//}
//
//double CGAL_PlaneEquation::MinimumValueAt(const ON_BoundingBox& bbox) const
//{
//    double xx, yy, zz, s;
//
//    s = x * bbox.m_min.x;
//    if (s < (xx = x * bbox.m_max.x)) xx = s;
//
//    s = y * bbox.m_min.y;
//    if (s < (yy = y * bbox.m_max.y)) yy = s;
//
//    s = z * bbox.m_min.z;
//    if (s < (zz = z * bbox.m_max.z)) zz = s;
//
//    return (xx + yy + zz + d);
//}
//
//double CGAL_PlaneEquation::MaximumValueAt(const ON_BoundingBox& bbox) const
//{
//    double xx, yy, zz, s;
//
//    s = x * bbox.m_min.x;
//    if (s > (xx = x * bbox.m_max.x)) xx = s;
//
//    s = y * bbox.m_min.y;
//    if (s > (yy = y * bbox.m_max.y)) yy = s;
//
//    s = z * bbox.m_min.z;
//    if (s > (zz = z * bbox.m_max.z)) zz = s;
//
//    return (xx + yy + zz + d);
//}
//
//
//double CGAL_PlaneEquation::MaximumAbsoluteValueAt(
//    bool bRational,
//    int point_count,
//    int point_stride,
//    const double* points,
//    double stop_value
//) const
//{
//    double value, max_value, w;
//
//    if (point_count < 1
//        || point_stride < (bRational ? 4 : 3)
//        || 0 == points
//        )
//    {
//        return ON_UNSET_VALUE;
//    }
//
//    if (ON_IsValid(stop_value))
//    {
//        if (bRational)
//        {
//            w = points[3];
//            w = (0.0 != w) ? 1.0 / w : 1.0;
//            max_value = fabs(x * w * points[0] + y * w * points[1] + z * w * points[2] + points[3]);
//            if (max_value > stop_value)
//                return max_value;
//            for (point_count--; point_count--; /*empty iterator*/)
//            {
//                points += point_stride;
//                w = points[3];
//                w = (0.0 != w) ? 1.0 / w : 1.0;
//                value = fabs(x * w * points[0] + y * w * points[1] + z * w * points[2] + points[3]);
//                if (value > max_value)
//                {
//                    if ((max_value = value) > stop_value)
//                        return max_value;
//                }
//            }
//        }
//        else
//        {
//            max_value = fabs(x * points[0] + y * points[1] + z * points[2] + d);
//            if (max_value > stop_value)
//                return max_value;
//            for (point_count--; point_count--; /*empty iterator*/)
//            {
//                points += point_stride;
//                value = fabs(x * points[0] + y * points[1] + z * points[2] + d);
//                if (value > max_value)
//                {
//                    if ((max_value = value) > stop_value)
//                        return max_value;
//                }
//            }
//        }
//    }
//    else
//    {
//        if (bRational)
//        {
//            w = points[3];
//            w = (0.0 != w) ? 1.0 / w : 1.0;
//            max_value = fabs(x * w * points[0] + y * w * points[1] + z * w * points[2] + points[3]);
//            for (point_count--; point_count--; /*empty iterator*/)
//            {
//                points += point_stride;
//                w = points[3];
//                w = (0.0 != w) ? 1.0 / w : 1.0;
//                value = fabs(x * w * points[0] + y * w * points[1] + z * w * points[2] + points[3]);
//                if (value > max_value)
//                    max_value = value;
//            }
//        }
//        else
//        {
//            max_value = fabs(x * points[0] + y * points[1] + z * points[2] + d);
//            for (point_count--; point_count--; /*empty iterator*/)
//            {
//                points += point_stride;
//                value = fabs(x * points[0] + y * points[1] + z * points[2] + d);
//                if (value > max_value)
//                    max_value = value;
//            }
//        }
//    }
//
//    return max_value;
//}
//
//double CGAL_PlaneEquation::MaximumValueAt(
//    bool bRational,
//    int point_count,
//    int point_stride,
//    const double* points,
//    double stop_value
//) const
//{
//    double value, max_value, w;
//
//    if (point_count < 1
//        || point_stride < (bRational ? 4 : 3)
//        || 0 == points
//        )
//    {
//        return ON_UNSET_VALUE;
//    }
//
//    if (ON_IsValid(stop_value))
//    {
//        if (bRational)
//        {
//            w = points[3];
//            w = (0.0 != w) ? 1.0 / w : 1.0;
//            max_value = x * w * points[0] + y * w * points[1] + z * w * points[2] + points[3];
//            if (max_value > stop_value)
//                return max_value;
//            for (point_count--; point_count--; /*empty iterator*/)
//            {
//                points += point_stride;
//                w = points[3];
//                w = (0.0 != w) ? 1.0 / w : 1.0;
//                value = x * w * points[0] + y * w * points[1] + z * w * points[2] + points[3];
//                if (value > max_value)
//                {
//                    if ((max_value = value) > stop_value)
//                        return max_value;
//                }
//            }
//        }
//        else
//        {
//            max_value = x * points[0] + y * points[1] + z * points[2] + d;
//            if (max_value > stop_value)
//                return max_value;
//            for (point_count--; point_count--; /*empty iterator*/)
//            {
//                points += point_stride;
//                value = x * points[0] + y * points[1] + z * points[2] + d;
//                if (value > max_value)
//                {
//                    if ((max_value = value) > stop_value)
//                        return max_value;
//                }
//            }
//        }
//    }
//    else
//    {
//        if (bRational)
//        {
//            w = points[3];
//            w = (0.0 != w) ? 1.0 / w : 1.0;
//            max_value = x * w * points[0] + y * w * points[1] + z * w * points[2] + points[3];
//            for (point_count--; point_count--; /*empty iterator*/)
//            {
//                points += point_stride;
//                w = points[3];
//                w = (0.0 != w) ? 1.0 / w : 1.0;
//                value = x * w * points[0] + y * w * points[1] + z * w * points[2] + points[3];
//                if (value > max_value)
//                    max_value = value;
//            }
//        }
//        else
//        {
//            max_value = x * points[0] + y * points[1] + z * points[2] + d;
//            for (point_count--; point_count--; /*empty iterator*/)
//            {
//                points += point_stride;
//                value = x * points[0] + y * points[1] + z * points[2] + d;
//                if (value > max_value)
//                    max_value = value;
//            }
//        }
//    }
//
//    return max_value;
//}
//
//double CGAL_PlaneEquation::MinimumValueAt(
//    bool bRational,
//    int point_count,
//    int point_stride,
//    const double* points,
//    double stop_value
//) const
//{
//    double value, min_value, w;
//
//    if (point_count < 1
//        || point_stride < (bRational ? 4 : 3)
//        || 0 == points
//        )
//    {
//        return ON_UNSET_VALUE;
//    }
//
//    if (ON_IsValid(stop_value))
//    {
//        if (bRational)
//        {
//            w = points[3];
//            w = (0.0 != w) ? 1.0 / w : 1.0;
//            min_value = x * w * points[0] + y * w * points[1] + z * w * points[2] + points[3];
//            if (min_value < stop_value)
//                return min_value;
//            for (point_count--; point_count--; /*empty iterator*/)
//            {
//                points += point_stride;
//                w = points[3];
//                w = (0.0 != w) ? 1.0 / w : 1.0;
//                value = x * w * points[0] + y * w * points[1] + z * w * points[2] + points[3];
//                if (value < min_value)
//                {
//                    if ((min_value = value) < stop_value)
//                        return min_value;
//                }
//            }
//        }
//        else
//        {
//            min_value = x * points[0] + y * points[1] + z * points[2] + d;
//            if (min_value < stop_value)
//                return min_value;
//            for (point_count--; point_count--; /*empty iterator*/)
//            {
//                points += point_stride;
//                value = x * points[0] + y * points[1] + z * points[2] + d;
//                if (value < min_value)
//                {
//                    if ((min_value = value) < stop_value)
//                        return min_value;
//                }
//            }
//        }
//    }
//    else
//    {
//        if (bRational)
//        {
//            w = points[3];
//            w = (0.0 != w) ? 1.0 / w : 1.0;
//            min_value = x * w * points[0] + y * w * points[1] + z * w * points[2] + points[3];
//            for (point_count--; point_count--; /*empty iterator*/)
//            {
//                points += point_stride;
//                w = points[3];
//                w = (0.0 != w) ? 1.0 / w : 1.0;
//                value = x * w * points[0] + y * w * points[1] + z * w * points[2] + points[3];
//                if (value < min_value)
//                    min_value = value;
//            }
//        }
//        else
//        {
//            min_value = x * points[0] + y * points[1] + z * points[2] + d;
//            for (point_count--; point_count--; /*empty iterator*/)
//            {
//                points += point_stride;
//                value = x * points[0] + y * points[1] + z * points[2] + d;
//                if (value < min_value)
//                    min_value = value;
//            }
//        }
//    }
//
//    return min_value;
//}
//
//
//bool CGAL_PlaneEquation::IsNearerThan(
//    const ON_BezierCurve& bezcrv,
//    double s0,
//    double s1,
//    int sample_count,
//    double endpoint_tolerance,
//    double interior_tolerance,
//    double* smin,
//    double* smax
//) const
//{
//    int i, n;
//    double smn, smx, vmn, vmx, s, v, w;
//    CGAL_Point P;
//    P.z = 0.0; // for 2d curves
//
//    sample_count--;
//    s = 0.5 * (s0 + s1);
//    bezcrv.Evaluate(s, 0, 3, &P.x);
//    vmn = vmx = x * P.x + y * P.y + z * P.z + d;
//    smn = smx = s;
//    if (vmn > interior_tolerance)
//    {
//        if (smin)
//            *smin = s;
//        if (smax)
//            *smax = s;
//        return false;
//    }
//
//    if (endpoint_tolerance >= 0.0)
//    {
//        bezcrv.Evaluate(s0, 0, 3, &P.x);
//        v = x * P.x + y * P.y + z * P.z + d;
//        if (v > endpoint_tolerance)
//        {
//            if (smin)
//                *smin = smn;
//            if (smax)
//                *smax = s0;
//            return false;
//        }
//        if (v < vmn) { vmn = v; smn = s0; }
//        else if (v > vmx) { vmx = v; smx = s0; }
//
//        bezcrv.Evaluate(s1, 0, 3, &P.x);
//        v = x * P.x + y * P.y + z * P.z + d;
//        if (v > endpoint_tolerance)
//        {
//            if (smin)
//                *smin = smn;
//            if (smax)
//                *smax = s1;
//            return false;
//        }
//        if (v < vmn) { vmn = v; smn = s1; }
//        else if (v > vmx) { vmx = v; smx = s1; }
//    }
//
//    w = 0.5;
//    for (n = 4; sample_count > 0; n *= 2)
//    {
//        w *= 0.5;
//        for (i = 1; i < n; i += 2) // do not test sample_count here
//        {
//            s = w * i;
//            s = (1.0 - s) * s0 + s * s1;
//            bezcrv.Evaluate(s, 0, 3, &P.x);
//            v = x * P.x + y * P.y + z * P.z + d;
//
//            if (v < vmn)
//            {
//                vmn = v;
//                smn = s;
//            }
//            else if (v > vmx)
//            {
//                vmx = v;
//                smx = s;
//                if (vmx > interior_tolerance)
//                {
//                    if (smin)
//                        *smin = smn;
//                    if (smax)
//                        *smax = s;
//                    return false;
//                }
//            }
//
//            sample_count--;
//        }
//    }
//
//    if (smin)
//        *smin = smn;
//    if (smax)
//        *smax = smx;
//    return true;
//}
