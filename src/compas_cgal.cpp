#include <pybind11/pybind11.h>
#include <compas.h>


// here all modules of "_cgal" are declared.
void init_meshing(pybind11::module&);
void init_booleans(pybind11::module&);
void init_slicer(pybind11::module&);
void init_intersections(pybind11::module&);
void init_measure(pybind11::module&);
void init_triangulations(pybind11::module&);
void init_subdivision(pybind11::module&);


// the first parameter here ("_cgal") will be the name of the "so" or "pyd" file that will be produced by PyBind11
// which is the entry point from where all other modules will be accessible.
PYBIND11_MODULE(_cgal, m) {
    m.doc() = "";

    // register Result as a Python class
    pybind11::class_<compas::Result>(m, "Result")
    	.def_readonly("vertices", &compas::Result::vertices)
    	.def_readonly("faces", &compas::Result::faces);

    // here all modules of "_cgal" are initializied.
    init_meshing(m);
    init_booleans(m);
    init_slicer(m);
    init_intersections(m);
    init_measure(m);
    init_triangulations(m);
    init_subdivision(m);
}
