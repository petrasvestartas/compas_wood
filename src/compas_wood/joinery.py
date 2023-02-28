import wood_pybind11
from wood_pybind11 import WoodVectorDouble
from wood_pybind11 import WoodNestedVectorDouble
from compas.geometry import Point
from compas.geometry import Polyline


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
