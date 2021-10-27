#pragma once
#ifndef COMPAS_H
#define COMPAS_H

#include <pybind11/eigen.h>
#include <pybind11/stl.h>

#include <Eigen/StdVector>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Intersections.h>
//#include <CGAL/Surface_mesh.h>

//#include <CGAL/Simple_cartesian.h>
#include <CGAL/Bbox_2.h>
#include <CGAL/Bbox_3.h>
#include <CGAL/Plane_3.h>
#include "clipper.h"

static double GlobalTolerance = 0.01;
static double GlobalToleranceSquare = 0.0001;
static double GlobalClipperScale = 1000000.0;
static double GlobalClipperAreaTolerance = 0.0001;
static double GlobalExtend[5] = {0,0,0,0,0};

#define ON_IS_FINITE(x) (0x7FF0 != (*((unsigned short*)(&x) + 3) & 0x7FF0))
#define ON_DBL_MIN 2.22507385850720200e-308
#define ON_EPSILON 2.2204460492503131e-16
#define ON_SQRT_EPSILON 1.490116119385000000e-8
#define ON_ZERO_TOLERANCE 2.3283064365386962890625e-10
#define ON_DBL_MAX 1.7976931348623158e+308


using IK = CGAL::Exact_predicates_inexact_constructions_kernel;
//using EK = CGAL::Exact_predicates_exact_constructions_kernel;
//typedef CGAL::Simple_cartesian<double>                           IK;

//using To_exact = CGAL::Cartesian_converter<IK, EK>;
//using Back_from_exact = CGAL::Cartesian_converter<EK, IK>;
//using EK_Plane = EK::Plane_3;
//using EK_Point = IK::Point_3;

//using CGAL_Kernel = CGAL::Exact_predicates_exact_constructions_kernel;
//using IK::Direction_3 = IK::Direction_3;
//using IK::Vector_3 = IK::Vector_3;
//using IK::Vector_2 = IK::Vector_2;
//using IK::Plane_3 = IK::Plane_3;
//using CGAL::Bbox_3 = CGAL::Bbox_3;
//using CGAL_Mesh = CGAL::Surface_mesh<CGAL_Kernel::Point_3>;
using CGAL_Polyline = std::vector<IK::Point_3>;
using CGAL_Polylines = std::list<CGAL_Polyline>;
//using CGAL::Aff_transformation_3<IK> = CGAL::Aff_transformation_3<IK>;
//using IK::Line_3 = IK::Line_3;
//using IK::Segment_3 = IK::Segment_3;

////////////////////////////For Intersections/////////////////////////////////

//#include <CGAL/Simple_cartesian.h>
//#include <CGAL/Quotient.h>
//#include <CGAL/MP_Float.h>
//#include <CGAL/Cartesian_converter.h>
//typedef CGAL::Simple_cartesian<CGAL::Quotient<CGAL::MP_Float> >  EK;
////using EK = CGAL::Exact_predicates_exact_constructions_kernel;
//typedef CGAL::Cartesian_converter<IK, EK>                         IK_to_EK;
//typedef CGAL::Cartesian_converter<EK, IK>                         EK_to_IK;

namespace compas
{
    
    using RowMatrixXd = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
    using RowMatrixXi = Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

    struct Result {
        RowMatrixXd vertices;
        RowMatrixXi faces;
    };

    //CGAL_Mesh mesh_from_vertices_and_faces(const RowMatrixXd& V, const RowMatrixXi& F);

    //std::tuple<compas::RowMatrixXd, compas::RowMatrixXi> mesh_to_vertices_and_faces(const CGAL_Mesh& mesh);

    std::vector<compas::RowMatrixXd> result_from_polylines (CGAL_Polylines polylines);

    //Result result_from_mesh(const CGAL_Mesh& mesh);

    

    //New Methods
    std::vector<compas::RowMatrixXd> result_from_vector(IK::Vector_3* v);

    std::vector<compas::RowMatrixXd> result_from_bbox(std::vector<CGAL::Bbox_3> boxes);

    std::vector<CGAL_Polyline> polylines_from_vertices_and_faces(const RowMatrixXd& V, const RowMatrixXi& F);

    std::vector<compas::RowMatrixXd> result_from_polylinesVectorVector(std::vector < std::vector < CGAL_Polyline>> polylines);

    std::vector<compas::RowMatrixXd> result_from_polylinesVector(std::vector < CGAL_Polyline> polylines);
}

#endif /* COMPAS_H */