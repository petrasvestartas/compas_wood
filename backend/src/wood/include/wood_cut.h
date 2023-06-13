#ifndef WOOD_CUT_TYPE_H
#define WOOD_CUT_TYPE_H

//#include "../../stdafx.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// cut_type describes the fabrication method
// this type is mainly used in wood_joint.h in the class joint
// the joint is defined in the wood_joint_library.h as predefined types
//
// string_to_cut_type and cut_type_to_string are used for conversion, when writing to XML from it
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace wood_cut {
enum cut_type
{
    nothing,

    // plates
    edge_insertion,
    hole,
    insert_between_multiple_edges,

    // beams
    slice, // always projected
    slice_projectsheer,

    mill, // always inside volume
    mill_project,
    mill_projectsheer,

    cut,
    cut_project,
    cut_projectsheer,
    cut_reverse,

    conic, // always projected
    conic_reverse,

    drill,
    drill_50,
    drill_10,
};

static std::map<std::string, cut_type> string_to_cut_type{

    {"nothing", nothing},

    {"hole", hole},
    {"edge_insertion", edge_insertion},
    {"insert_between_multiple_edges", insert_between_multiple_edges},

    {"slice", slice},
    {"slice_projectsheer", slice_projectsheer},

    {"mill", mill},
    {"mill_project", mill_project},
    {"mill_projectsheer", mill_projectsheer},

    {"cut", cut},
    {"cut_project", cut_project},
    {"cut_projectsheer", cut_projectsheer},
    {"cut_reverse", cut_reverse},

    {"conic", conic},
    {"conic_reverse", conic_reverse},

    {"drill", drill},
};

static std::array<std::string, 16> cut_type_to_string{

    "nothing",

    "hole",
    "edge_insertion",
    "insert_between_multiple_edges",

    "slice",
    "slice_projectsheer",

    "mill",
    "mill_project",
    "mill_projectsheer",

    "cut",
    "cut_project",
    "cut_projectsheer",
    "cut_reverse",

    "conic",
    "conic_reverse",

    "drill",

};
}
#endif // WOOD_CUT_TYPE_H