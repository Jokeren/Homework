#include <utility>

int nodePosition(double xLine, double yLine, double xNode, double yNode);

bool isPointOnLine(double x1Line, double x2Line, double xNode);

bool isLineIntersect(
    const std::pair<double, double>& line1Start,
    const std::pair<double, double>& line1End,
    const std::pair<double, double>& line2Start,
    const std::pair<double, double>& line2End);
