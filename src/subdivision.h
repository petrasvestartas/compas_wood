#ifndef COMPAS_SUBD_H
#define COMPAS_SUBD_H

#include <compas.h>


std::tuple<compas::RowMatrixXd, compas::RowMatrixXi>
subd_catmullclark(
    compas::RowMatrixXd & V,
    std::vector< std::vector<int> > & faces,
    unsigned int k);


#endif /* COMPAS_SUBD_H */
