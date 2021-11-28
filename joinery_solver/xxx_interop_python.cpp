#include "xxx_interop_python.h"
#include "connection_zones.h"

void init_connectionzones(pybind11::module& m) {
    pybind11::module submodule = m.def_submodule("connectionzonesCGAL");

    submodule.def(
        "get_connection_zones_compas",
        &get_connection_zones_compas,
        pybind11::arg("V").noconvert(),
        pybind11::arg("F").noconvert(),
        pybind11::arg("D").noconvert(),
        pybind11::arg("J").noconvert(),
        pybind11::arg("X").noconvert(),
        pybind11::arg("P").noconvert(),
       // pybind11::arg("OUT1").noconvert(),

        
        pybind11::arg("search_type"),
        pybind11::arg("division_distance"),
        pybind11::arg("shift"),
        pybind11::arg("output_type"),
        pybind11::arg("triangulation")
        
    );
}

void say_hello() {
    printf("CPP SHello World \n");
}


//pybind11module, module
PYBIND11_MODULE(pybind11module, m) {

    m.doc() = "";
    m.def("say_hello", &say_hello);//Function reference

    pybind11::class_<compas::Result>(m, "Result")
        .def_readonly("vertices", &compas::Result::vertices)
        .def_readonly("faces", &compas::Result::faces);

    init_connectionzones(m);
}




