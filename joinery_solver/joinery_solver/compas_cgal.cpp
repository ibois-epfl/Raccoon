#include <pybind11/pybind11.h>
#include "compas.h"

namespace py = pybind11;

//void init_meshing(py::module&);
//void init_booleans(py::module&);
//void init_slicer(py::module&);
//void init_intersections(py::module&);
//void init_measure(py::module&);
void init_connectionzones(py::module&);


void say_hello() {
    printf("CPP SHello World \n");
}

//pybind11module, module
PYBIND11_MODULE(pybind11module, m) {

    m.doc() = "";
    //m.doc() = "pybind11module";//Module name
    m.def("say_hello", &say_hello);//Function reference

    py::class_<compas::Result>(m, "Result")
        .def_readonly("vertices", &compas::Result::vertices)
        .def_readonly("faces", &compas::Result::faces);

    //init_meshing(m);
    //init_booleans(m);
    //init_slicer(m);
    //init_intersections(m);
    //init_measure(m);
    init_connectionzones(m);
}