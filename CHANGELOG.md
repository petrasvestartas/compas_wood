# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## Unreleased

### Added

### Changed

### Removed


## [0.5.0] 2022-10-07

### Added

* Support to python 3.10.
* Added Changelog check in PRs.
* Exposing mesh `compas_cgal.booleans.split` function.

### Changed

* Updated github workflow.

### Removed


## [0.4.0] 2022-01-20

### Added

* Added type annotations.
* Added dimension checks to trimesh setters.
* Added `compas_cgal.measure.volume`.
* Added `compas_cgal.subdivision.catmull_clark`.

### Changed

### Removed

## [0.3.0] 2021-12-14

### Added

* Added `compas_cgal.booleans.boolean_union`.
* Added `compas_cgal.booleans.boolean_difference`.
* Added `compas_cgal.booleans.boolean_intersection`.
* Added `compas_cgal.intersections.intersection_mesh_mesh`.
* Added `compas_cgal.meshing.remesh`.
* Added `compas_cgal.slicer.slice_mesh`.
* Added `compas_cgal.triangulation.delaunay_triangulation`.
* Added `compas_cgal.triangulation.constrained_delaunay_triangulation`.
* Added `compas_cgal.triangulation.conforming_delaunay_triangulation`.

### Changed

### Removed





## :small_red_triangle: Under-construction all the code is being developrd on the "backend" branch :small_red_triangle:

## - to-do list - 

### 01 2023 - finalization of joinery methods (top-to-top, clip joint, multi-butterfly connections)

#### CPP
- [x] separate header only library structure to CPP and H files

#### top-to-top 

- [x] single point drill
- [x] line drill
- [x] circle (polylabel) drill
- [x] smooth drill
- [x] hatch drill
#### multi-butterfly
- [x] replicate the joint n-times
#### clip
- [x] take tenon-mortise and adapt to the clipper geometry


### 02 2023 - finalization of the plate algorithms - MOOC delivery

#### bug fixes
- [x] merge function clean-up
- [x] finger joint parameterization 
- [ ] update the CGAL version for Inscribed Rectangle bug fix made by CGAL guys

#### unit-tests 
- [x] add units tests from MOOC to CPP data-sets: Annen (corner cases, full arch)
- [x] add units tests from MOOC to CPP data-sets: Cross joints (IBOIS pavilion, planarized shell)
- [x] add units tests from MOOC to CPP data-sets: Folded plates arch
- [x] add units tests from MOOC to CPP data-sets: VDA pavilion
- [x] add units tests from MOOC to CPP data-sets: Reciprocal with cross joints
- [x] add units tests from MOOC to CPP data-sets: Reciprocal with Mixed Joint (cross + finger + tenon+mortise)
- [ ] add units tests from MOOC to CPP data-sets: RTree search + OOBB

#### COMPAS integration 
- [x] Create backend in C++ with a Viewer - Finished
- [x] Create a wrapper in C++ with Pybind11 - In Process
- [x] Setup the Compas Python Environment that replicates the 2nd step workflow - Not done
- [ ] Produce documentation - Finished
- [ ] Write Grasshopper components - Not done
- [ ] Write if the code works on MAC and Linux - Not done

### 03 2023 - finalization of beam-panel model integration | C++ unit tests from the collected study cases, wrapping the code to pybind11
- [ ] continue the model based on " beam_node_0.cpp -> beam_node_0 " , currently only connection areas are generated, but not the joint volumes
- [ ] use for tests already implemented mesh boolean difference method
- [ ] separate connection area detection into 3 steps a) cross, b) end-to-end, c) side-to-end
- [ ] display connection area by type color
- [ ] transfer all NGon joinery tests to this one " beam_node_0.cpp " and write Google Tests
- [ ] try to clean up the python code that builds correctly, including the documentaion
- [ ] wrap the two major methods to pybind11

### 04 2023 - documentation, pytest, conda packaging
- [ ] document how to set up all unit tests
- [ ] create pytests that are the same as GoogleTests
- [ ] publish conda package -> meeting with Tom

### 05 2023 - update of the Rhino3D, Grasshopper version, compatibility check with Windows, Mac and Linux
- [ ] develop a Grasshopper plugin that is completely python based
- [ ] check compatibility with MAC
- [ ] check compatibility with Linux


### Switch precompiled headers

´´´cmake
cmake_minimum_required(VERSION 3.14)
project(tslam_reconstruct)
set(CMAKE_CXX_STANDARD 17)
option(ENABLE_O3D_VISUAL_DEBUG  "This option will find locally and link open3d to enable visual debugging"   OFF)
# build shared lib
file(GLOB tslamrec_srcs_base "*.cc")
list(REMOVE_ITEM tslamrec_srcs_base "${CMAKE_CURRENT_SOURCE_DIR}/reconstruct_debug.cc")
add_library(tslamrec SHARED)  # SHARED or STATIC
target_sources(tslamrec PUBLIC ${tslamrec_srcs_base})
if(ENABLE_O3D_VISUAL_DEBUG)
    find_package(Open3D 0.16.1 REQUIRED)
    target_link_libraries(tslamrec PUBLIC Open3D::Open3D)
    target_compile_definitions(tslamrec PUBLIC TSLAM_REC_O3D_VISUAL_DEBUG=0)
endif()
´´´

´´´cpp
#ifdef TSLAM_REC_O3D_VISUAL_DEBUG
    #include<open3d/Open3D.h>
#endif
​
#include <algorithm>
#include <math.h>
// ...
​
int main()
{
		std::cout << "Hello World" << std::endl;
#ifdef TSLAM_REC_O3D_VISUAL_DEBUG
        this->visualize(this->m_ShowVisualizer,
                        this->m_DrawTags,
                        this->m_DrawTagNormals,
                        this->m_DrawAabb,
                        this->m_DrawIntersectedPoly,
                        this->m_DrawSplittingSegments,
                        this->m_DrawSelectedFace,
                        this->m_DrawFinalMesh);
#endif
}
´´´

