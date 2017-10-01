#include "LineIntersection.h"
#include <algorithm>
#include <utility>

// Test a node's position related to a line
// 1: left
// -1: right
// 0: on line
int nodePosition(double xLine, double yLine, double xNode, double yNode) {
    double res = xNode * yLine - yNode * xLine;
    if (res > 0) {
        return 1;
    } else if (res < 0) {
        return -1;
    } else {
        return 0;
    }
}

// Test a node's position related to a line
bool isPointOnLine(double x1Line, double x2Line, double xNode) {
    if (std::min(x1Line, x2Line) > xNode || std::max(x1Line, x2Line) < xNode) {
        return false;
    } else {
        return true;
    }
}

// Test if two lines intersect
bool isLineIntersect(
    const std::pair<double, double>& line1Start,
    const std::pair<double, double>& line1End,
    const std::pair<double, double>& line2Start,
    const std::pair<double, double>& line2End) {
    double x1 = line1Start.first, x2 = line1End.first, x3 = line2Start.first, x4 = line2End.first;
    double y1 = line1Start.second, y2 = line1End.second, y3 = line2Start.second, y4 = line2End.second;
    int pos1 = nodePosition(x2 - x1, y2 - y1, x3 - x1, y3 - y1);
    int pos2 = nodePosition(x2 - x1, y2 - y1, x4 - x1, y4 - y1);
    int pos3 = nodePosition(x4 - x3, y4 - y3, x1 - x3, y1 - y3);
    int pos4 = nodePosition(x4 - x3, y4 - y3, x2 - x3, y2 - y3);
    
    if (pos1 == 0 && isPointOnLine(x1, x2, x3)) return true;
    if (pos2 == 0 && isPointOnLine(x1, x2, x4)) return true;
    if (pos3 == 0 && isPointOnLine(x3, x4, x1)) return true;
    if (pos4 == 0 && isPointOnLine(x3, x4, x2)) return true;
    
    if (pos1 == pos2 || pos3 == pos4) {
        return false;
    } else {
        if (pos1 != 0 && pos2 != 0 && pos3 != 0 && pos4 != 0) {
            return true;
        } else {
            return false;
        }
    }
}
