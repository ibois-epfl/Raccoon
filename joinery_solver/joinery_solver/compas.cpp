//#pragma once
//#include "compas.h"
//#include <pybind11/pybind11.h>
//
//namespace py = pybind11;
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
