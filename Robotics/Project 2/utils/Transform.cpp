#include "Transform.h"
#include <cmath>
#include <utility>

// Apply 2D transformation matrix
std::pair<double, double> transform2D(
    const std::pair<double, double>& point, double theta, double XMove, double YMove)
{
    double XDest = point.first * cos(theta) - point.second * sin(theta) + XMove;
    double YDest = point.first * sin(theta) + point.second * cos(theta) + YMove;
    return std::make_pair(XDest, YDest);
}
