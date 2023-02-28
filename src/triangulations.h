#ifndef COMPAS_TRIANGULATIONS_H
#define COMPAS_TRIANGULATIONS_H

#include <compas.h>


compas::RowMatrixXi
pmp_delaunay_triangulation(
    Eigen::Ref<const compas::RowMatrixXd> & V);


std::tuple<compas::RowMatrixXd, compas::RowMatrixXi>
pmp_constrained_delaunay_triangulation(
    Eigen::Ref<const compas::RowMatrixXd> & B,
    Eigen::Ref<const compas::RowMatrixXd> & P,
    std::vector< Eigen::Ref<const compas::RowMatrixXd> > & holes,
    std::vector< Eigen::Ref<const compas::RowMatrixXd> > & curves,
    bool is_conforming = false);


std::tuple<compas::RowMatrixXd, compas::RowMatrixXi>
pmp_refined_delaunay_mesh(
    Eigen::Ref<const compas::RowMatrixXd> & B,
    Eigen::Ref<const compas::RowMatrixXd> & P,
    std::vector< Eigen::Ref<const compas::RowMatrixXd> > & holes,
    std::vector< Eigen::Ref<const compas::RowMatrixXd> > & curves,
    double minangle = 0.0,
    double maxlength = 0.0,
    bool is_optimized = true);


#endif /* COMPAS_TRIANGULATIONS_H */
