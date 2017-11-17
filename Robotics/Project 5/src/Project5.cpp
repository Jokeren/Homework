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
// Apps
#include <omplapp/apps/SE2RigidBodyPlanning.h>

#include "CostPath.h"
#include "ClearanceCostPath.h"
#include "SmoothCostPath.h"
#include "Optimizer.h"
#include "PerturbingOptimizer.h"
#include "HybridizationOptimizer.h"
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


// This is our state validitiy checker for checking if our point robot is in collision
bool isValidStateSquare(const ompl::base::State* state, const std::vector<Rectangle>& obstacles)
{
    const ompl::base::CompoundState* cstate = state->as<ompl::base::CompoundState>();

    // Extract x, y, theta
    const ompl::base::RealVectorStateSpace::StateType* r2state = cstate->as<ompl::base::RealVectorStateSpace::StateType>(0);
    double x = r2state->values[0];
    double y = r2state->values[1];

    const ompl::base::SO2StateSpace::StateType* so2State = cstate->as<ompl::base::SO2StateSpace::StateType>(1);
    double theta = so2State->value;

    return isValidSquare(x, y, theta, SIDE_LENGTH, obstacles);
}


void planSmoothnessR2(const std::vector<Rectangle> &obstacles,
        const double low, const double high,
        const Robot rstart, const Robot rgoal,
        MethodSetup method)
{
    ompl::base::StateSpacePtr r2(new ompl::base::RealVectorStateSpace(2));

    // Set bounds on R^2
    ompl::base::RealVectorBounds bounds(2);
    bounds.setLow(low);
    bounds.setHigh(high);

    r2->as<ompl::base::RealVectorStateSpace>()->setBounds(bounds);

    // Set validity checkers
    ompl::geometric::SimpleSetupPtr ss(new ompl::geometric::SimpleSetup(r2));

    ss->setStateValidityChecker(std::bind(isValidStatePoint, std::placeholders::_1, obstacles));

    // Set the start and goal states
    ompl::base::ScopedState<> start(r2);
    start[0] = rstart.x;
    start[1] = rstart.y;

    ompl::base::ScopedState<> goal(r2);
    goal[0] = rgoal.x;
    goal[1] = rgoal.y;

    ss->setStartAndGoalStates(start, goal);

    ompl::base::PlannerPtr planner(new ompl::geometric::RRT(ss->getSpaceInformation()));
    ss->setPlanner(planner);

    // Solve the problem within the given time (seconds)
    ompl::base::PlannerStatus solved = ss->solve(10.0);

    if (solved)
    {
        // Print the path to screen
        std::cout << "Found solution:" << std::endl;
        ompl::geometric::PathGeometric& path = ss->getSolutionPath();

        // Initial solution
        path.printAsMatrix(std::cout);
        std::ofstream fout("original_path.txt");
        fout << "Simple" << std::endl;
        path.printAsMatrix(fout);
        fout.close();

        // Set costPath for optimization purpose
        auto costPath = std::make_shared<ompl::geometric::SmoothCostPath>(ss->getSpaceInformation());

        std::shared_ptr<ompl::geometric::Optimizer> op = method == PERTURBING ?
            std::static_pointer_cast<ompl::geometric::Optimizer>(
                    std::make_shared<ompl::geometric::PerturbingOptimizer>(costPath)) :
            std::static_pointer_cast<ompl::geometric::Optimizer>(
                    std::make_shared<ompl::geometric::HybridizationOptimizer>(costPath));

        // Optimize solution
        ompl::geometric::PathGeometric optimizedPath = op->optimizeSolution(ss);

        std::cout << "Optimized solution:" << std::endl;

        // Optimized
        optimizedPath.printAsMatrix(std::cout);
        fout.open("optimized_path.txt");
        fout << "Simple" << std::endl;
        optimizedPath.printAsMatrix(fout);
        fout.close();
    }
    else
        std::cout << "No solution found" << std::endl;
}


void planSmoothnessSE2(const std::vector<Rectangle>& obstacles,
        const double low, const double high,
        const Robot rstart, const Robot rgoal,
        MethodSetup method)
{
    ompl::base::StateSpacePtr r2(new ompl::base::RealVectorStateSpace(2));

    // Set bounds on R^2
    ompl::base::RealVectorBounds bounds(2);
    bounds.setLow(low);
    bounds.setHigh(high);

    r2->as<ompl::base::RealVectorStateSpace>()->setBounds(bounds);

    ompl::base::StateSpacePtr so2(new ompl::base::SO2StateSpace()); 

    ompl::base::StateSpacePtr se2 = r2 + so2;

    // Set validity checkers
    ompl::geometric::SimpleSetupPtr ss(new ompl::geometric::SimpleSetup(se2));

    ss->setStateValidityChecker(std::bind(isValidStateSquare, std::placeholders::_1, obstacles));

    // Set the start and goal states
    ompl::base::ScopedState<> start(se2);
    start[0] = rstart.x;
    start[1] = rstart.y;
    start[2] = rstart.theta;

    ompl::base::ScopedState<> goal(se2);
    goal[0] = rgoal.x;
    goal[1] = rgoal.y;
    goal[2] = rgoal.theta;

    ss->setStartAndGoalStates(start, goal);

    ompl::base::PlannerPtr planner(new ompl::geometric::RRT(ss->getSpaceInformation()));
    ss->setPlanner(planner);

    // Solve the problem within the given time (seconds)
    ompl::base::PlannerStatus solved = ss->solve(10.0);

    if (solved)
    {
        // Print the path to screen
        std::cout << "Found solution:" << std::endl;
        ompl::geometric::PathGeometric& path = ss->getSolutionPath();

        // Initial solution
        path.printAsMatrix(std::cout);
        std::ofstream fout("original_path.txt");
        fout << "Complex" << std::endl;
        path.printAsMatrix(fout);
        fout.close();

        // Set costPath for optimization purpose
        auto costPath = std::make_shared<ompl::geometric::SmoothCostPath>(ss->getSpaceInformation());

        std::shared_ptr<ompl::geometric::Optimizer> op = method == PERTURBING ?
            std::static_pointer_cast<ompl::geometric::Optimizer>(
                    std::make_shared<ompl::geometric::PerturbingOptimizer>(costPath)) :
            std::static_pointer_cast<ompl::geometric::Optimizer>(
                    std::make_shared<ompl::geometric::HybridizationOptimizer>(costPath));

        std::cout << "Optimized solution:" << std::endl;
        ompl::geometric::PathGeometric optimizedPath = op->optimizeSolution(ss);

        // Optimized solution
        optimizedPath.printAsMatrix(std::cout);
        fout.open("optimized_path.txt");
        fout << "Complex" << std::endl;
        optimizedPath.printAsMatrix(fout);
        fout.close();
    }
    else
        std::cout << "No solution found" << std::endl;
}


