#include <pybind11/pybind11.h>

namespace py = pybind11;



void test(){
    printf("Pybind11 Hello world \n");
}

int add(int i, int j) {
    return i + j;
}



PYBIND11_MODULE(pybind11_test_module, m) {
    m.doc() = "";

    m.def("test", &test);
    
    m.def("add", &add, R"pbdoc(
        Add two numbers
        Some other explanation about the add function.
    )pbdoc");

}