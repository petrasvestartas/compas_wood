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
- [ ] interface - beam polyline detection
- [ ] documentation 
- [ ] publish documentation gh-pages
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

## Search Methods
### Global
- [x] Closest Object Query + OOB and ABB collision - 
- [ ] Visualize RTree - show geometry with boxes by printing from fill of rtree
- [ ] no search by user given index (must have input in the beginning) 

### Local

- [x] face_to_face side-to-side parallel in-plane | **joinery_library 1-9** | type 12
- [x] face_to_face side-to-side parallel out-of-plane | **joinery_library 10-19** | type 11
- [x] face_to_face top-to-side | **joinery_library 20-29** | type 20
- [x] plane_to_face cross | **joinery_library 30-39** | type 30
- [ ] face_to_face top_to_top | **joinery_library 40-49** |  type 40  currently only output joint_area with 0 local search
- [ ] face_to_face side-to-side | **joinery_library non-parallel 50-59** |  type 50 



### Local Search and Insertion Vector
Description: a vector on an element edge that is equal to a plane normal on an edge. 
- [x] side-top connection insertion vector is currently defined by plane normals, not insertion direction because it is equal what is inside code
![Tenon-mortise Annen](https://github.com/ibois-epfl/Raccoon_Litter_Box/blob/main/joinery_solver/documentation/insertion_vectors_0.jpg) 
![Tenon-mortise Annen2](https://github.com/ibois-epfl/Raccoon_Litter_Box/blob/main/joinery_solver/documentation/insertion_vectors_2.jpg) 

- [ ] side-side connection, not finished for out of plane connections
- [ ] side-side connection in rotaton insertion vector not tested 
- [ ] top-top connection

________________________________________________________

## Element 

### Code implementation
- [x] element is specified as a pair of polylines, with planes for each side
 
### Merge
- [x] insert face joints inside edge
- [x] insert between multiple edges (open polylines + closed polygons) ![Merge closed polygons and open edges](https://github.com/ibois-epfl/Raccoon_Litter_Box/blob/main/joinery_solver/documentation/merge_1.jpg) 
- [ ] cut projection and insert in polygon (case when side-side joints are rotated)
- [x] mesh boolean: a) reference shapes, b) joints polygon pairs

### Element Grouping
- [ ] introduce unordered_map to track grouping like sorted lists x;x;x 
________________________________________________________

## Joint 

### Code implementation
- [x] joint class that stores adjacency information
- [x] change-basis transformation from unit tile to joint volume defined by two rectangles 
- [x] assignment of joints based and search categories and connection types
- [ ] store each parameterized joint inside std::unordered_map<string, joint> that is not remapped yet

- [x] parametric ss_e_op_1 Nejiri Arigata 

![Nejiri Arigata](https://github.com/ibois-epfl/Raccoon_Litter_Box/blob/main/joinery_solver/documentation/joint_documentation_0.jpg) 

- [x] parametric ss_e_op_2 Dovetail

![Dovetail](https://github.com/ibois-epfl/Raccoon_Litter_Box/blob/main/joinery_solver/documentation/joint_documentation_1.jpg) 

- [x] parametric ss_e_ip_0 Dovetail 

![Dovetail](https://github.com/ibois-epfl/Raccoon_Litter_Box/blob/main/joinery_solver/documentation/joint_documentation_2.jpg) 

- [x] parameterize tenon mortise ts_e_p_2 (ts_e_p_0 - original) (start and end point of the joint line are skipped)

![Tenon-mortise](https://github.com/ibois-epfl/Raccoon_Litter_Box/blob/main/joinery_solver/documentation/joint_documentation_3.jpg) 

- [x] parameterize Annen joint - ts_e_p_3 (ts_e_p_2 - original)

![Tenon-mortise Annen](https://github.com/ibois-epfl/Raccoon_Litter_Box/blob/main/joinery_solver/documentation/joint_documentation_4.jpg) 

- [x] parameterize cr_c_ip_0 cross 

![Cross Joint](https://github.com/ibois-epfl/Raccoon_Litter_Box/blob/main/joinery_solver/documentation/joint_documentation_5.jpg) 

- [x] parameterize cr_c_ip_1 conic cross 

![Conic Joint](https://github.com/ibois-epfl/Raccoon_Litter_Box/blob/main/joinery_solver/documentation/joint_documentation_6.jpg) 

- [ ] snap-fit joint 
- [ ] keys
- [ ] tenon-mortise beams
- [ ] tenon-mortise half beam
- [ ] scarf
- [ ] screws

## Joints Parameters

    nothing = '0',

    //plates
    edge_insertion = '1',
    hole = '2',
    insert_between_multiple_edges = '3',

    //beams
    slice = '4',//project and make rectangle
    mill = '5',
    mill_project = '6', //project
    cut = '7',
    cut_project = '8', //project
    binary_slice_mill = '9' //project and make rectangle
    
________________________________________________________

### Tool-path using Joint Parameters
- [x] Flipping case :tomato: 

### BLT
- [ ] Read
- [ ] Write

________________________________________________________

### Data-sets: 
- [x] Small samples 
- [x] Annen 1) Create a C# mesh from nubrs, output a list of V,F 2) C++ create plates, vectors of insertions, joint types
- [ ] CR 
- [ ] VDA 
- [ ] Vidy 
- [ ] Zollinger

