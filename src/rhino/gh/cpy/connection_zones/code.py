"""get joints ids and connections areas between elements"""

__author__ = "petras vestartas"
__version__ = "2023.03.29"


from ghpythonlib.componentbase import executingcomponent as component

# STEP 0 - Installation, anaconda prompt:
# python -m compas_rhino.install
# python -m compas_rhino.install -p compas_wood
# STEP 1 - check the environment name
# import compas_bootstrapper
# print(compas_bootstrapper.ENVIRONMENT_NAME)
# STEP 2 - import proxy from compas.rpc
# https://compas.dev/compas/latest/tutorial/rpc.html#supported-data-types-1
from compas.rpc import Proxy  # https://compas.dev/compas/latest/api/compas.rpc.html

proxy = Proxy("compas_wood.joinery")  # import module
from compas_rhino import conversions
from ghpythonlib import treehelpers
import Rhino
import Grasshopper as gh

# STEP 3 - start server
# proxy.stop_server()
# proxy.start_server()


class connections_zones(component):
    def RunScript(
        self,
        _polylines,
        _id,
        _vectors,
        _joint_types,
        _three_valence,
        _adjacency,
        _params,
        _scale,
        _output,
        _search,
        _extension,
    ):
        # ==============================================================================
        # input
        # ==============================================================================
        output_polylines = []
        if _polylines.AllData().Count != 0:
            print("user_input")
            # convert to compas polylines
            for i in _polylines.AllData():
                output_polylines.append(conversions.polyline_to_compas(i))
        else:
            print("xml_input")
            feasible_ids = [
                1,
                3,
                7,
                10,
                11,
                12,
                14,
                16,
                17,
                18,
                19,
                20,
                21,
                22,
                23,
                24,
                25,
                27,
                28,
                29,
                30,
                31,
                34,
                36,
                37,
            ]

            # read the xml file and get a data-set
            foldername = proxy.get_cwd() + "/frontend/src/wood/dataset/"

            # read polylines from an xml files
            id = feasible_ids[max(min(24, _id), 0)]
            filename_of_dataset = proxy.get_filenames_of_datasets()[id]
            output_polylines = proxy.read_xml_polylines(foldername, filename_of_dataset)

        vectors = []
        if _vectors != None:
            for i in range(_vectors.BranchCount):
                list = []
                for j in _vectors.Branch(i):
                    list.append(conversions.vector_to_compas(j))
                vectors.append(list)

        joint_parameters = [
            300,
            0.5,
            3,  # 1-9 ss_e_ip (side-to-side edge in-plane)
            50,
            0.64,
            15,  # 10-19 ss_e_op (side-to-side edge out-of-plane)
            450,
            0.5,
            20,  # 20-29 ts_e_p (top-to-side edge plane)
            300,
            0.5,
            30,  # 30-39 cr_c_ip (cross cutting in-plane)
            6,
            0.95,
            40,  # 40-49 tt_e_p  (top-to-top edge plane)
            300,
            0.5,
            58,  # 50-59 ss_e_r (side-to-side edge rotated)
            300,
            1.0,
            60,  # 60-69 b (boundary)
        ]

        for i in range(0, len(_params)):
            joint_parameters[i] = _params[i]
        print(joint_parameters)
        search = id = max(min(2, _search), 0)

        scale = [1, 1, 1]
        for i in range(0, len(_scale)):
            scale[i] = _scale[i]

        output_type = _output if _output != None else 4

        extension = [0, 0, 0]
        for i in range(0, len(_extension)):
            extension[i] = _extension[i]

        joint_types = []
        if _joint_types.DataCount != 0:
            joint_types = treehelpers.tree_to_list(_joint_types)

        three_valence = []
        if _three_valence.DataCount != 0:
            three_valence = treehelpers.tree_to_list(_three_valence)

        # ==============================================================================
        # get joints
        # ==============================================================================
        result = proxy.get_connection_zones(
            output_polylines,
            vectors,  # output_vectors,
            joint_types,  # output_joints_types,
            three_valence,  # output_three_valence_element_indices_and_instruction,
            _adjacency,  # output_adjacency,
            joint_parameters,
            search,
            scale,
            output_type,
            False,
            extension,
        )

        # ==============================================================================
        # output
        # ==============================================================================
        _polylines_out = gh.DataTree[Rhino.Geometry.Polyline]()

        # nest two times the result and add to the tree
        for i in range(0, len(result)):
            for j in range(0, len(result[i])):
                _polylines_out.Add(
                    conversions.polyline_to_rhino(result[i][j]),
                    gh.Kernel.Data.GH_Path(i),
                )

        _cut_type = []

        return _polylines_out, _cut_type
