///////////////////////////////////////
// COMP/ELEC/MECH 450/550
// Project 4
// Authors: Keren Zhou
// Date: 09/29/2017
//////////////////////////////////////

#ifndef UTILS_COLLISION_CHECKING_H_
#define UTILS_COLLISION_CHECKING_H_

#include "Setup.h"

// Intersect the point (x,y) with the set of rectangles. If the point lies
// outside of all obstacles, return true.
bool isValidPoint(double x, double y, const std::vector<Rectangle>& obstacles);

// Intersect a square with center at (x,y), orientation theta, and the given
// side length with the set of rectangles. If the square lies outside of all
// obstacles, return true
bool isValidSquare(double x, double y, double theta, double sideLength, const std::vector<Rectangle>& obstacles);

#endif  // UTILS_COLLISION_CHECKING_H_
