#include "Setup.h"

// Intersect the point (x,y) with the set of rectangles. If the point lies
// outside of all obstacles, return true.
bool isValidPoint(double x, double y, const std::vector<Rectangle>& obstacles);

// Intersect a square with center at (x,y), orientation theta, and the given
// side length with the set of rectangles. If the square lies outside of all
// obstacles, return true
bool isValidSquare(double x, double y, double theta, double sideLength, const std::vector<Rectangle>& obstacles);
