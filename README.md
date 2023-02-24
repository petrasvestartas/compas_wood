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
![type_plates_name_side_to_side_edge_outofplane_folding](https://user-images.githubusercontent.com/18013985/221279215-b0fbccbf-cbbd-4ac1-b64d-4834b508cd23.png)
![type_plates_name_side_to_side_edge_outofplane_icosahedron](https://user-images.githubusercontent.com/18013985/221279219-17d5acd5-4c7b-4004-bcb0-3e47ec1fd8b9.png)
![type_plates_name_side_to_side_edge_outofplane_inplane_and_top_to_top_hexboxes](https://user-images.githubusercontent.com/18013985/221279223-c6b3268f-fabc-4b92-b154-a19ac4bbc510.png)
![type_plates_name_side_to_side_edge_outofplane_octahedron](https://user-images.githubusercontent.com/18013985/221279226-4f993d74-f95f-442d-9fa9-c7955357ef05.png)
![type_plates_name_side_to_side_edge_outofplane_tetra](https://user-images.githubusercontent.com/18013985/221279227-04d4ae8c-db59-4eb8-849e-7f5537b958f9.png)
![type_plates_name_top_to_side_and_side_to_side_outofplane_annen_box](https://user-images.githubusercontent.com/18013985/221279230-41729108-7b39-4858-844a-26a3b12a7ddc.png)
![type_plates_name_top_to_side_and_side_to_side_outofplane_annen_box_pair](https://user-images.githubusercontent.com/18013985/221279232-6f31fa1f-54f7-4512-9219-7e5c7361fdee.png)
![type_plates_name_top_to_side_and_side_to_side_outofplane_annen_corner](https://user-images.githubusercontent.com/18013985/221279239-8f82187c-882c-4620-b8b4-3584794449bd.png)
![type_plates_name_top_to_side_and_side_to_side_outofplane_annen_grid_full_arch](https://user-images.githubusercontent.com/18013985/221279240-dc2b3f29-b434-44fa-802b-5f82ab958479.png)
![type_plates_name_top_to_side_and_side_to_side_outofplane_annen_grid_small](https://user-images.githubusercontent.com/18013985/221279243-44cdfaea-ad72-40ff-9ae8-d987529e4784.png)
![type_plates_name_top_to_side_box](https://user-images.githubusercontent.com/18013985/221279247-e02e13aa-cd73-4bf2-99ad-324a5109e415.png)
![type_plates_name_top_to_side_corners](https://user-images.githubusercontent.com/18013985/221279248-854aedb5-39cc-45bd-982a-fefaccda8396.png)
![type_plates_name_top_to_side_pairs](https://user-images.githubusercontent.com/18013985/221279251-eddd56be-fd23-4279-82c9-a5a419092fbf.png)
![type_plates_name_top_to_top_pairs](https://user-images.githubusercontent.com/18013985/221279252-2e3daecc-ce99-4bad-b21e-9898ecc64308.png)
![type_plates_name_cross_and_sides_corner](https://user-images.githubusercontent.com/18013985/221279257-6c5fb012-db26-4d10-a9b0-49034769a0a7.png)
![type_plates_name_cross_corners](https://user-images.githubusercontent.com/18013985/221279260-3f8454f6-4f68-4be2-a902-9ff22f6da731.png)
![type_plates_name_cross_ibois_pavilion](https://user-images.githubusercontent.com/18013985/221279264-5808144c-727c-4e97-aa59-e6de5c615f9e.png)
![type_plates_name_cross_square_reciprocal_iseya](https://user-images.githubusercontent.com/18013985/221279269-36184565-995e-4504-b671-89945b17b5d5.png)
![type_plates_name_cross_square_reciprocal_two_sides](https://user-images.githubusercontent.com/18013985/221279270-0714c675-11ca-4b57-be28-921f42f4f87d.png)
![type_plates_name_cross_vda_corner](https://user-images.githubusercontent.com/18013985/221279272-488a6bf1-144c-4b7f-acc6-420fc798cdf4.png)
![type_plates_name_cross_vda_hexshell](https://user-images.githubusercontent.com/18013985/221279273-0cea57f6-0977-4365-a124-8947240aaf49.png)
![type_plates_name_cross_vda_hexshell_reciprocal](https://user-images.githubusercontent.com/18013985/221279274-dca27ff4-9707-466e-8cff-4fc04a499069.png)
![type_plates_name_cross_vda_shell](https://user-images.githubusercontent.com/18013985/221279275-aa9549f5-b1b3-4ad9-a614-74b20e9d9715.png)
![type_plates_name_cross_vda_single_arch](https://user-images.githubusercontent.com/18013985/221279278-23d64692-8415-4dc1-b474-3c838cbec6e0.png)
![type_plates_name_hexbox_and_corner](https://user-images.githubusercontent.com/18013985/221279279-7de2a8e8-8d8a-4aa6-9c41-17916110244d.png)
![type_plates_name_joint_linking_vidychapel_corner](https://user-images.githubusercontent.com/18013985/221279280-1b59f4f6-570e-4e78-b057-de86182916f7.png)
![type_plates_name_joint_linking_vidychapel_full](https://user-images.githubusercontent.com/18013985/221279282-06d93c71-8aa6-4e40-918f-0ae023330373.png)
![type_plates_name_joint_linking_vidychapel_one_axis_two_layers](https://user-images.githubusercontent.com/18013985/221279285-dae6274e-a434-4f5d-ac1d-95eb415a4ca2.png)
![type_plates_name_joint_linking_vidychapel_one_layer](https://user-images.githubusercontent.com/18013985/221279288-ae0031aa-9570-4331-bff3-c5a88d122014.png)
![type_plates_name_side_to_side_edge_inplane_2_butterflies](https://user-images.githubusercontent.com/18013985/221279291-360a2e27-42e5-4367-aa79-14e5ff76e1f6.png)
![type_plates_name_side_to_side_edge_inplane_differentdirections](https://user-images.githubusercontent.com/18013985/221279292-3cd95514-190a-4be0-ba07-f85bdf79e5c5.png)
![type_plates_name_side_to_side_edge_inplane_hexshell](https://user-images.githubusercontent.com/18013985/221279294-450ae18f-3af1-4cd2-a427-49d462d1f36b.png)
![type_plates_name_side_to_side_edge_inplane_outofplane_simple_corners](https://user-images.githubusercontent.com/18013985/221279296-790982bf-0482-4906-81a4-91ae2d6a41fb.png)
![type_plates_name_side_to_side_edge_inplane_outofplane_simple_corners_combined](https://user-images.githubusercontent.com/18013985/221279298-9293da80-f395-4979-bac6-cef8df44b550.png)
![type_plates_name_side_to_side_edge_inplane_outofplane_simple_corners_different_lengths](https://user-images.githubusercontent.com/18013985/221279301-e3441619-7e1b-43be-9ac9-5b8ec0db2341.png)
![type_plates_name_side_to_side_edge_outofplane_box](https://user-images.githubusercontent.com/18013985/221279303-ba100c92-5541-4f8d-94bb-4f80b4d90175.png)
![type_plates_name_side_to_side_edge_outofplane_dodecahedron](https://user-images.githubusercontent.com/18013985/221279305-5e4af770-3cd3-4233-9478-c2022fd78765.png)


