
#include "cgal_mesh_boolean.h"
// data structure
#include "wood_cut.h"
#include "wood_main.h"
// viewer
#include "viewer_wood.h"
#include "wood_xml.h"

// joinery
#include "wood_joint_lib.h"
#include "wood_joint.h"

#ifndef WOOD_TEST_H
#define WOOD_TEST_H
// test
// #include "wood/include/wood_test.h"
// #include "wood/include/viewer_wood.h"
// #include "wood/include/wood_data_set.h"

namespace wood_test
{
    void test_cgal_mesh_boolean_0();

    void test_side_to_top();

    void test_F_get_connection_zones_D_in_top_to_top_corner();


    void test_F_three_valence_joint_addition_vidy_D_chapel_corner();

    void test_F_three_valence_joint_addition_vidy_D_chapel_one_layer();

    void test_F_three_valence_joint_addition_vidy_D_chapel_simple();

    void test_F_three_valence_joint_addition_vidy_D_chapel();

    void ss_e_op_4();
}

#endif // WOOD_TEST_H