Joinery
* create joint "remove_sides_from_two_sides"

Pointcloud processing in Rhino
* Rhino crop the environment manually
* Rhino distance cluster
* Rhino mesh
* Rhino orient mesh vertically, where center is on 0,0,0
* Rhino orient meshes to lines 
* Rhino perform boolean cuts

Tool-path
* update .net output, that gives information about cutting is milling, slice.
* before output correct winding direction using inside/outside options (add types to cpp)
* in rhino check toolpaths such as slice, if needed flip them in cpp
* Check tool ids and radius
* test if 5-axis cutting is correct regarding tool radius, plates were off
* Be careful with end cuts, they must be cut in one feed very SLOWLY
* Prepare tool-for tenon and mortise and try to cut it on Friday next week.

Practical Information
* Ask francois to mount B-Axis
* test tool-path on rectangular elements, check the tolerances
* Find chainsaw and manual saws and chisels
* Prepare pieces rectangular pieces
* Screw each piece to beams
