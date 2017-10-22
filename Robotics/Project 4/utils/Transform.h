///////////////////////////////////////
// COMP/ELEC/MECH 450/550
// Project 4
// Authors: Keren Zhou
// Date: 09/29/2017
//////////////////////////////////////

#ifndef UTILS_TRANSFORM_H_
#define UTILS_TRANSFORM_H_

#include <utility>

std::pair<double, double> transform2D(
    const std::pair<double, double>& point, double theta, double XMove, double YMove);

#endif  // UTILS_TRANSFORM_H_
