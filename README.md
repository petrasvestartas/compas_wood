# Project for Generating Timber Joints
Documentation: https://ibois-epfl.github.io/compas_wood/latest/

Author: [Petras Vestartas](https://petrasvestartas.com/About-Petras-Vestartas)


## Interface compas_wood

### Installation

#### Clone Github Repository

##### Windows

    conda create -n wood-dev python=3.8 mpir mpfr boost-cpp eigen=3.3 cgal-cpp=5.2 pybind11 compas compas_view2 --yes
    conda activate wood-dev
    git clone https://github.com/petrasvestartas/compas_wood
    cd compas_wood
    pip install -e .

##### Mac


    conda create -n wood-dev python=3.8 gmp mpfr boost-cpp eigen=3.3 cgal-cpp=5.2 pybind11 compas compas_view2 --yes
    conda activate wood-dev
    git clone https:/github.com/petrasvestartas/compas_wood
    cd compas_wood 
    pip install -e . 

##### Video
https://user-images.githubusercontent.com/18013985/147587822-cfb43c96-4a81-4b89-987f-8d201ec2e5ba.mp4

### Notes

* Be sure that you activate ```wood-dev``` after installation and you are in ```compas_wood``` folder before writing ```pip install -e .```
* For more detailed instructions please follow  `ReadMe <https://github.com/petrasvestartas/compas_wood>`_.

- [x] pybind11 - get_connection_zones
- [x] pybind11 - closed_mesh_from_polylines
- [x] pybind11 - rtree
- [x] pybind11 - joints (joint_area output)
- [ ] pybind11 - preview hard-coded joint library
- [ ] pybind11 - mesh difference give a set of meshes
- [ ] interface - groups
- [ ] interface - R-Tree for joint types
- [ ] interface - edge vector display
- [ ] interface - beam polyline detection, 1) check what that not clean if statement does, do you need add joints to elements, if there are no elements just central line? 2) try generate tenon-mortise joint 3) cut using mesh boolean
- [x] publish documentation gh-pages
- [ ] publish conda


## Interface Rhino C++ Plugin

### Build from scratch, linking:

- [ ] transfer plugin code from raccoonlitterbox

- Rhino requires ```stdafx.h``` linked in all .cpp files. Therefore in the source file there is a stdafx.h to compile for both cases. If you are using Rhino skip the the stafx.h, if not just add this empty file.
- In Rhino ```stdafx.h``` , write to run CGAL ```#define NOMINMAX #undef min #undef max```
-  Add all .cpp files such as ```clipper.cpp``` to the new project, since they cannot be linked like header files.
-  CGAL Builds with C++14, not C++17 
- (Headers) C/C++ -> General -> Additional Include Directions :
```

$(RhinoIncDir); \
C:\Users\petra\AppData\Local\Programs\Python\Python38\include;
C:\IBOIS57\_Code\Software\CPP\pybind11-2.9.0\include;
C:\IBOIS57\_Code\Software\CPP\CGAL\CGAL-5.3\include;
C:\IBOIS57\_Code\Software\CPP\CGAL\CGAL-5.3\auxiliary\gmp\include;
C:\IBOIS57\_Code\Software\CPP\Eigen\eigen-3.3.9;
C:\IBOIS57\_Code\Software\CPP\Boost\;

C:\IBOIS57\_Code\Software\Raccoon_Litter_Box\joinery_solver; \
C:\IBOIS57\_Code\Software\Raccoon_Litter_Box\joinery_solver\joinery_solver_rhino7; \
%(AdditionalIncludeDirectories)
```

![image](https://user-images.githubusercontent.com/18013985/148752197-b05ebef9-709d-4fbb-ac55-9e905aaaddcf.png)



- (Libraries) Linker -> Input -> Additional Dependencies :
```
C:\Users\petra\AppData\Local\Programs\Python\Python38\libs\python38.lib; \
C:\IBOIS57\_Code\Software\CPP\CGAL\CGAL-5.3\auxiliary\gmp\lib\libgmp-10.lib; \
C:\IBOIS57\_Code\Software\CPP\CGAL\CGAL-5.3\auxiliary\gmp\lib\libmpfr-4.lib; 
```
![image](https://user-images.githubusercontent.com/18013985/148752247-8d0cacbe-55de-461c-9a4c-429974fb151d.png)


 
### Command-line interface:
- [x] Menu 1 : Select Polylines pairs
- [x] Menu 2 : Select Insertion vectors
- [x] Menu 3 : Skip not needed joints, by dots 
- [x] Menu 4 : search type | division distance | shift | 
- [x] Menu 5 : Joint parameters per category 
- [x] Separate command: Loft polyline groups with holes, 2d cgal constrained delaunay triangulations
- [ ] MAC - only possible through NET or CPython wrappers

 [Video](https://vimeo.com/650968401 "Crash test")


![rhino_command_line](https://github.com/ibois-epfl/Raccoon_Litter_Box/blob/main/joinery_solver/documentation/rhino_interface_1.gif)



## Interface Rhino, Grasshopper
- [ ] Python or C# wrappers 
- [ ] Grasshopper components 

## Interface Stand-alone
- [ ] C++ Visualizer
________________________________________________________


### Data-sets: 
- [x] Small samples 
- [x] Annen 1) Create a C# mesh from nubrs, output a list of V,F 2) C++ create plates, vectors of insertions, joint types
- [ ] CR 
- [ ] VDA 
- [ ] Vidy 
- [ ] Zollinger

