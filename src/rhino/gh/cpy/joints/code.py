
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

proxy = Proxy('compas_wood.joinery') # import module
from compas_rhino import conversions
from ghpythonlib import treehelpers
import Grasshopper as gh
import Rhino

# STEP 3 - start server
#proxy.stop_server()
#proxy.start_server()

class joints(component):
    def RunScript(self, _polylines, _id):
        # ==============================================================================
        # input
        # ==============================================================================
        output_polylines = []
        if( _polylines.AllData().Count!= 0):
            print("user_input")
            #convert to compas polylines
            for i in _polylines.AllData():
                output_polylines.append(conversions.polyline_to_compas(i))
        else:
            print("xml_input")
            feasible_ids = [1,3,7,10,11,12,14,16,17,18,19,20,21,22,23,24,25,27,28,29,30,31,34,36,37]
            
            # read the xml file and get a data-set
            foldername = (
                proxy.get_cwd()+"/frontend/src/wood/dataset/" 
            )
            
            # read polylines from an xml files
            id = feasible_ids[max(min(24, _id), 0)]
            filename_of_dataset = proxy.get_filenames_of_datasets()[id]
            output_polylines = proxy.read_xml_polylines(foldername, filename_of_dataset)
        
        # ==============================================================================
        # get joints
        # ==============================================================================
        element_pairs_list, joint_areas_polylines, joint_types = proxy.joints(output_polylines, 2) 
        
        
        # ==============================================================================
        # output
        # ==============================================================================
        _element_pairs = treehelpers.list_to_tree(element_pairs_list)
        
        # iterate joints add neighbors to individual elements element
        _element_neighbors = gh.DataTree[int]()
        for i in element_pairs_list:
            _element_neighbors.Add(i[1],gh.Kernel.Data.GH_Path(i[0]))
            _element_neighbors.Add(i[0],gh.Kernel.Data.GH_Path(i[1]))
        
        
        _joint_areas = []
        _joint_areas_mesh = []
        for i in joint_areas_polylines:
            polyline = conversions.polyline_to_rhino(i)
            polyline.MergeColinearSegments(0.02, True)
            polyline.CollapseShortSegments(0.02)
            _joint_areas.append(polyline)
            _joint_areas_mesh.append(Rhino.Geometry.Mesh.CreateFromClosedPolyline(polyline))
        
        remap_joint_types = {11: 1, 12: 0, 13: 5, 20: 2, 30: 3, 40: 4}
        _joint_types = []
        for i in joint_types:
            _joint_types.append(remap_joint_types[i])
        
        _polylines_out = []
        for i in output_polylines:
            _polylines_out.append(conversions.polyline_to_rhino(i))
        
        return _polylines_out, _element_pairs, _joint_areas, _joint_areas_mesh, _joint_types, _element_neighbors