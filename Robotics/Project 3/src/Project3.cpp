#include <iostream>
#include <fstream>
#include <cmath>
#include <functional>

// Planners
#include <ompl/geometric/planners/rrt/RRT.h>
#include <ompl/geometric/planners/est/EST.h>
#include <ompl/geometric/planners/prm/PRM.h>
// Including SimpleSetup.h will pull in MOST of what you need to plan
#include <ompl/geometric/SimpleSetup.h>
// Except for the state space definitions and any planners
#include <ompl/base/spaces/RealVectorStateSpace.h>
#include <ompl/base/spaces/SO2StateSpace.h>
// Benchmark
#include <ompl/tools/benchmark/Benchmark.h>

// The collision checker produced in project 2
#include "Setup.h"
#include "CollisionChecking.h"
#include "RTP.h"

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

// This is our state validity checker for checking if our square robot is in collision
bool isValidStateSquare(const ompl::base::State* state, double sideLength, const std::vector<Rectangle>& obstacles)
{
    const ompl::base::CompoundState* cstate = state->as<ompl::base::CompoundState>();

    // Extract x, y, theta
    const ompl::base::RealVectorStateSpace::StateType* r2state = cstate->as<ompl::base::RealVectorStateSpace::StateType>(0);
    double x = r2state->values[0];
    double y = r2state->values[1];

    const ompl::base::SO2StateSpace::StateType* so2State = cstate->as<ompl::base::SO2StateSpace::StateType>(1);
    double theta = so2State->value;

    return isValidSquare(x, y, theta, sideLength, obstacles);
}

void planWithSimpleSetupR2(const std::vector<Rectangle>& obstacles,
    const double low, const double high, const Robot rstart, const Robot rgoal,
    int benchmark)
{
    // Step 1) Create the state (configuration) space for your system
    // For a robot that can translate in the plane, we can use R^2 directly
    // We also need to set bounds on R^2
    ompl::base::StateSpacePtr r2(new ompl::base::RealVectorStateSpace(2));

    // We need to set bounds on R^2
    ompl::base::RealVectorBounds bounds(2);
    bounds.setLow(low); // x and y have a minimum of -2
    bounds.setHigh(high); // x and y have a maximum of 2

    // Cast the r2 pointer to the derived type, then set the bounds
    r2->as<ompl::base::RealVectorStateSpace>()->setBounds(bounds);

    // Step 2) Create the SimpleSetup container for the motion planning problem.
    // this container ensures that everything is initialized properly before
    // trying to solve the motion planning problem.
    // ALWAYS USE SIMPLE SETUP!  There is no loss of functionality when using
    // this class.
    ompl::geometric::SimpleSetup ss(r2);

    // Step 3) Setup the StateValidityChecker
    // This is a function that takes a state and returns whether the state is a
    // valid configuration of the system or not.  For geometric planning, this
    // is a collision checker.
    // Note, we are "binding" the obstacles to the state validity checker. The
    // _1 notation is from std::placeholders and indicates "the first argument"
    // to the function pointer.
    ss.setStateValidityChecker(std::bind(isValidStatePoint, std::placeholders::_1, obstacles));

    // Step 4) Specify the start and goal states
    // ScopedState creates the correct state instance for the state space
    ompl::base::ScopedState<> start(r2);
    start[0] = rstart.x;
    start[1] = rstart.y;

    ompl::base::ScopedState<> goal(r2);
    goal[0] = rgoal.x;
    goal[1] = rgoal.y;

    // set the start and goal states
    ss.setStartAndGoalStates(start, goal);

    if (benchmark == 1) {
        ompl::tools::Benchmark b(ss, "Point experiment");
        
        // We add the planners to evaluate.
        b.addPlanner(ompl::base::PlannerPtr(new ompl::geometric::PRM(ss.getSpaceInformation())));
        b.addPlanner(ompl::base::PlannerPtr(new ompl::geometric::RRT(ss.getSpaceInformation())));
        b.addPlanner(ompl::base::PlannerPtr(new ompl::geometric::EST(ss.getSpaceInformation())));
        b.addPlanner(ompl::base::PlannerPtr(new ompl::geometric::RTP(ss.getSpaceInformation())));
        // etc
        
        // Now we can benchmark: 5 second time limit for each plan computation,
        // 100 MB maximum memory usage per plan computation, 50 runs for each planner
        // and true means that a text-mode progress bar should be displayed while
        // computation is running.
        ompl::tools::Benchmark::Request req;
        req.maxTime = 5.0;
        req.maxMem = 1000.0;
        req.runCount = 10;
        req.displayProgress = true;
        b.benchmark(req);

        b.saveResultsToFile();
    } else {
        // Step 5) (optional) Specify a planning algorithm to use
        ompl::base::PlannerPtr planner(new ompl::geometric::RTP(ss.getSpaceInformation()));
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
            fout << "R2" << std::endl;
            path.printAsMatrix(fout);
            fout.close();
        }
        else
            std::cout << "No solution found" << std::endl;
    }
}

