
import os

from compas.geometry.transformations import translation
os.system("cls")

import os
import numpy as np

import compas.geometry as cg
from compas.geometry import Point
from compas.geometry import Vector
from compas.geometry import Plane
from compas.geometry import Line
from compas.geometry import Polyline
from compas.datastructures import Mesh

from compas_view2.app import App
from compas_view2.objects import Collection
import compas_view2.objects as w


import connection_detection


from data.joinery_solver_cross import  *
from data.joinery_solver_sideside import *
from data.JoinerySolverTopSideDataSet import *
from data.joinery_solver_annen import *
from data.joinery_solver_plates import *




def DisplayLines(viewer, lines, scale=0.01):
    linesScaled = []
    for i in lines:
        l = cg.Line(i[0]*scale,i[1]*scale)
        linesScaled.append(l)
    viewer.add(Collection(linesScaled),color = (0, 0, 0), linewidth = 2)

def DisplayPolylinesAsMesh(viewer, polylines, type, scale=0.01):
    outPolylines = [[],[],[],[]]
    outPolylinesColors = [
        (255/255.0,0/255.0,0/255.0),
        (255/255.0,165/255.0,0/255.0),
        (200/255.0,200/255.0,200/255.0),
        (8/255.0,96/255.0,168/255.0)
        ]
    #0 Red Side-Side 1 Yellow Top-Side 2 Grey Top-Top 3 Blue Cross

    for i in range (len(polylines)):
        polyline = polylines[i]
        polyline.transform(cg.transformations.scale.matrix_from_scale_factors([scale,scale,scale]))
        mesh = Mesh.from_polygons([polyline])
        outPolylines[type[i]].append(mesh)


    for i in range (len(outPolylines)):
        #print(len(outPolylines[i]))
        if(len(outPolylines[i])>0):
            viewer.add(Collection(outPolylines[i]),color = outPolylinesColors[i],opacity=0.75)

def DisplayPolylines(viewer, polylines, scale=0.00,r = 0.0, g = 0.0, b = 0.0, t = 1, collection = True):
    polylinesScaled = []
    for i in polylines:
        #print(i)
        if(len(i)>1):
            a = i.transformed(cg.transformations.scale.matrix_from_scale_factors([scale,scale,scale]))
            polylinesScaled.append(a)
            if collection==False:
                if(len(i)==4):
                    viewer.add(a,color=(0,0,255),linewidth = 1)
                elif(len(i)==2):
                    viewer.add(a,color=(0,255,0),linewidth =10)
                else:
                    viewer.add(a,color=(r,g,b), linewidth = t)

    if collection:
        if(len(polylinesScaled)):
            viewer.add(Collection(polylinesScaled),color=(r,g,b), linewidth = t)
        else:
            print("Nothing is displayed")

def DisplayPolyline(viewer, polyline, scale=0.01,r = 0.0, g = 0.0, b = 0.0, t = 1):
    if(len(polyline)>1):
        y = polyline.transformed(cg.transformations.scale.matrix_from_scale_factors([scale,scale,scale]))
        viewer.add(y,color=(r,g,b), linewidth = t)




def     test_connectionDetection():

    # ==============================================================================
    # Get a list of polyline pairs
    # ==============================================================================

    #input = PolylineDataSet.GetAnnenPolylineTwoPlates()


    #for i in PolylineDataSet.GetAnnenPolylineTwoPlates():
    #    input.append(i.transformed(cg.transformations.Translation.from_vector(Vector(100, 0,0))))
    #input = PolylineDataSet.GetAnnenPolyline()

#in-plane wrong - rotate correct
    #input = JoinerySolverSideSideDataSet.SS24()#//0 7 Angles-14 FULL SET 14 // IN-PLANE 6 21, issues 3 - not detected 11,15,16 - strange offset, 21 - side-top not implemented

    #input = JoinerySolverTopSideDataSet.TS8()#3 - errors with multiple connections, and flip for tenons, 7 - does not work
    #input = PolylineDataSet.Test6()
    #input = PolylineDataSet.TestCross14()#9 errors
    #input  = PolylineDataSet.TestChangeBasis()



    # ==============================================================================
    # GetConnectionZones
    # ==============================================================================

    input = annen_small_polylines()
    """
    result = connectionDetection.GetConnectionZones(
        input,
        joinery_solver_annen.annen_small_edge_directions(),
        joinery_solver_annen.annen_small_edge_joints(),
        joinery_solver_annen.annen_small_three_valance_element_indices_and_instruction()
        )
    """
    #input = joinery_solver_sideside.SS26()
    #input = joinery_solver_cross.TestCross9()#8
    
    input = SS0();

    """

    input = joinery_solver_plates.ts_0_polylines()
    result = connectionDetection.GetConnectionZones(
        input,
        joinery_solver_plates.ts_0_edge_directions()
        )

    """
    result = connection_detection.GetConnectionZones(
    input
    )

    # ==============================================================================
    # Process output
    # ==============================================================================
    viewer = App(show_grid=False,width = 3840,height = 2160-250)
    #DisplayPolylines(viewer,input,0.01,0.5, 0.5, 0.5,1,True)
    DisplayPolylines(viewer,result[0],0.01, 1.0, 0.0, 0.0,3,False)

    viewer.run()


print("___________________________")
connection_detection.WrapperTest()
print("___________________________")
test_connectionDetection()
