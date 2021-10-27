#pragma once
#include "compas.h"

#include <pybind11/pybind11.h>

namespace py = pybind11;
//
//// construct a CGAL surface mesh
//// from vertices and faces
//// contained in nx3 and fx3 eigen matrices
//// using the Exact Predicates Exact Constructions Kernel
//CGAL_Mesh compas::mesh_from_vertices_and_faces(const compas::RowMatrixXd& V, const compas::RowMatrixXi& F){
//    int v = V.rows();
//    int f = F.rows();
//
//    CGAL_Mesh mesh;
//    std::vector<CGAL_Mesh::Vertex_index> index_descriptor(v);
//
//    for (int i = 0; i < v; i++) {
//        index_descriptor[i] = mesh.add_vertex(CGAL_Kernel::Point_3(V(i, 0), V(i, 1), V(i, 2)));
//    }
//
//    CGAL_Mesh::Vertex_index a;
//    CGAL_Mesh::Vertex_index b;
//    CGAL_Mesh::Vertex_index c;
//
//    for (int i = 0; i < f; i++) {
//        a = index_descriptor[F(i, 0)];
//        b = index_descriptor[F(i, 1)];
//        c = index_descriptor[F(i, 2)];
//        mesh.add_face(a, b, c);
//    }
//
//    return mesh;
//}
//
//std::tuple<compas::RowMatrixXd, compas::RowMatrixXi> compas::mesh_to_vertices_and_faces(const CGAL_Mesh& mesh){
//    int v = mesh.number_of_vertices();
//    int f = mesh.number_of_faces();
//
//    compas::RowMatrixXd V(v, 3);
//    compas::RowMatrixXi F(f, 3);
//
//    CGAL_Mesh::Property_map<CGAL_Mesh::Vertex_index, CGAL_Kernel::Point_3> location = mesh.points();
//
//    for (CGAL_Mesh::Vertex_index vd : mesh.vertices()) {
//        V(vd, 0) = (double)location[vd][0];
//        V(vd, 1) = (double)location[vd][1];
//        V(vd, 2) = (double)location[vd][2];
//    }
//
//    for (CGAL_Mesh::Face_index fd : mesh.faces()) {
//        int i = 0;
//        for (CGAL_Mesh::Vertex_index vd : vertices_around_face(mesh.halfedge(fd), mesh)) {
//            F(fd, i) = (int)vd;
//            i++;
//        }
//    }
//
//    std::tuple<compas::RowMatrixXd, compas::RowMatrixXi> result = std::make_tuple(V, F);
//    return result;
//}
//
//// TODO: rename to ResultMesh
//// construct a result mesh
//// from a CGAL surface mesh
//compas::Result compas::result_from_mesh(const CGAL_Mesh& mesh){
//    int v = mesh.number_of_vertices();
//    int f = mesh.number_of_faces();
//
//    compas::Result R;
//    compas::RowMatrixXd V(v, 3);
//    compas::RowMatrixXi F(f, 3);
//
//    CGAL_Mesh::Property_map<CGAL_Mesh::Vertex_index, CGAL_Kernel::Point_3> location = mesh.points();
//
//    for (CGAL_Mesh::Vertex_index vd : mesh.vertices()) {
//        V(vd, 0) = (double)location[vd][0];
//        V(vd, 1) = (double)location[vd][1];
//        V(vd, 2) = (double)location[vd][2];
//    }
//
//    for (CGAL_Mesh::Face_index fd : mesh.faces()) {
//        int i = 0;
//        for (CGAL_Mesh::Vertex_index vd : vertices_around_face(mesh.halfedge(fd), mesh)) {
//            F(fd, i) = (int)vd;
//            i++;
//        }
//    }
//
//    R.vertices = V;
//    R.faces = F;
//
//    return R;
//}

// construct a set of result polylines
// from CGAL polylines
// the CGAL polylines are a list of vectors of points
std::vector<compas::RowMatrixXd> compas::result_from_polylines (CGAL_Polylines polylines){

    std::vector<compas::RowMatrixXd> pointsets;

    for (auto i = polylines.begin(); i != polylines.end(); i++) {

        const CGAL_Polyline& poly = *i;
        int n = poly.size();
        compas::RowMatrixXd points(n, 3);

        for (int j = 0; j < n; j++) {
            points(j, 0) = (double)poly[j].x();
            points(j, 1) = (double)poly[j].y();
            points(j, 2) = (double)poly[j].z();
        }

        pointsets.push_back(points);
    }

    return pointsets;
}


