#pragma once
//#include "compas.h"
#include <pybind11/eigen.h>
#include <pybind11/stl.h>
//#include "connection_zones.h"

void init_connectionzones(pybind11::module&);
void test();
