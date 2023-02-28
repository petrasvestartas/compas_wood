#ifndef COMPAS_MESHING_H
#define COMPAS_MESHING_H

#include <compas.h>


std::tuple<compas::RowMatrixXd, compas::RowMatrixXi>
pmp_remesh(
    Eigen::Ref<const compas::RowMatrixXd> & V,
    Eigen::Ref<const compas::RowMatrixXi> & F,
    double target_edge_length,
    unsigned int number_of_iterations = 10,
    bool do_project = true);


#endif /* COMPAS_MESHING_H */
