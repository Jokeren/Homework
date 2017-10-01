///////////////////////////////////////
// COMP/ELEC/MECH 450/550
// Project 2
// Authors: FILL ME OUT!!
// Date: FILL ME OUT!!
//////////////////////////////////////

#include "CollisionChecking.h"
#include <utility>
#include <cmath>

#include "../utils/Transform.h"
#include "../utils/LineIntersection.h"

static const size_t CORNERS = 4;

// Intersect the point (x,y) with the set of rectangles. If the point lies
// outside of all obstacles, return true.
bool isValidPoint(double x, double y, const std::vector<Rectangle>& obstacles)
{
    // A point robot is not collided if it is not within an obstacle
    for (size_t i = 0; i < obstacles.size(); ++i) {
        if (obstacles[i].x <= x && obstacles[i].x + obstacles[i].width >= x &&
            obstacles[i].y <= y && obstacles[i].y + obstacles[i].height >= y) {
            return false;
        }
    }
    return true;
}
    
// Intersect a circle with center (x,y) and given radius with the set of
// rectangles. If the circle lies outside of all obstacles, return true.
bool isValidCircle(double x, double y, double radius, const std::vector<Rectangle>& obstacles)
{
    std::pair<double, double> vertice[CORNERS];

    // A circle robot is not collided if it is:
    // 1. Not within an obstalce
    // 2. Not touched an obstalces vertice
    for (size_t i = 0; i < obstacles.size(); ++i) {
        double xMin = obstacles[i].x;
        double yMin = obstacles[i].y;
        double xMax = obstacles[i].x + obstacles[i].width;
        double yMax = obstacles[i].y + obstacles[i].height;
        vertice[0] = std::make_pair(xMin, yMin);
        vertice[1] = std::make_pair(xMin, yMax);
        vertice[2] = std::make_pair(xMax, yMin);
        vertice[3] = std::make_pair(xMax, yMax);

        if (xMin - radius <= x && x <= xMax + radius && yMin <= y && y <= yMax) {
            return false;
        }
        if (xMin <= x && x <= xMax && yMin - radius <= y && y <= yMax + radius) {
            return false;
        }
        for (size_t j = 0; j < CORNERS; ++j) {
            double distance = (vertice[j].first - x) * (vertice[j].first - x) + 
                (vertice[j].second - y) * (vertice[j].second - y);
            if (distance <= radius * radius) {
                return false;
            }
        }
    }
    return true;
}

// Test if childVertice shape is inside parentVertice shape by checking each point of it
static bool contained(std::pair<double, double> childVertice[], std::pair<double, double> parentVertice[]) {
    for (size_t i = 0; i < CORNERS; ++i) {
        double x = childVertice[i].first;
        double y = childVertice[i].second;
	    double x1 = parentVertice[0].first, x2 = parentVertice[1].first, 
               x3 = parentVertice[2].first, x4 = parentVertice[3].first;
	    double y1 = parentVertice[0].second, y2 = parentVertice[1].second,
               y3 = parentVertice[2].second, y4 = parentVertice[3].second;
        int pos1 = nodePosition(x2 - x1, y2 - y1, x - x1, y - y1);
        int pos2 = nodePosition(x3 - x2, y3 - y2, x - x2, y - y2);
        int pos3 = nodePosition(x3 - x4, y3 - y4, x - x3, y - y3);
        int pos4 = nodePosition(x4 - x1, y4 - y1, x - x4, y - y4);
        if (!(pos1 != pos3 && pos2 != pos4))
            return false;
    }
    return true;
}

// Intersect a square with center at (x,y), orientation theta, and the given
// side length with the set of rectangles. If the square lies outside of all
// obstacles, return true.
bool isValidSquare(double x, double y, double theta, double sideLength, const std::vector<Rectangle>& obstacles)
{
    // A square robot is not collided if it is:
    // 1. Not inside an obstalce
    // 2. Not intersect with each line of an obstacle
    // Detect collision by following steps:
    // 1. Calculate out position of four vertice by the transformation matrix
    // 2. Collision detection
    std::pair<double, double> squareVertice[CORNERS];
    std::pair<double, double> obstacleVertice[CORNERS];
    squareVertice[0] = std::make_pair(0.5 * sideLength, 0.5 * sideLength);
    squareVertice[1] = std::make_pair(0.5 * sideLength, -0.5 * sideLength);
    squareVertice[2] = std::make_pair(-0.5 * sideLength, -0.5 * sideLength);
    squareVertice[3] = std::make_pair(-0.5 * sideLength, 0.5 * sideLength);
    // Rotate
    squareVertice[0] = transform2D(squareVertice[0], theta, x, y);
    squareVertice[1] = transform2D(squareVertice[1], theta, x, y);
    squareVertice[2] = transform2D(squareVertice[2], theta, x, y);
    squareVertice[3] = transform2D(squareVertice[3], theta, x, y);
    // All pair check
    for (size_t i = 0; i < obstacles.size(); ++i) {
        double xMin = obstacles[i].x;
        double yMin = obstacles[i].y;
        double xMax = obstacles[i].x + obstacles[i].width;
        double yMax = obstacles[i].y + obstacles[i].height;
        obstacleVertice[0] = std::make_pair(xMin, yMin);
        obstacleVertice[1] = std::make_pair(xMin, yMax);
        obstacleVertice[2] = std::make_pair(xMax, yMax);
        obstacleVertice[3] = std::make_pair(xMax, yMin);
        // Iterate each line
        for (size_t j = 0; j < CORNERS; ++j) {
            std::pair<double, double> line1Start = squareVertice[j];
            std::pair<double, double> line1End = squareVertice[(j + 1) % CORNERS];
            for (size_t k = 0; k < CORNERS; ++k) {
                std::pair<double, double> line2Start = obstacleVertice[k];
                std::pair<double, double> line2End = obstacleVertice[(k + 1) % CORNERS];
                if (isLineIntersect(line1Start, line1End, line2Start, line2End)) {
                    return false;
                }
            }
        }
        if (contained(squareVertice, obstacleVertice)) return false;
        if (contained(obstacleVertice, squareVertice)) return false;
    }
    return true;
}

// Add any custom debug / development code here. This code will be executed
// instead of the statistics checker (Project2.cpp). Any code submitted here
// MUST compile, but will not be graded.
void debugMode(const std::vector<Robot>& /*robots*/, const std::vector<Rectangle>& /*obstacles*/, const std::vector<bool>& /*valid*/)
{
}