std::vector<compas::RowMatrixXd> compas::result_from_vector(IK::Vector_3* v)
{
    std::vector<compas::RowMatrixXd> pointsets;

    //for (auto i = 0; i < AABBs.size(); i++) {


        int n = 2;
        compas::RowMatrixXd points(4, 3);

        points(0, 0) = (double)v[0].x();
        points(0, 1) = (double)v[0].y();
        points(0, 2) = (double)v[0].z();
        points(1, 0) = (double)v[1].x();
        points(1, 1) = (double)v[1].y();
        points(1, 2) = (double)v[1].z();
        points(2, 0) = (double)v[2].x();
        points(2, 1) = (double)v[2].y();
        points(2, 2) = (double)v[2].z();
        points(3, 0) = (double)v[3].x();
        points(3, 1) = (double)v[3].y();
        points(3, 2) = (double)v[3].z();

        pointsets.push_back(points);
   // }
    return pointsets;
}


std::vector<compas::RowMatrixXd> compas::result_from_bbox(std::vector<CGAL::Bbox_3> AABBs)
{
    std::vector<compas::RowMatrixXd> pointsets;

    for (auto i = 0; i < AABBs.size(); i++) {


        int n = 2;
        compas::RowMatrixXd points(n, 3);

        points(0, 0) = (double)AABBs[i].xmin();
        points(0, 1) = (double)AABBs[i].ymin();
        points(0, 2) = (double)AABBs[i].zmin();
        points(1, 0) = (double)AABBs[i].xmax();
        points(1, 1) = (double)AABBs[i].ymax();
        points(1, 2) = (double)AABBs[i].zmax();

        pointsets.push_back(points);
    }
    return pointsets;
}

std::vector<CGAL_Polyline> compas::polylines_from_vertices_and_faces(const compas::RowMatrixXd& V, const compas::RowMatrixXi& F) {
  //////////////////////////////////////////////////////////////////////////////
  //Convert Raw data to list of Polyline
  //////////////////////////////////////////////////////////////////////////////
    std::vector<CGAL_Polyline> polylinePairs(F.size());
    CGAL_Polyline pline;

    int counter = 0;
    int lastCount = F(counter, 0);
    for (int i = 0; i < V.size() / 3; i++) {

        CGAL::Epick::Point_3 p(V(i, 0), V(i, 1), V(i, 2));
        pline.push_back(p);

        if (pline.size() == lastCount) {
            polylinePairs[counter] = pline;
            pline.clear();//Clear points from the polyline
            lastCount = F(++counter, 0); //Take next polyline Count
        }

    }
    return polylinePairs;
}


std::vector<compas::RowMatrixXd> compas::result_from_polylinesVectorVector(std::vector < std::vector < CGAL_Polyline>> polylines) {

    std::vector<compas::RowMatrixXd> pointsets;

    for (auto i = polylines.begin(); i != polylines.end(); i++) {

        for (auto j = i->begin(); j != i->end(); j++) {

            const CGAL_Polyline& poly = *j;
            int n = poly.size();
            compas::RowMatrixXd points(n, 3);

            for (int k = 0; k < n; k++) {
                points(k, 0) = (double)poly[k].x();
                points(k, 1) = (double)poly[k].y();
                points(k, 2) = (double)poly[k].z();
            }

            pointsets.push_back(points);
        }
    }

    //printf("CPP number of points: %i ", pointsets.size());

    return pointsets;
}


std::vector<compas::RowMatrixXd> compas::result_from_polylinesVector( std::vector < CGAL_Polyline> polylines) {

    std::vector<compas::RowMatrixXd> pointsets;

    for (auto i = polylines.begin(); i != polylines.end(); i++) {



            const CGAL_Polyline& poly = *i;
            int n = poly.size();
            compas::RowMatrixXd points(n, 3);

            for (int k = 0; k < n; k++) {
                points(k, 0) = (double)poly[k].x();
                points(k, 1) = (double)poly[k].y();
                points(k, 2) = (double)poly[k].z();
            }

            pointsets.push_back(points);
      
    }

  //  printf("CPP number of points: %i ", pointsets.size());

    return pointsets;
}