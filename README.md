# Project for Generating Timber Joints

Author: [Petras Vestartas](https://petrasvestartas.com/About-Petras-Vestartas)

![type_plates_name_side_to_side_edge_inplane_hexshell](https://user-images.githubusercontent.com/18013985/221278631-8ed8f5af-8485-49ec-9a99-625d5a4466e4.png)

![type_plates_name_top_to_side_and_side_to_side_outofplane_annen_grid_full_arch](https://user-images.githubusercontent.com/18013985/221278750-e42b5596-8d6a-4a48-ab80-b825dde446af.png)


## :small_red_triangle: Under-construction to-do list :small_red_triangle:

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
- [ ] Create a wrapper in C++ with Pybind11 - In Process
- [ ] Setup the Compas Python Environment that replicates the 2nd step workflow - Not done
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



##### References
https://grail.cs.washington.edu/projects/carpentrycompiler/
@article {wu_siga19,
    author = {Chenming Wu and Haisen Zhao and Chandrakana Nandi and Jeffrey I. Lipton and Zachary Tatlock and Adriana Schulz},
    title = {Carpentry Compiler},
    journal = {ACM Transactions on Graphics},
    note = {presented at SIGGRAPH Asia 2019},
    volume = {38},
    number = {6},
    pages = {Article No. 195},
    year = {2019}
}
