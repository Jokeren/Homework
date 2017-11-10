///////////////////////////////////////
// COMP/ELEC/MECH 450/550
// Project 5
// Modifer: Keren Zhou
// Date: 11/10/2017
//////////////////////////////////////

#include <iostream>
#include <fstream>
#include <cmath>
#include <functional>

// Use RRT planner
#include <ompl/geometric/planners/rrt/RRT.h>
// Including SimpleSetup.h will pull in MOST of what you need to plan
#include <ompl/geometric/SimpleSetup.h>
// Except for the state space definitions and any planners
#include <ompl/base/spaces/RealVectorStateSpace.h>
#include <ompl/base/spaces/SO2StateSpace.h>

#include "../utils/Setup.h"
#include "../utils/CollisionChecking.h"


// This is our state validitiy checker for checking if our point robot is in collision
bool isValidStatePoint(const ompl::base::State* state, const std::vector<Rectangle>& obstacles)
{
    const ompl::base::RealVectorStateSpace::StateType* r2state;
    r2state = state->as<ompl::base::RealVectorStateSpace::StateType>();
    // Extract x, y
    double x = r2state->values[0];
    double y = r2state->values[1];

    return isValidPoint(x, y, obstacles);
}


void planSimple(const std::vector<Rectangle>& obstacles,
                const double low, const double high,
                const Robot rstart, const Robot rgoal)
{
    ompl::base::StateSpacePtr r2(new ompl::base::RealVectorStateSpace(2));

    // We need to set bounds on R^2
    ompl::base::RealVectorBounds bounds(2);
    bounds.setLow(low); // x and y have a minimum of -2
    bounds.setHigh(high); // x and y have a maximum of 2

    // Cast the r2 pointer to the derived type, then set the bounds
    r2->as<ompl::base::RealVectorStateSpace>()->setBounds(bounds);

    ompl::geometric::SimpleSetup ss(r2);

    ss.setStateValidityChecker(std::bind(isValidStatePoint, std::placeholders::_1, obstacles));

    ompl::base::ScopedState<> start(r2);
    start[0] = rstart.x;
    start[1] = rstart.y;

    ompl::base::ScopedState<> goal(r2);
    goal[0] = rgoal.x;
    goal[1] = rgoal.y;

    // set the start and goal states
    ss.setStartAndGoalStates(start, goal);

    ompl::base::PlannerPtr planner(new ompl::geometric::RRT(ss.getSpaceInformation()));
    ss.setPlanner(planner);

    // Step 6) Attempt to solve the problem within the given time (seconds)
    ompl::base::PlannerStatus solved = ss.solve(10.0);

    if (solved)
    {
        // Apply some heuristics to simplify (and prettify) the solution
        ss.simplifySolution();

        // print the path to screen
        std::cout << "Found solution:" << std::endl;
        ompl::geometric::PathGeometric& path = ss.getSolutionPath();
        path.interpolate(50);
        path.printAsMatrix(std::cout);

        // print path to file
        std::ofstream fout("path.txt");
        fout << "Simple" << std::endl;
        path.printAsMatrix(fout);
        fout.close();
    }
    else
        std::cout << "No solution found" << std::endl;
}


int main(int, char **)
{
    int choice;
    do
    {
        std::cout << "Plan for: "<< std::endl;
        std::cout << " (1) A point in 2D" << std::endl;
        std::cout << " (2) A rigid box in 2D" << std::endl;

        std::cin >> choice;
    } while (choice < 1 || choice > 2);

    std::vector<Rectangle> obstacles;
    getObstacles(obstacles);

    Robot start, goal;
    double low, high;
    switch(choice)
    {
        case 1:
            start.type = 'p';
            goal.type = 'p';
            getStart(start);
            getGoal(goal);
            getBound(low, high);
            planSimple(obstacles, low, high, start, goal);
            break;
        case 2:
            break;
        defualt:
            std::cerr << "No such option!" << std::endl;
            break;
    }
    return 0;
}
