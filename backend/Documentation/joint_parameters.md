# Joint Parameters

## Joint Parameters: Type, Divisions, Double

Three parameters are used per joint type in one continuous array, meaning only one joint type can be used or other joint type wil used the same parameters.


```cpp

std::vector<double> JOINTS_TYPES{
    300, 0.5, 8, // 1-9 ss_e_ip (side-to-side edge in-plane)
    450, 0.64, 15, // 10-19 ss_e_op (side-to-side edge out-of-plane)
    450, 0.5, 20, // 20-29 ts_e_p (top-to-side edge plane)
    300, 0.5, 30, // 30-39 cr_c_ip (cross cutting in-plane)
    6, 0.95, 40, // 40-49 tt_e_p  (top-to-top edge plane)
    300, 0.5, 58, // 50-59 ss_e_r (side-to-side edge rotated)
    300, 1.0, 60 // 60-69 b (boundary)
};

```

**WARNING:** This is a simplification, that can be extended to the solution below. At the current moment only one joint is possible, while there can be multiple joints e.g. in-plane finger joints and butterfly joints. \
**SOLUTION:** Replace this vector by a list of dictionary, the example is below. The first one is always default, while other are specific to each type.

```cpp
std::vector< std::map< int, std::vector<int>> > JOINTS_TYPES{
    {-1, {300, 0.5, 8}},
    {-1, {450, 0.64, 15}}, {12, {450, 0.64, 15,1}},
    {-1, {450, 0.5, 20}},
    {-1, {300, 0.5, 30}}, {31, {300, 0.5, 30}}, {33, {300, 0.5}},
    {-1, {6, 0.95, 40, }},
    {-1, {300, 0.5, 58}},   
    {-1, {300, 1.0, 60}},    
    };
```

## Joint Volume Change 

Joints are defined within two rectangles: **[ ] <---> [ ]** \
Depending if the structure is made from plates or beams the joint volume needs to be modified. \
The rectangles two edge can be extended by modifying **JOINT_VOLUME_EXTENSION[0]** and **JOINT_VOLUME_EXTENSION[1]**. While the distance between them can be modified by **JOINT_VOLUME_EXTENSION[2]**.

```cpp
 double JOINT_VOLUME_EXTENSION[5] = {
    0.0, //x
    0.0, //y
    0.0, //z
    0, 
    0}; 
```