void planClearanceSE2(MethodSetup method)
{
    // plan in SE2
    ompl::app::SE2RigidBodyPlanning setup;

    // load the robot and the environment
    std::string robot_fname = std::string(OMPLAPP_RESOURCE_DIR) + "/2D/car1_planar_robot.dae";
    std::string env_fname = std::string(OMPLAPP_RESOURCE_DIR) + "/2D/Maze_planar_env.dae";
    setup.setRobotMesh(robot_fname);
    setup.setEnvironmentMesh(env_fname);

    // define starting state
    ompl::base::ScopedState<ompl::base::SE2StateSpace> start(setup.getSpaceInformation());
    start->setX(0.0);
    start->setY(0.0);

    // define goal state
    ompl::base::ScopedState<ompl::base::SE2StateSpace> goal(start);
    goal->setX(26.0);
    goal->setY(0.0);

    // set the start & goal states
    setup.setStartAndGoalStates(start, goal);

    // attempt to solve the problem, and print it to screen if a solution is found
    if (setup.solve())
    {
        ompl::geometric::PathGeometric& path = setup.getSolutionPath();

        // Print path out
        path.printAsMatrix(std::cout);
        // Print path to file
        std::ofstream fout("original_path.txt");
        fout << "Rigid body" << std::endl;
        path.printAsMatrix(fout);
        fout.close();

        // Set costPath for optimization purpose
        auto costPath = std::make_shared<ompl::geometric::ClearanceCostPath>(setup.getSpaceInformation());

        std::shared_ptr<ompl::geometric::Optimizer> op = method == PERTURBING ?
            std::static_pointer_cast<ompl::geometric::Optimizer>(
                    std::make_shared<ompl::geometric::PerturbingOptimizer>(costPath)) :
            std::static_pointer_cast<ompl::geometric::Optimizer>(
                    std::make_shared<ompl::geometric::HybridizationOptimizer>(costPath));

        std::cout << "Optimized solution:" << std::endl;
        ompl::geometric::SimpleSetupPtr ss(&setup);
        ompl::geometric::PathGeometric optimizedPath = op->optimizeSolution(ss);

        // Optimized path
        optimizedPath.printAsMatrix(std::cout);
        fout.open("optimized_path.txt");
        fout << "Rigid body" << std::endl;
        optimizedPath.printAsMatrix(fout);
        fout.close();
    }
    else
    {
        std::cout << "No solution found" << std::endl;
    }
}


int main(int, char **)
{
    //----------------------------
    // Init metric
    //----------------------------
    int metric;
    do
    {
        std::cout << "Optimizing metric: "<< std::endl;
        std::cout << " (1) Smoothness" << std::endl;
        std::cout << " (2) Clearance" << std::endl;

        std::cin >> metric;
    } while (metric < 1 || metric > 2);

    //----------------------------
    // Init setup
    //----------------------------
    int method;
    do
    {
        std::cout << "Optimization method: "<< std::endl;
        std::cout << " (1) Perturbing" << std::endl;
        std::cout << " (2) Hybridization" << std::endl;

        std::cin >> method;
    } while (method < 1 || method > 2);

    if (metric == SMOOTHNESS)
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
                getSimpleObstacles(obstacles);
                break;
            case 2:
                getComplexObstacles(obstacles);
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

        double low, high;
        getBound(low, high);
        switch (shape)
        {
            case 1:
                {
                    Robot start, goal;
                    start.type = 'p';
                    goal.type = 'p';
                    getStart(start);
                    getGoal(goal);
                    planSmoothnessR2(obstacles, low, high, start, goal, (MethodSetup)method);
                    break;
                }
            case 2:
                {
                    Robot start, goal;
                    start.type = 's';
                    goal.type = 's';
                    getStart(start);
                    getGoal(goal);
                    planSmoothnessSE2(obstacles, low, high, start, goal, (MethodSetup)method);
                    break;
                }
            default:
                {
                    std::cerr << "No such shape option!" << std::endl;
                }
        }
    }
    else  // default env
    {
        planClearanceSE2((MethodSetup)method);
    }

    return 0;
}
