///////////////////////////////////////
// COMP/ELEC/MECH 450/550
// Project 3
// Authors: Keren Zhou
// Date: 09/29/2017
//////////////////////////////////////

#ifndef PROJECT3_SETUP_H_
#define PROJECT3_SETUP_H_

#include <vector>

struct Rectangle
{
    // Coordinate of the lower left corner of the rectangle
    double x, y;
    // The width (x-axis extent) of the rectangle
    double width;
    // The height (y-axis extent) of the rectangle
    double height;
};

// Definition of our robot.
struct Robot
{
    // Type = {c,s,p}.  Circle, square, or point robot
    char type;
    // The location of the robot in the environment
    double x, y;
    // The orientation of the square robot.  Undefined for point or circle robot
    double theta;
    // The length of a side of the square robot or the radius of the circle robot
    // Undefined for the point robot.
    double length;
};

void getSimpleObstacles(std::vector<Rectangle>& obstacles);

void getComplexObstacles(std::vector<Rectangle>& obstacles);

void getStart(Robot& robot);

void getGoal(Robot& robot);

void getBound(double& low, double& high);

#endif  // PROJECT3_SETUP_H_
