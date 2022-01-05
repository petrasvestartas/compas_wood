#compas
import compas.geometry as cg
from compas.datastructures import Mesh

#viewer
from compas_view2.app import App
from compas_view2.objects import Collection
import compas_view2.objects as w
from compas_view2.views import view

#python
from random import random, randrange

def display(input=None, result=None, meshes = None, scale = 0.01, movex_0 = 1.25, movex_1 = 0, movex_2 = 0):
    viewer = App(show_grid=False, width = 3840,height = 2160-250, enable_sidebar=True,version= '120')  
    

    #preview
    if(input != None):
        display_polylines(viewer,input,scale,1.0, 0.0, 0.0,3,True,movex_0) #polylines without joints
    
    if(result != None):
        for i in range(len(result)):
            if(len(result)>0):
                display_polylines(viewer,result[i],scale,206/255,0,88/255.0, 3,False,movex_1) #polylines with joints
                #display_polylines(viewer,result[i],scale,randrange(0,206)/255.0,0,randrange(0,88)/255.0, 3,False,movex_1) #polylines with joints

    if(meshes != None):
        for i in range(len(meshes)):
            display_mesh(viewer,meshes[i], scale,0.9,0.9,0.9,3)
    
    viewer.run()
  






def display_lines(viewer, lines, scale=0.01):
    linesScaled = []
    for i in lines:
        l = cg.Line(i[0]*scale,i[1]*scale)
        linesScaled.append(l)
    viewer.add(Collection(linesScaled),color = (0, 0, 0), linewidth = 2)


def display_polylines_as_mesh(viewer, polylines, type, scale=0.01):
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

def display_polylines(viewer, polylines, scale=0.00,r = 0.0, g = 0.0, b = 0.0, t = 1, collection = True, movex = 1.25):


    polylinesScaled = []
    colors = []
    for i in polylines:
        #print(i)
        if(len(i)>1):
            a = i.transformed(cg.transformations.scale.matrix_from_scale_factors([scale,scale,scale]))
            polylinesScaled.append(a)
            if collection==False:
                if(len(i)==4):
                    viewer.add(a,color=(0,0,1),linewidth = 1)
                elif(len(i)==2):
                    viewer.add(a,color=(0,1,0),linewidth =10)
                else:
                    viewer.add(a,color=(r,g,b), linewidth = t)

            if collection==True:
                colors.append((r,g,b))
                #colors.append((1,0,0))

                


    #Translation within function, because there is scaling
    #translate input data
    if(movex>1):
        points = []
        for i in range (len(polylinesScaled)):
            points.extend(polylinesScaled[i])

        bbox = cg.bounding_box(points)
        xmin = bbox[0][0]
        xmax = bbox[1][0]
        offset_dist = abs(xmax-xmin)

        T = cg.transformations.Translation.from_vector([-offset_dist*movex, 0, 0])
        for i in range (len(polylinesScaled)):
            
            polylinesScaled[i].transform(T)
    

    if collection:
        if(len(polylinesScaled)):
            viewer.add(Collection(polylinesScaled), linewidth = t,colors = colors, color = (r,g,b))

        else:
            print("Nothing is displayed")

def display_polyline(viewer, polyline, scale=0.01,r = 0.0, g = 0.0, b = 0.0, t = 1):
    if(len(polyline)>1):
        y = polyline.transformed(cg.transformations.scale.matrix_from_scale_factors([scale,scale,scale]))
        viewer.add(y,color=(r,g,b), linewidth = t)


def display_mesh(viewer, input, scale=0.01,r = 0.0, g = 0.0, b = 0.0, t = 1):

    y = input.transformed(cg.transformations.scale.matrix_from_scale_factors([scale,scale,scale]))
    viewer.add(y,facecolor = (r,g,b), linecolor=(0,0,0), show_edges = True, opacity = 0.9, linewidth = 1,hide_coplanaredges = False)
