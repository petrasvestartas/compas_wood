#ifndef COMPAS_SLICER_H
#define COMPAS_SLICER_H

#include <compas.h>


std::vector<compas::RowMatrixXd>
pmp_slice_mesh(
    Eigen::Ref<const compas::RowMatrixXd> & V,
    Eigen::Ref<const compas::RowMatrixXi> & F,
    Eigen::Ref<const compas::RowMatrixXd> & P,
    Eigen::Ref<const compas::RowMatrixXd> & N);


#endif /* COMPAS_SLICER_H */
