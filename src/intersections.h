#ifndef COMPAS_INTERSECTIONS_H
#define COMPAS_INTERSECTIONS_H

#include <compas.h>


std::vector<compas::RowMatrixXd>
pmp_intersection_mesh_mesh(
    Eigen::Ref<const compas::RowMatrixXd> & VA,
    Eigen::Ref<const compas::RowMatrixXi> & FA,
    Eigen::Ref<const compas::RowMatrixXd> & VB,
    Eigen::Ref<const compas::RowMatrixXi> & FB);


#endif /* COMPAS_INTERSECTIONS_H */
