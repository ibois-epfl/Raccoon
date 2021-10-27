#pragma once
#if !defined(CGAL_PLANEAXIS_INC_)
#define CGAL_PLANEAXIS_INC_


#include "compas.h"


class CGAL_PlaneAxis
{
public:

    /*
    Description:
      The default constructor creates a plane
      with orgin=(0,0,0), xaxis=(1,0,0), yaxis=(0,1,0)
      zaxis=(0,0,1), and equation=(0,0,1,0).
    */
    CGAL_PlaneAxis();

    /*
    Description:
      Construct a plane from a point and normal vector.
    Parameters:
      origin - [in] point on the plane
      normal - [in] non-zero normal to the plane
    Remarks:
      origin = point, zaxis = unitized normal, xaxis
      xaxis set with xaxis.PerpindicularTo(zaxis).
    See Also:
      CGAL_PlaneAxis::CreateFromNormal
    */
    CGAL_PlaneAxis(
        const CGAL_Point& origin,
        const CGAL_Vector& normal
    );

    /*
    Description:
      Construct a plane from a point, and two vectors in
      the plane.
    Parameters:
      origin - [in] point on the plane
      x_dir - [in] non-zero vector in the plane that
          determines the xaxis direction.
      y_dir - [in] non-zero vector not parallel to x_dir
          that is used to determine the yaxis direction.
          y_dir does not have to be perpendicular to x_dir.
    */
    CGAL_PlaneAxis(
        const CGAL_Point& origin,
        const CGAL_Vector& x_dir,
        const CGAL_Vector& y_dir
    );

    /*
    Description:
      Construct a plane from three non-collinear points.
    Parameters:
      origin - [in] point on the plane
      x_point - [in] second point in the plane.
          The xaxis will be parallel to x_point-origin.
      y_point - [in] third point on the plane that is
          not collinear with the first two points.
          yaxis*(y_point-origin) will be > 0.
    */
    CGAL_PlaneAxis(
        const CGAL_Point& origin,
        const CGAL_Point& x_point,
        const CGAL_Point& y_point
    );

    /*
    Description:
      Construct a plane from an equation.
    Parameters:
      equation - [in] an array of 4 doubles with
         one of equation[0], equation[1], or equation[2]
         being non-zero.
    */
    CGAL_PlaneAxis(
        const double equation[4]
    );

    CGAL_PlaneAxis(
        const CGAL_PlaneAxisEquation& plane_equation
    );

    ~CGAL_PlaneAxis();

    bool operator==(const CGAL_PlaneAxis&) const;
    bool operator!=(const CGAL_PlaneAxis&) const;

    /*
    Description:
      Create a plane from a point and normal vector.
    Parameters:
      origin - [in] point on the plane
      normal - [in] non-zero normal to the plane
    Remarks:
      origin = point, zaxis = unitized normal, xaxis
      xaxis set with xaxis.PerpindicularTo(zaxis).
    Returns:
      true if valid plane is created.
    */
    bool CreateFromNormal(
        const CGAL_Point& origin,
        const CGAL_Vector& normal
    );

    /*
    Description:
      Construct a plane from a point, and two vectors in
      the plane.
    Parameters:
      origin - [in] point on the plane
      x_dir - [in] non-zero vector in the plane that
          determines the xaxis direction.
      y_dir - [in] non-zero vector not parallel to x_dir
          that is used to determine the yaxis direction.
          y_dir does not have to be perpendicular to x_dir.
    Returns:
      true if valid plane is created.
    */
    bool CreateFromFrame(
        const CGAL_Point& origin,
        const CGAL_Vector& x_dir,
        const CGAL_Vector& y_dir
    );

    /*
    Description:
      Construct a plane from three non-collinear points.
    Parameters:
      origin - [in] point on the plane
      point_on_x - [in] second point in the plane.
          The xaxis will be parallel to x_point-origin.
      point_on - [in] third point on the plane that is
          not collinear with the first two points.
          yaxis*(y_point-origin) will be > 0.
    Returns:
      true if valid plane is created.
    */
    bool CreateFromPoints(
        const CGAL_Point& origin,
        const CGAL_Point& point_on_x,
        const CGAL_Point& point_on
    );

    /*
    Description:
      Construct a plane from an equation.
    Parameters:
      equation - [in] an array of 4 doubles with
         one of equation[0], equation[1], or equation[2]
         being non-zero.
    Remarks:
      points on the plane will satisfy
      x*equation[0] +y*equation[1] + z*equation[2] + equation[3] = 0
    Returns:
      true if valid plane is created.
    */
    bool CreateFromEquation(
        const double equation[4]
    );