void planWithSimpleSetupSE2(const std::vector<Rectangle>& obstacles,
    const double low, const double high, const Robot rstart, const Robot rgoal,
    int benchmark)
{
    // Step 1) Create the state (configuration) space for your system
    // In this instance, we will plan for a square of side length 0.3
    // that both translates and rotates in the plane.
    // The state space can be easily composed of simpler state spaces
    ompl::base::StateSpacePtr se2;

    ompl::base::StateSpacePtr r2(new ompl::base::RealVectorStateSpace(2));
    // We need to set bounds on R^2
    ompl::base::RealVectorBounds bounds(2);
    bounds.setLow(low); // x and y have a minimum of -2
    bounds.setHigh(high); // x and y have a maximum of 2

    // Cast the r2 pointer to the derived type, then set the bounds
    r2->as<ompl::base::RealVectorStateSpace>()->setBounds(bounds);

    ompl::base::StateSpacePtr so2(new ompl::base::SO2StateSpace());

    se2 = r2 + so2;

    // Step 2) Create the SimpleSetup container for the motion planning problem.
    // this container ensures that everything is initialized properly before
    // trying to solve the motion planning problem using OMPL.
    // ALWAYS USE SIMPLE SETUP!  There is no loss of functionality when using
    // this class.
    ompl::geometric::SimpleSetup ss(se2);

    // Step 3) Setup the StateValidityChecker
    // This is a function that takes a state and returns whether the state is a
    // valid configuration of the system or not.  For geometric planning, this
    // is a collision checker

    // Note, we are "binding" the side length, 0.3, and the obstacles to the
    // state validity checker. The _1 notation is from std::placeholders and
    // indicates "the first argument" to the function pointer.
    ss.setStateValidityChecker(std::bind(isValidStateSquare, std::placeholders::_1, 0.3, obstacles));

    // Step 4) Specify the start and goal states
    // ScopedState creates the correct state instance for the state space
    // You can index into the components of the state easily with ScopedState
    // The indexes correspond to the order that the StateSpace components were
    // added into the StateSpace
    ompl::base::ScopedState<> start(se2);
    start[0] = rstart.x;
    start[1] = rstart.y;
    start[2] = rstart.theta;

    ompl::base::ScopedState<> goal(se2);
    goal[0] = rgoal.x;
    goal[1] = rgoal.y;
    goal[2] = rgoal.theta;

    // set the start and goal states
    ss.setStartAndGoalStates(start, goal);

    if (benchmark == 1) {
        ompl::tools::Benchmark b(ss, "Square experiment");
        
        // We add the planners to evaluate.
        b.addPlanner(ompl::base::PlannerPtr(new ompl::geometric::PRM(ss.getSpaceInformation())));
        b.addPlanner(ompl::base::PlannerPtr(new ompl::geometric::RRT(ss.getSpaceInformation())));
        b.addPlanner(ompl::base::PlannerPtr(new ompl::geometric::EST(ss.getSpaceInformation())));
        b.addPlanner(ompl::base::PlannerPtr(new ompl::geometric::RTP(ss.getSpaceInformation())));
        // etc
        
        // Now we can benchmark: 5 second time limit for each plan computation,
        // 100 MB maximum memory usage per plan computation, 50 runs for each planner
        // and true means that a text-mode progress bar should be displayed while
        // computation is running.
        ompl::tools::Benchmark::Request req;
        req.maxTime = 5.0;
        req.maxMem = 100.0;
        req.runCount = 50;
        req.displayProgress = true;
        b.benchmark(req);

        b.saveResultsToFile();
    } else {
        // Step 5) (optional) Specify a planning algorithm to use
        ompl::base::PlannerPtr planner(new ompl::geometric::RTP(ss.getSpaceInformation()));
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
            fout << "SE2" << std::endl;
            path.printAsMatrix(fout);
            fout.close();
        }
        else
            std::cout << "No solution found" << std::endl;
    }
}


int main(int, char **)
{
    std::vector<Rectangle> obstacles;

    int choice;
    do
    {
        std::cout << "Plan for: "<< std::endl;
        std::cout << " (1) A point in 2D" << std::endl;
        std::cout << " (2) A rigid box in 2D" << std::endl;

        std::cin >> choice;
    } while (choice < 1 || choice > 3);

    int env;
    do
    {
        std::cout << "Env for: "<< std::endl;
        std::cout << " (1) A simple obstacle" << std::endl;
        std::cout << " (2) Complex obstacles" << std::endl;

        std::cin >> env;
    } while (env < 1 || env > 3);

    if (env == 1) {
        getSimpleObstacles(obstacles);
    } else {
        getComplexObstacles(obstacles);
    }

    int benchmark;
    do
    {
        std::cout << "Bench for: "<< std::endl;
        std::cout << " (1) Run benchmark" << std::endl;
        std::cout << " (2) Simple plan" << std::endl;

        std::cin >> benchmark;
    } while (env < 1 || env > 3);

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
            planWithSimpleSetupR2(obstacles, low, high, start, goal, benchmark);
            break;
        case 2:
            start.type = 's';
            goal.type = 's';
            getStart(start);
            getGoal(goal);
            getBound(low, high);
            planWithSimpleSetupSE2(obstacles, low, high, start, goal, benchmark);
            break;
        defualt:
            std::cerr << "No such option!" << std::endl;
            break;
    }
    return 0;
}

