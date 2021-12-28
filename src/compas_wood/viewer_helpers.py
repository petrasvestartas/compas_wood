#compas
import compas.geometry as cg
from compas.datastructures import Mesh

#viewer
from compas_view2.app import App
from compas_view2.objects import Collection
import compas_view2.objects as w

def display(input, result):
    viewer = App(show_grid=False, width = 3840,height = 2160-250)  
    DisplayPolylines(viewer,input,0.01,0.5, 0.5, 0.5,1,True) #polylines without joints
    DisplayPolylines(viewer,result,0.01, 1.0, 0.0, 0.0,3,False) #polylines with joints
    viewer.run()




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