    bool CreateFromEquation(
        const class CGAL_PlaneAxisEquation& plane_equation
    );

    /*
    Returns:
      CGAL_PlaneAxis::UnsetPlane if input is not valid.
    */
    static CGAL_PlaneAxis FromPointList(
        size_t point_list_count,
        const CGAL_Point* point_list
    );

    ///*
    //Returns:
    //  CGAL_PlaneAxis::UnsetPlane if input is not valid.
    //*/
    //static CGAL_PlaneAxis FromPointList(
    //    const ON_SimpleArray< CGAL_Point >& point_list
    //);


    ///*
    //Returns:
    //  CGAL_PlaneAxis::UnsetPlane if input is not valid.
    //*/
    //static CGAL_PlaneAxis FromPointList(
    //    size_t point_list_count,
    //    const ON_3fPoint* point_list
    //);

    ///*
    //Returns:
    //  CGAL_PlaneAxis::UnsetPlane if input is not valid.
    //*/
    //static CGAL_PlaneAxis FromPointList(
    //    const ON_SimpleArray< ON_3fPoint >& point_list
    //);

    ///*
    //Returns:
    //  CGAL_PlaneAxis::UnsetPlane if input is not valid.
    //*/
    //static CGAL_PlaneAxis FromPointList(
    //    const class CGAL_PointListRef& point_list
    //);

    ///*
    //Returns:
    //  CGAL_PlaneAxis::UnsetPlane if input is not valid.
    //*/
    //static CGAL_PlaneAxis FromPointList(
    //    size_t point_index_count,
    //    const unsigned int* point_index_list,
    //    const class CGAL_PointListRef& point_list
    //);

    //static CGAL_PlaneAxis FromPointList(
    //    size_t point_index_count,
    //    size_t point_index_stride,
    //    const unsigned int* point_index_list,
    //    const class CGAL_PointListRef& point_list
    //);

    /*
    Description:
      Test plane to see if it is valid.
    Returns:
      true if all fields contain reasonable
      information and equation jibes with point and zaxis.
    */
    bool IsValid() const;

    /*
    Returns:
      Plane origin.
    */
    const CGAL_Point& Origin() const;

    /*
    Returns:
      Plane unit x-axis.
    */
    const CGAL_Vector& Xaxis() const;

    /*
    Returns:
      Plane unit y-axis.
    */
    const CGAL_Vector& Yaxis() const;

    /*
    Returns:
      Plane unit normal.
    */
    const CGAL_Vector& Normal() const;


    /*
    Description:
      Set the origin and update the plane equation
    Parameters:
      origin - [in] the new origin
    */
    void SetOrigin(const CGAL_Point& origin);

    /*
    Description:
      Evaluate a point on the plane
    Parameters:
      u - [in]
      v - [in] evaulation parameters
    Returns:
      plane.origin + u*plane.xaxis + v*plane.yaxis
    */
    CGAL_Point PointAt(
        double u,
        double v
    ) const;

    /*
    Description:
      Evaluate a point on the plane
    Parameters:
      u - [in]
      v - [in] evaluation parameters
      w - [in] elevation parameter
    Returns:
      plane.origin + u*plane.xaxis + v*plane.yaxis + z*plane.zaxis
    */
    CGAL_Point PointAt(
        double u,
        double v,
        double w
    ) const;

    /*
    Description:
      Get an isoparameteric line on the plane.
    Parameters:
      dir - [in] direction of iso-parametric line
          0: first parameter varies and second parameter is constant
             e.g., line(t) = plane(t,c)
          1: first parameter is constant and second parameter varies
             e.g., line(t) = plane(c,t)
      c - [in] value of constant parameter
    Returns:
      iso-parametric line
    */
    //ON_Line IsoLine(
    //    int dir,
    //    double c
    //) const;

    /*
    Description:
      Get signed distance from the plane to a point.
    Parameters:
      point - [in]
    Returns:
      Signed distance from a point to a plane.
    Remarks:
      If the point is on the plane, the distance is 0.
      If the point is above the plane, the distance is > 0.
      If the point is below the plane the distance is < 0.
      The zaxis determines the plane's orientation.
    */
    double DistanceTo(
        const CGAL_Point& point
    ) const;


    //bool GetDistanceToBoundingBox(
    //    //returns false if plane has zero length normal
    //    const ON_BoundingBox&, // Box

//output
//double* min,    // min signed dist from plane to box 
//double* max     //max signed dist from plane to box
//) const;

