# COMPAS_WOOD - Project for Generating Timber Joints

Author: [Petras Vestartas](https://petrasvestartas.com/Petras-Vestartas-CV)

![alt text](docs/_images/github_repo_structure/compas_wood_github_repository_structure.png "github_repository_structure")
![type_plates_name_side_to_side_edge_inplane_hexshell](https://user-images.githubusercontent.com/18013985/221278631-8ed8f5af-8485-49ec-9a99-625d5a4466e4.png)


## Interface compas_wood

### Installation

##### Windows

    conda create -n wood-dev python=3.8 mpir mpfr boost-cpp eigen=3.4 cgal-cpp=5.5 pybind11 compas compas_view2 --yes
    conda activate wood-dev
    git clone https://github.com/petrasvestartas/compas_wood
    cd compas_wood
    pip install -e .

##### Mac


    conda create -n wood-dev python=3.8 gmp mpfr boost-cpp eigen=3.4 cgal-cpp=5.5 pybind11 compas compas_view2 --yes
    conda activate wood-dev
    git clone https:/github.com/petrasvestartas/compas_wood
    cd compas_wood 
    pip install -e . 

* Note: Be aware that "pip install -e ." command will take some time to run without any messages. This is normal for C++ compilation that is done only once.


##### If you install this library on a new computer that did not have Anaconda Prompt or VScode follow these steps

* Install Anaconda Prompt: https://www.anaconda.com/
* Type in the anaconda prompt: "conda config --add channels conda-forge"
* Add this environment path. On windows search type "Edit the system environmental variables", click "Environmental Variables..", under second tab click "path", then edit and add the following path: "C:\ProgramData\Anaconda3\Scripts". This process can be avoided if during installation of Anaconda Prompt you ticked the option to the Anaconda Prompt to the Environmental Variables.
* Open Anaconda Prompt terminal and update conda:
    conda activate base
    conda update --all

##### If you never used VSCode

* Install VSCode: https://code.visualstudio.com/Download
* Install VSCode Extensions: Python
* Launch VScode, type CTRL+SHIFT+P and >Python: Create Terminal


##### Video
https://user-images.githubusercontent.com/18013985/147587822-cfb43c96-4a81-4b89-987f-8d201ec2e5ba.mp4

##### ToDo
- [ ] recenter geometry before processing
- [ ] ctypes floating number conversion, currently precision is lost
- [ ] ctypes user tolerance

## Examples
- [x] "tests/compas_wood_1_test.py" - test if the library is loading
- [x] "tests/compas_wood_2_read_xml.py" - get data-set a set of polylines and display in viewer
- [x] "tests/compas_wood_3_joints.py" - get connection-areas between timber plates
- [x] "tests/compas_wood_4_rtree.py" - get neighbors of an element
- [x] "tests/compas_wood_5_get_connection_zones.py" - generate timber joints
- [x] "tests/compas_wood_6_closed_mesh_from_polylines.py" - create a solid mesh from polylines with holes
- [x] "tests/compas_wood_7_mooc_homework.py" - create a special connection of the Vidy project


## Tests
![type_plates_name_cross_and_sides_corner](https://user-images.githubusercontent.com/18013985/221279761-5753e90d-b73a-4e63-824a-da8b5d57d3c2.png)
![type_plates_name_cross_corners](https://user-images.githubusercontent.com/18013985/221279772-0d037f1a-1c43-4c11-b42d-77558b9bc2d1.png)
![type_plates_name_cross_ibois_pavilion](https://user-images.githubusercontent.com/18013985/221279812-6262616b-b9d7-4e49-a132-1463da5d521d.png)
![type_plates_name_cross_square_reciprocal_iseya](https://user-images.githubusercontent.com/18013985/221279849-06216149-af3d-43cd-9c38-556787568f95.png)
![type_plates_name_cross_square_reciprocal_two_sides](https://user-images.githubusercontent.com/18013985/221279870-a92d24a5-77d6-4bc4-b673-576d2080dbba.png)
![type_plates_name_cross_vda_corner](https://user-images.githubusercontent.com/18013985/221279885-da130a52-069c-48fd-bad7-4d77aea3ab8f.png)
![type_plates_name_cross_vda_hexshell](https://user-images.githubusercontent.com/18013985/221279901-1f5db62d-e706-44bd-aa7d-89585941ab4d.png)
![type_plates_name_cross_vda_hexshell_reciprocal](https://user-images.githubusercontent.com/18013985/221279915-c2e475a9-985b-4de4-ab3a-86cebe5d2135.png)
![type_plates_name_cross_vda_shell](https://user-images.githubusercontent.com/18013985/221279929-1371e713-7550-4cba-9c98-b3a89bdd3b9e.png)
![type_plates_name_cross_vda_single_arch](https://user-images.githubusercontent.com/18013985/221279946-74486522-8b29-43ff-b37d-f15ac232def7.png)
![type_plates_name_hexbox_and_corner](https://user-images.githubusercontent.com/18013985/221279958-cc2f48c3-a11e-4594-bd44-759fe6cd28dd.png)
![type_plates_name_joint_linking_vidychapel_corner](https://user-images.githubusercontent.com/18013985/221279972-428f1c5c-c89a-4f80-a727-1ea72eafcfde.png)
![type_plates_name_joint_linking_vidychapel_full](https://user-images.githubusercontent.com/18013985/221279981-6566f387-c2e5-4e0d-ad90-761fee049d03.png)
![type_plates_name_top_to_side_box](https://user-images.githubusercontent.com/18013985/221280094-d764672b-e5b8-4f94-a2c7-81e2533faee9.png)
![type_plates_name_top_to_side_corners](https://user-images.githubusercontent.com/18013985/221280099-145ded55-4534-408f-a5c1-61862af68e12.png)
![type_plates_name_top_to_side_pairs](https://user-images.githubusercontent.com/18013985/221280101-d70d9f67-0835-4781-bf7d-558292fa3213.png)
![type_plates_name_top_to_top_pairs](https://user-images.githubusercontent.com/18013985/221280103-38caa7c4-7eb8-4390-bcda-5464498ec8de.png)
![type_plates_name_joint_linking_vidychapel_one_axis_two_layers](https://user-images.githubusercontent.com/18013985/221280106-73f6fe45-b7c6-4bf3-a3a9-9ec2194d462b.png)
![type_plates_name_joint_linking_vidychapel_one_layer](https://user-images.githubusercontent.com/18013985/221280107-d63cee86-ea27-4110-a0b7-0f4334725ff4.png)
![type_plates_name_side_to_side_edge_inplane_2_butterflies](https://user-images.githubusercontent.com/18013985/221280109-a93eabda-7043-40d0-ac95-e41114d45926.png)
![type_plates_name_side_to_side_edge_inplane_differentdirections](https://user-images.githubusercontent.com/18013985/221280110-ab04dba6-d0c3-43a3-b9c5-4e11735be8c0.png)
![type_plates_name_side_to_side_edge_inplane_hexshell](https://user-images.githubusercontent.com/18013985/221280112-957672d5-61a1-4b8b-baa5-b1742e6376cc.png)
![type_plates_name_side_to_side_edge_inplane_outofplane_simple_corners](https://user-images.githubusercontent.com/18013985/221280113-ad967c7b-69bc-4d31-adee-a4cb222ab54d.png)
![type_plates_name_side_to_side_edge_inplane_outofplane_simple_corners_combined](https://user-images.githubusercontent.com/18013985/221280116-04b9cc7a-71fe-41a3-98e0-72c0f86a1023.png)
![type_plates_name_side_to_side_edge_inplane_outofplane_simple_corners_different_lengths](https://user-images.githubusercontent.com/18013985/221280117-f1a3978e-5279-4b4e-b8fe-71eda7bd365d.png)
![type_plates_name_side_to_side_edge_outofplane_box](https://user-images.githubusercontent.com/18013985/221280119-81da8530-d4a3-4ad3-8827-2a9d37dabec0.png)
![type_plates_name_side_to_side_edge_outofplane_dodecahedron](https://user-images.githubusercontent.com/18013985/221280120-ae8f7c99-d337-4086-9a18-1cc1e39206b7.png)
![type_plates_name_side_to_side_edge_outofplane_folding](https://user-images.githubusercontent.com/18013985/221280121-65b795b5-4121-4992-aa6c-ea9698188f72.png)
![type_plates_name_side_to_side_edge_outofplane_icosahedron](https://user-images.githubusercontent.com/18013985/221280124-f732d47b-3cc3-455c-befe-86229d6b9bfb.png)
![type_plates_name_side_to_side_edge_outofplane_inplane_and_top_to_top_hexboxes](https://user-images.githubusercontent.com/18013985/221280127-c15abe51-6ea8-473c-924e-5d4aa57dd2f5.png)
![type_plates_name_side_to_side_edge_outofplane_octahedron](https://user-images.githubusercontent.com/18013985/221280130-0e424630-9fe8-4de6-a5aa-9ae2cb417183.png)
![type_plates_name_side_to_side_edge_outofplane_tetra](https://user-images.githubusercontent.com/18013985/221280131-0dc89330-acfb-4b59-b704-9dff53754268.png)
![type_plates_name_top_to_side_and_side_to_side_outofplane_annen_box](https://user-images.githubusercontent.com/18013985/221280134-209bedf8-8a87-4c85-b72c-2da1f5f5bf08.png)
![type_plates_name_top_to_side_and_side_to_side_outofplane_annen_box_pair](https://user-images.githubusercontent.com/18013985/221280135-2b560e71-d89c-49ad-89a8-511a837532c4.png)
![type_plates_name_top_to_side_and_side_to_side_outofplane_annen_corner](https://user-images.githubusercontent.com/18013985/221280139-fcd8499d-c04a-4522-9b1a-260fe513c0a0.png)
![type_plates_name_top_to_side_and_side_to_side_outofplane_annen_grid_full_arch](https://user-images.githubusercontent.com/18013985/221280141-fc286217-8aa4-44f5-af91-d40a30e35941.png)
![type_plates_name_top_to_side_and_side_to_side_outofplane_annen_grid_small](https://user-images.githubusercontent.com/18013985/221280143-af0952b8-24df-48b1-b242-e56a5bea5ecf.png)


## Rhino Implementation

![Test](https://github.com/petrasvestartas/compas_wood/assets/18013985/a4f9ee09-acdf-4f9d-ad68-daf1c988ee53)
![Roof3](https://github.com/petrasvestartas/compas_wood/assets/18013985/4afc55b4-7d17-46ef-ba03-a3f1ade80fa3)

![Folding](https://github.com/petrasvestartas/compas_wood/assets/18013985/aa9c8959-5a3d-472c-b96a-0f21dae89284)

![slab_0](https://github.com/petrasvestartas/compas_wood/assets/18013985/cfbc30b6-d587-4fad-aeea-c1e7f6b0fca0)

![slab_1](https://github.com/petrasvestartas/compas_wood/assets/18013985/54998c08-7b7b-4380-8bc0-2e3bc185717b)
![slab_5](https://github.com/petrasvestartas/compas_wood/assets/18013985/e1ea09f6-6f16-499a-ad1f-5c74b1d59959)

![slab_8](https://github.com/petrasvestartas/compas_wood/assets/18013985/8614b367-2f61-41a1-85e8-b121a67b9fc1)
![slab_7](https://github.com/petrasvestartas/compas_wood/assets/18013985/14aad553-2cac-44ff-9289-ac4485bb5071)
![slab_9](https://github.com/petrasvestartas/compas_wood/assets/18013985/7a2c0727-7394-4930-b61f-135ba0d9fa6e)
![slab_22](https://github.com/petrasvestartas/compas_wood/assets/18013985/d7998ed3-4078-4ebd-b87d-14ffc0e86b04)












