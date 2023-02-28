#ifndef COMPAS_MEASURE_H
#define COMPAS_MEASURE_H

#include <compas.h>


double
pmp_volume(
    Eigen::Ref<const compas::RowMatrixXd> & V,
    Eigen::Ref<const compas::RowMatrixXi> & F);


#endif /* COMPAS_MEASURE_H */
