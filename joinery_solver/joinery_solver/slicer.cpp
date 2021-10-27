//#include "slicer.h"
//#include <CGAL/Polygon_mesh_slicer.h>
//
//namespace py = pybind11;
//
//
//std::vector<compas::RowMatrixXd>
//pmp_slice_mesh(
//    Eigen::Ref<const compas::RowMatrixXd>& V,
//    Eigen::Ref<const compas::RowMatrixXi>& F,
//    Eigen::Ref<const compas::RowMatrixXd>& P,
//    Eigen::Ref<const compas::RowMatrixXd>& N)
//{
//    CGAL_Mesh mesh = compas::mesh_from_vertices_and_faces(V, F);
//
//    CGAL::Polygon_mesh_slicer<CGAL_Mesh, CGAL_Kernel> slicer(mesh);
//    CGAL_Polylines polylines;
//    std::back_insert_iterator<CGAL_Polylines> slices(polylines);
//
//    int number_of_planes = P.rows();
//
//    for (int i = 0; i < number_of_planes; i++) {
//        CGAL_Kernel::Plane_3 plane = CGAL_Kernel::Plane_3(
//            CGAL_Kernel::Point_3(P(i, 0), P(i, 1), P(i, 2)),
//            CGAL_Kernel::Vector_3(N(i, 0), N(i, 1), N(i, 2)));
//
//        slicer(plane, slices);
//    }
//
//    std::vector<compas::RowMatrixXd> result = compas::result_from_polylines(polylines);
//    return result;
//
//    // // this avoids the segmentation fault
//    // // but returns only one of the results
//
//    // Polyline poly = polylines.front();
//    // int n = poly.size();
//    // compas::RowMatrixXd points(n, 3);
//
//    // for(int j = 0; j < n; j++){
//    //     points(j, 0) = (double) poly[j].x();
//    //     points(j, 1) = (double) poly[j].y();
//    //     points(j, 2) = (double) poly[j].z();
//    // }
//
//    // return points;
//};
//
////PYBIND11_MODULE(pybind11module, module) {
////    module.doc() = "Pybind11Module";//Module name
////    module.def("say_hello", &say_hello);//Function reference
////    module.def("boostTest", &boostTest);//Function reference
////}
//
//void init_slicer(pybind11::module& m) {
//    py::module submodule = m.def_submodule("slicerCGAL");
//
//    submodule.def(
//        "slice_meshCGAL",
//        &pmp_slice_mesh,
//        py::arg("V").noconvert(),
//        py::arg("F").noconvert(),
//        py::arg("P").noconvert(),
//        py::arg("N").noconvert()
//    );
//}