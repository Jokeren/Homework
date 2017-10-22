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
void getObstacles(std::vector<Rectangle>& obstacles)
{
    Rectangle rect;
    rect.x = -5;
    rect.y = -5;
    rect.height = 1;
    rect.width = 10;
    obstacles.push_back(rect);

    rect.x = -5;
    rect.y = 9;
    rect.height = 1;
    rect.width = 10;
    obstacles.push_back(rect);

    rect.x = -5;
    rect.y = -3;
    rect.height = 6;
    rect.width = 4;
    obstacles.push_back(rect);

    rect.x = 1;
    rect.y = -3;
    rect.height = 6;
    rect.width = 4;
    obstacles.push_back(rect);
}

void getStart(Robot& robot) {
    robot.x = -4;
    robot.y = -4;
    robot.theta = 0;
    robot.length = CAR_LENGTH;
}

void getGoal(Robot& robot) {
    robot.x = 4;
    robot.y = 4;
    robot.theta = M_PI / 4;
    robot.length = CAR_LENGTH;
}

void getBound(double& low, double& high) {
    low = -5;
    high = 5;
}
