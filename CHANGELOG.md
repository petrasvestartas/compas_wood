# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## Unreleased

### Added

### Changed

### Removed


## [1.0.0] 2023-01-01

### Added

- [x] separate header only library structure to CPP and H files
- [x] single point drill
- [x] line drill
- [x] circle (polylabel) drill
- [x] smooth drill
- [x] hatch drill
- [x] multi-butterly replicate the joint n-times
- [x] clip take tenon-mortise and adapt to the clipper geometry

### Changed

### Removed


## [1.0.0] 2023-02-01

### Added

- [x] add unit tests from MOOC to CPP data-sets: Annen (corner cases, full arch)
- [x] add unit tests from MOOC to CPP data-sets: Cross joints (IBOIS pavilion, planarized shell)
- [x] add unit tests from MOOC to CPP data-sets: Folded plates arch
- [x] add unit tests from MOOC to CPP data-sets: VDA pavilion
- [x] add unit tests from MOOC to CPP data-sets: Reciprocal with cross joints
- [x] add unit tests from MOOC to CPP data-sets: Reciprocal with Mixed Joint (cross + finger + tenon+mortise)
- [ ] add unit tests from MOOC to CPP data-sets: RTree search + OOBB

### Changed

- [x] merge function clean-up
- [x] finger joint parameterization 
- [ ] update the CGAL version for Inscribed Rectangle bug fix made by CGAL guys


### Removed

## [1.0.0] 2023-03-01

### Added

- [x] finalization of the plate algorithms - MOOC delivery
- [x] Create backend in C++ with a Viewer - Finished
- [x] Create a wrapper in C++ with Pybind11 - In Process
- [x] Setup the Compas Python Environment that replicates the 2nd step workflow 
- [x] update the code for MAC
- [x] Produce documentation for MOOC section. Since docs building fails try just build one file or compas_wood docs
- [ ] rewrite Grasshopper components in Python
- [x] test on another Windows and mac pc
- [ ] push package to Conda
- [ ] update c++ code for Linux

### Changed

- [x] Backend has a new data-set: wood_test::type_plates_name_vda_floor_0

### Removed

## [1.0.0] 2023-04-01

### Added

- [ ] write a cpp method for a notch generation
- [ ] continue the model based on " beam_node_0.cpp -> beam_node_0 " , currently only connection areas are generated, but not the joint volumes
- [ ] use for tests already implemented mesh boolean difference method
- [ ] separate connection area detection into 3 steps a) cross, b) end-to-end, c) side-to-end
- [ ] display connection area by type color
- [ ] transfer all NGon joinery tests to this one " beam_node_0.cpp " and write Google Tests
- [ ] try to clean up the python code that builds correctly, including the documentation
- [ ] wrap the two major methods to pybind11

### Changed

### Removed

## [1.0.0] 2023-05-01

### Added

- [ ] document how to set up all unit tests
- [ ] create pytests that are the same as GoogleTests

### Changed


# Wish-list
* OpenNest rewriten version for c++
* box packing
* linear stock
* fabrication outlines for plates with notches
* global form methods: mesh pipe, surface mesh offset


### Switch precompiled headers

```cmake
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
```

```cpp
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
```

