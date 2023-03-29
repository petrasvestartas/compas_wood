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
from compas.rpc import Proxy # https://compas.dev/compas/latest/api/compas.rpc.html
proxy = Proxy('compas_wood.joinery') # import module
from compas_rhino import conversions
from ghpythonlib import treehelpers
import Rhino
import Grasshopper as gh
from compas.datastructures import Mesh

# STEP 3 - start server
#proxy.stop_server()
#proxy.start_server()


class connections_zones(component):
    def RunScript(self, _polylines):
        # ==============================================================================
        # input
        # ==============================================================================
        output_polylines = []
        if( len(_polylines)!= 0):
            #convert to compas polylines
            for i in _polylines:
                output_polylines.append(conversions.polyline_to_compas(i))
        else:
            return
           
        
        # ==============================================================================
        # get joints
        # ==============================================================================
        result = proxy.closed_mesh_from_polylines(output_polylines)
        
        # ==============================================================================
        # output
        # ==============================================================================
        _mesh = Rhino.Geometry.Mesh()
        
        
        vertices = list(result.vertices())
        key_index = result.key_index()
        faces = [[key_index[key] for key in result.face_vertices(face)] for face in result.faces()]
        
        
        for v in vertices:
            xyz = result.vertex_attributes(v, 'xyz')
            _mesh.Vertices.Add(Rhino.Geometry.Point3f(xyz[0],xyz[1],xyz[2]))
        for f in faces:
            _mesh.Faces.AddFace(f[0],f[1],f[2])
        
        return _mesh