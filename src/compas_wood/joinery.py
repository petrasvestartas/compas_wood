import wood_pybind11
from wood_pybind11 import WoodNestedVectorDouble
from compas.geometry import Polyline

# existing data-sets
filenames_of_datasets = [
    "type_beams_name_phanomema_node",
    "type_plates_name_cross_and_sides_corner",
    "type_plates_name_cross_corners",
    "type_plates_name_cross_ibois_pavilion",
    "type_plates_name_cross_square_reciprocal_iseya",
    "type_plates_name_cross_square_reciprocal_two_sides",
    "type_plates_name_cross_vda_corner",
    "type_plates_name_cross_vda_hexshell",
    "type_plates_name_cross_vda_hexshell_reciprocal",
    "type_plates_name_cross_vda_shell",
    "type_plates_name_cross_vda_single_arch",
    "type_plates_name_hexbox_and_corner",
    "type_plates_name_joint_linking_vidychapel_corner",
    "type_plates_name_joint_linking_vidychapel_full",
    "type_plates_name_joint_linking_vidychapel_one_axis_two_layers",
    "type_plates_name_joint_linking_vidychapel_one_layer",
    "type_plates_name_side_to_side_edge_inplane_2_butterflies",
    "type_plates_name_side_to_side_edge_inplane_differentdirections",
    "type_plates_name_side_to_side_edge_inplane_hexshell",
    "type_plates_name_side_to_side_edge_inplane_outofplane_simple_corners",
    "type_plates_name_side_to_side_edge_inplane_outofplane_simple_corners_combined",
    "type_plates_name_side_to_side_edge_inplane_outofplane_simple_corners_different_lengths",
    "type_plates_name_side_to_side_edge_outofplane_box",
    "type_plates_name_side_to_side_edge_outofplane_dodecahedron",
    "type_plates_name_side_to_side_edge_outofplane_folding",
    "type_plates_name_side_to_side_edge_outofplane_icosahedron",
    "type_plates_name_side_to_side_edge_outofplane_inplane_and_top_to_top_hexboxes",
    "type_plates_name_side_to_side_edge_outofplane_octahedron",
    "type_plates_name_side_to_side_edge_outofplane_tetra",
    "type_plates_name_top_to_side_and_side_to_side_outofplane_annen_box",
    "type_plates_name_top_to_side_and_side_to_side_outofplane_annen_box_pair",
    "type_plates_name_top_to_side_and_side_to_side_outofplane_annen_corner",
    "type_plates_name_top_to_side_and_side_to_side_outofplane_annen_grid_full_arch",
    "type_plates_name_top_to_side_and_side_to_side_outofplane_annen_grid_small",
    "type_plates_name_top_to_side_box",
    "type_plates_name_top_to_side_corners",
    "type_plates_name_top_to_side_pairs",
    "type_plates_name_top_to_top_pairs",
]


def test():
    """Test if Wood library is loaded

    Parameters
    ----------

    Returns
    -------
    Prints
        Hello from CPP Wood
    """

    wood_pybind11.test()


def read_xml(
    foldername, filename_of_dataset="type_plates_name_cross_vda_hexshell_reciprocal"
):
    """read xml data_sets files from foldername and filename_of_dataset
    Parameters
    ----------
    foldername : str
        The name of the folder where all the .xml data-sets are located e.g. "C://IBOIS57//_Code//Software//Python//compas_wood//frontend//src//wood//dataset//".
    filename_of_dataset : str, optional
        The name of the file e.g. "type_plates_name_cross_vda_hexshell_reciprocal" .
    Returns
    -------
    list[Polyline]
        A list of top and bottom polylines
    """

    ###################################################################################################
    # run the WOOD CPP code
    ###################################################################################################
    polylines_coordinates = WoodNestedVectorDouble(
        []
    )  # fills with double values e.g. WoodVectorDouble([0.5, 1.4, 2.3])

    print(
        "\n___________________________________start_of_WOOD_____________________________________\n"
    )
    wood_pybind11.get_data_set(
        foldername,
        filename_of_dataset,
        polylines_coordinates,
    )
    print(
        "\n____________________________________end_of_WOOD______________________________________\n"
    )

    ###################################################################################################
    # convert nested vector of doubles to a list of polylines
    ###################################################################################################

    output_polylines = []

    for i in range(len(polylines_coordinates)):
        points = []
        for j in range(0, len(polylines_coordinates[i]), 3):
            points.append(
                [
                    polylines_coordinates[i][j + 0],
                    polylines_coordinates[i][j + 1],
                    polylines_coordinates[i][j + 2],
                ]
            )
        polyline = Polyline(points)
        output_polylines.append(polyline)

    ###################################################################################################
    # output
    ###################################################################################################
    return output_polylines


def joints():
    """Test if Wood library is loaded

    Parameters
    ----------

    Returns
    -------
    Prints
        Hello from CPP Wood
    """

    wood_pybind11.test()


def rtree():
    """Test if Wood library is loaded

    Parameters
    ----------

    Returns
    -------
    Prints
        Hello from CPP Wood
    """

    wood_pybind11.test()


def get_connection_zones():
    """Test if Wood library is loaded

    Parameters
    ----------

    Returns
    -------
    Prints
        Hello from CPP Wood
    """

    wood_pybind11.test()


def closed_mesh_from_polylines():
    """Test if Wood library is loaded

    Parameters
    ----------

    Returns
    -------
    Prints
        Hello from CPP Wood
    """

    wood_pybind11.test()
