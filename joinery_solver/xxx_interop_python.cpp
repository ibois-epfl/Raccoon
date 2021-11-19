#include "xxx_interop_python.h"
#include "connection_zones.h"

void init_connectionzones(pybind11::module& m) {
    pybind11::module submodule = m.def_submodule("connectionzonesCGAL");

    submodule.def(
        "get_connection_zones",
        &get_connection_zones,
        pybind11::arg("V").noconvert(),
        pybind11::arg("F").noconvert(),
        pybind11::arg("D").noconvert(),
        pybind11::arg("J").noconvert(),
        pybind11::arg("X").noconvert()
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




