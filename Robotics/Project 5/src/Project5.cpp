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

#include "SmoothCostPath.h"
#include "CostPath.h"
#include "PerturbingSetup.h"
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

    // Set bounds on R^2
    ompl::base::RealVectorBounds bounds(2);
    bounds.setLow(low);
    bounds.setHigh(high);

    r2->as<ompl::base::RealVectorStateSpace>()->setBounds(bounds);

    // Set validity checkers
    ompl::geometric::PerturbingSetup ps(r2);

    ps.setStateValidityChecker(std::bind(isValidStatePoint, std::placeholders::_1, obstacles));

    // Set the start and goal states
    ompl::base::ScopedState<> start(r2);
    start[0] = rstart.x;
    start[1] = rstart.y;

    ompl::base::ScopedState<> goal(r2);
    goal[0] = rgoal.x;
    goal[1] = rgoal.y;

    ps.setStartAndGoalStates(start, goal);

    ompl::base::PlannerPtr planner(new ompl::geometric::RRT(ps.getSpaceInformation()));
    ps.setPlanner(planner);

    // Solve the problem within the given time (seconds)
    ompl::base::PlannerStatus solved = ps.solve(10.0);

    // Set costPath for optimization purpose
    auto costPath = std::static_pointer_cast<ompl::geometric::CostPath>(
        std::make_shared<ompl::geometric::SmoothCostPath>(ps.getSpaceInformation()));
    ps.setCostPath(costPath);

    if (solved)
    {
        // Print the path to screen
        std::cout << "Found solution:" << std::endl;
        ompl::geometric::PathGeometric& path = ps.getSolutionPath();

        // Initial solution
        path.printAsMatrix(std::cout);

        // Optimize solution
        ps.optimizeSolutionRandom();
        std::cout << "Optimized solution:" << std::endl;
        path = ps.getSolutionPath();
        path.printAsMatrix(std::cout);

        // Print path to file
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
    //----------------------------
    // Init environment
    //----------------------------
    int choice;
    do
    {
        std::cout << "Plan for: "<< std::endl;
        std::cout << " (1) Simple environment" << std::endl;
        std::cout << " (2) Complex environment" << std::endl;

        std::cin >> choice;
    } while (choice < 1 || choice > 2);

    std::vector<Rectangle> obstacles;
    switch (choice)
    {
        case 1:
            getObstacles(obstacles);
            break;
        case 2:
            break;
        default:
            std::cerr << "No such environment option!" << std::endl;
            break;
    }
    
    //----------------------------
    // Init robot shape
    //----------------------------
    int shape;
    do
    {
        std::cout << "Robot shape: "<< std::endl;
        std::cout << " (1) Point" << std::endl;
        std::cout << " (2) Square" << std::endl;

        std::cin >> shape;
    } while (shape < 1 || shape > 2);

    Robot start, goal;
    double low, high;
    getStart(start);
    getGoal(goal);
    getBound(low, high);
    switch (shape)
    {
        case 1:
        {
            start.type = 'p';
            goal.type = 'p';
            planSimple(obstacles, low, high, start, goal);
            break;
        }
        case 2:
        {
            start.type = 's';
            goal.type = 's';
            break;
        }
        default:
        {
            std::cerr << "No such shape option!" << std::endl;
        }
    }

    return 0;
}