    /*
    Description:
      Update the plane equation based on the current values
      of the origin and zaxis.
    Returns:
      true if successful.  false if zaxis is zero.
    Remarks:
      If you modify a plane's origin or zaxis, call UpdateEquation()
      to set equation[].
    */
    bool UpdateEquation();

    /*
    Description:
      Get point on plane that is closest to a given point.
    Parameters:
      world_point - [in] 3d point
      u - [out]
      v - [out] The point CGAL_PlaneAxis::PointAt(*u,*v) is the point
                on the plane that is closest to world_point.
    Returns:
      true if successful.
    */
    bool ClosestPointTo(
        CGAL_Point world_point,
        double* u,
        double* v
    ) const;

    /*
    Description:
      Get point on plane that is closest to a given point.
    Parameters:
      point - [in]
    Returns:
      A 3d point on the plane that is closest to world_point.
    */
    CGAL_Point ClosestPointTo(
        CGAL_Point point
    ) const;

    // For intersections see ON_Intersect();

    /*
    Description:
      Transform plane.
    Parameters:
      xform - [in] transformation to apply to plane
    Returns:
      true if successful
    */
    bool Transform(
        const CGAL_Transformation& xform
    );


    /*
    Description:
      Transform a plane by swapping coordinates.
    Parameters:
      i - [in]
      j - [in] indices of coordinates to swap.
          0 = x coordinate, 1 = y coordinate, 2 = z coordinate.
    Returns:
      true if successful.
    */
    bool SwapCoordinates(
        int i,
        int j
    );

    /*
    Description:
      Rotate a plane about its origin.
    Parameters:
      sin_angle - [in] sine of rotation angle
      cos_angle - [in] cosine of rotation angle
      axis - [in] axis of rotation
    Returns:
      true if successful
    */
    bool Rotate(
        double sin_angle,
        double cos_angle,
        const CGAL_Vector& axis
    );

    /*
    Description:
      Rotate a plane about its origin.
    Parameters:
      angle - [in] rotation angle in radians
      axis - [in] axis of rotation
    Returns:
      true if successful
    */
    bool Rotate(
        double angle,
        const CGAL_Vector& axis
    );

    /*
    Description:
      Rotate a plane about a point.
    Parameters:
      sin_angle - [in] sine of rotation angle
      cos_angle - [in] cosine of rotation angle
      axis - [in] axis of rotation
      center - [in] center of rotation
    Returns:
      true if successful
    */
    bool Rotate(
        double sin_angle,
        double cos_angle,
        const CGAL_Vector& axis,
        const CGAL_Point& center
    );

    /*
    Description:
      Rotate a plane about a point.
    Parameters:
      angle - [in] rotation angle in radians
      axis - [in] axis of rotation
      center - [in] center of rotation
    Returns:
      true if successful
    */
    bool Rotate(
        double angle,
        const CGAL_Vector& axis,
        const CGAL_Point& center
    );

    /*
    Description:
      Translate a plane.
    Parameters:
      delta - [in] translation vector
    Returns:
      true if successful
    */
    bool Translate(
        const CGAL_Vector& delta
    );

    /*
    Description:
      Flip plane orientation by swapping x and y axes,
      reversing the zaxis, and updating the equation.
    Returns:
      true if successful
    */
    bool Flip();

    // world coordinate system CGAL_PlaneAxis(CGAL_Point::Origin, CGAL_Vector::XAxis, CGAL_Vector::YAxis); 
    const static CGAL_PlaneAxis World_xy;

    // world coordinate system CGAL_PlaneAxis(CGAL_Point::Origin, CGAL_Vector::YAxis, CGAL_Vector::ZAxis); 
    const static CGAL_PlaneAxis World_yz;

    // world coordinate system CGAL_PlaneAxis(CGAL_Point::Origin, CGAL_Vector::ZAxis, CGAL_Vector::XAxis); 
    const static CGAL_PlaneAxis World_zx;

    // All values are ON_UNSET_VALUE.
    const static
        CGAL_PlaneAxis UnsetPlane;

    // All values are ON_DBL_QNAN.
    const static CGAL_PlaneAxis NanPlane;

public:
    // origin of plane
    CGAL_Point  origin;

    // unit X axis of plane
    CGAL_Vector xaxis;

    // unit Y axis of plane
    CGAL_Vector yaxis;

    // unit Z axis of plane
    CGAL_Vector zaxis;

    // equation of plane
    //CGAL_PlaneAxisEquation plane_equation;
    //double equation[4];
};

#endif