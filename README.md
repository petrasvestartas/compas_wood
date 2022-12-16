# Project for Generating Timber Joints

Author: [Petras Vestartas](https://petrasvestartas.com/About-Petras-Vestartas)

## :small_red_triangle: Under-construction to-do list :small_red_triangle:

### 01 2023 - finalization of joinery methods (top-to-top, clip joint, multi-butterfly connections)

#### top-to-top 
- [x] single point drill
- [ ] line drill
- [x] circle (polylabel) drill
- [ ] smooth drill
- [x] hatch drill
#### clip
- [ ] take tenon-mortise and adapt to the clipper geometry
#### multi-butterfly
- [ ] replicate the joint n-times

### 02 2023 - finalization of beam-panel model integration
- [ ] continue the model based on " beam_node_0.cpp -> beam_node_0 " , currently only connection areas are generated, but not the joint volumes
- [ ] separate connection area detection into 3 steps a) cross, b) end-to-end, c) side-to-end
- [ ] display connection area by type color

### 03 2023 - C++ unit tests from the collected study cases, wrapping the code to pybind11
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
