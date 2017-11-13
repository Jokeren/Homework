///////////////////////////////////////
// COMP/ELEC/MECH 450/550
// Project 3
// Authors: Keren Zhou
// Date: 09/29/2017
//////////////////////////////////////

#include "Setup.h"
#include <vector>
#include <cmath>

// Retrieve a representation of the environment
void getSimpleObstacles(std::vector<Rectangle>& obstacles) {
    Rectangle rect;
    rect.x = -4;
    rect.y = 0;
    rect.height = 1.0;
    rect.width = 8.0;
    obstacles.push_back(rect);
}


void getComplexObstacles(std::vector<Rectangle>& obstacles) {
    Rectangle rect;
    rect.x = -1; 
    rect.y = 1;
    rect.height = 0.5;
    rect.width = 4.5;
    obstacles.push_back(rect);

    rect.x = -1; 
    rect.y = 1.5;
    rect.height = 2.5;
    rect.width = 0.5;
    obstacles.push_back(rect);

    rect.x = -1; 
    rect.y = 4;
    rect.height = 0.5;
    rect.width = 4.5;
    obstacles.push_back(rect);

    rect.x = 3.4;
    rect.y = 1.5;
    rect.height = 1.5;
    rect.width = 0.5;
    obstacles.push_back(rect);
}


void getStart(Robot& robot)
{
    robot.x = -4;
    robot.y = -4;
    if (robot.type == 's') {
        robot.theta = 0;
        robot.length = 0.2;
    }
}


void getGoal(Robot& robot)
{
    robot.x = 3;
    robot.y = 3;
    if (robot.type == 's') {
        robot.theta = M_PI / 4;
        robot.length = 0.2;
    }
}


void getBound(double& low, double& high) {
    low = -5;
    high = 5;
}
