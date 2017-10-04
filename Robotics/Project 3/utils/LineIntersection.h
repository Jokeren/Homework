///////////////////////////////////////
// COMP/ELEC/MECH 450/550
// Project 3
// Authors: Keren Zhou
// Date: 09/29/2017
//////////////////////////////////////

#ifndef UTILS_LINEINTERSECTION_H_
#define UTILS_LINEINTERSECTION_H_

#include <utility>

int nodePosition(double xLine, double yLine, double xNode, double yNode);

bool isPointOnLine(double x1Line, double x2Line, double xNode);

bool isLineIntersect(
    const std::pair<double, double>& line1Start,
    const std::pair<double, double>& line1End,
    const std::pair<double, double>& line2Start,
    const std::pair<double, double>& line2End);

#endif  // UTILS_LINEINTERSECTION_H_
