///////////////////////////////////////
// COMP/ELEC/MECH 450/550
// Project 3
// Modifier: Hankun Deng
// Author: Ioan Sucan
// Date: 09/29/2017
//////////////////////////////////////

#include <omplapp/config.h>
#include <omplapp/apps/SE3RigidBodyPlanning.h>
#include <ompl/tools/benchmark/Benchmark.h>
#include <ompl/geometric/planners/rrt/RRTConnect.h>
#include <ompl/geometric/planners/rrt/RRT.h>
#include <ompl/geometric/planners/kpiece/LBKPIECE1.h>
#include <ompl/geometric/planners/kpiece/BKPIECE1.h>
#include <ompl/geometric/planners/kpiece/KPIECE1.h>
#include <ompl/geometric/planners/sbl/SBL.h>
#include <ompl/geometric/planners/est/EST.h>
#include <ompl/geometric/planners/prm/PRM.h>

#include <ompl/base/samplers/UniformValidStateSampler.h>
#include <ompl/base/samplers/GaussianValidStateSampler.h>
#include <ompl/base/samplers/ObstacleBasedValidStateSampler.h>
#include <ompl/base/samplers/MaximizeClearanceValidStateSampler.h>
#include "RTP/RTP.h"

using namespace ompl;

void benchmarkCubicles(std::string& benchmarkName, app::SE3RigidBodyPlanning& setup,
                double& runtimeLimit, double& memoryLimit, int& runCount)
{
    benchmarkName = std::string("cubicles");
    std::string robotFName = std::string(OMPLAPP_RESOURCE_DIR) + "/3D/cubicles_robot.dae";
    std::string envFName = std::string(OMPLAPP_RESOURCE_DIR) + "/3D/cubicles_env.dae";
    setup.setRobotMesh(robotFName);
    setup.setEnvironmentMesh(envFName);

    base::ScopedState<base::SE3StateSpace> start(setup.getSpaceInformation());
    start->setX(-4.96);
    start->setY(-40.62);
    start->setZ(70.57);
    start->rotation().setIdentity();

    base::ScopedState<base::SE3StateSpace> goal(start);
    goal->setX(200.49);
    goal->setY(-40.62);
    goal->setZ(70.57);
    goal->rotation().setIdentity();

    setup.setStartAndGoalStates(start, goal);
    setup.getSpaceInformation()->setStateValidityCheckingResolution(0.01);
    setup.setup();

    std::vector<double> cs(3);
    cs[0] = 35; cs[1] = 35; cs[2] = 35;
    setup.getStateSpace()->getDefaultProjection()->setCellSizes(cs);

    runtimeLimit = 10.0;
    memoryLimit  = 10000.0; // set high because memory usage is not always estimated correctly
    runCount     = 500;
}

void benchmarkTwistycool(std::string& benchmarkName, app::SE3RigidBodyPlanning& setup,
                double& runtimeLimit, double& memoryLimit, int& runCount)
{
    benchmarkName = std::string("Twistycool");
    std::string robotFName = std::string(OMPLAPP_RESOURCE_DIR) + "/3D/Twistycool_robot.dae";
    std::string envFName = std::string(OMPLAPP_RESOURCE_DIR) + "/3D/Twistycool_env.dae";
    setup.setRobotMesh(robotFName);
    setup.setEnvironmentMesh(envFName);

    base::ScopedState<base::SE3StateSpace> start(setup.getSpaceInformation());
    start->setX(270.);
    start->setY(160.);
    start->setZ(-200.);
    start->rotation().setIdentity();

    base::ScopedState<base::SE3StateSpace> goal(start);
    goal->setX(270.);
    goal->setY(160.);
    goal->setZ(-400.);
    goal->rotation().setIdentity();

    base::RealVectorBounds bounds(3);
    bounds.setHigh(0,350.);
    bounds.setHigh(1,250.);
    bounds.setHigh(2,-150.);
    bounds.setLow(0,200.);
    bounds.setLow(1,75.);
    bounds.setLow(2,-450.);
    setup.getStateSpace()->as<base::SE3StateSpace>()->setBounds(bounds);

    setup.setStartAndGoalStates(start, goal);
    setup.getSpaceInformation()->setStateValidityCheckingResolution(0.01);

    runtimeLimit = 60.0;
    memoryLimit  = 10000.0; // set high because memory usage is not always estimated correctly
    runCount     = 50;
}

void preRunEvent(const base::PlannerPtr& /*planner*/)
{
}

void postRunEvent(const base::PlannerPtr& /*planner*/, tools::Benchmark::RunProperties& /*run*/)
{
}

int main(int argc, char **argv)
{
    app::SE3RigidBodyPlanning setup;
    std::string benchmarkName;
    double runtimeLimit, memoryLimit;
    int runCount;
    int benchmarkId = argc > 1 ? ((argv[1][0] - '0') % 2) : 0;

    if (benchmarkId == 1)
        benchmarkCubicles(benchmarkName, setup, runtimeLimit, memoryLimit, runCount);
    else
        benchmarkTwistycool(benchmarkName, setup, runtimeLimit, memoryLimit, runCount);

    // create the benchmark object and add all the planners we'd like to run
    tools::Benchmark::Request request(runtimeLimit, memoryLimit, runCount);
    tools::Benchmark b(setup, benchmarkName);

    // optionally set pre & pos run events
    b.setPreRunEvent([](const base::PlannerPtr& planner) { preRunEvent (planner); });
    b.setPostRunEvent([](const base::PlannerPtr& planner, tools::Benchmark::RunProperties& run)
        { postRunEvent(planner, run); });

    b.addPlanner(std::make_shared<geometric::RRT>(setup.getSpaceInformation()));
    b.addPlanner(std::make_shared<geometric::EST>(setup.getSpaceInformation()));
    b.addPlanner(std::make_shared<geometric::PRM>(setup.getSpaceInformation()));
    b.addPlanner(ompl::base::PlannerPtr(new ompl::geometric::RTP(setup.getSpaceInformation())));

    int samplerId = argc > 2 ? ((argv[2][0] - '0') % 4) : -1;

    if (samplerId == 0 || samplerId < 0)
    {
        // run all planners with a uniform valid state sampler on the benchmark problem
        setup.getSpaceInformation()->setValidStateSamplerAllocator(
            [](const base::SpaceInformation *si) -> base::ValidStateSamplerPtr
            {
                return std::make_shared<base::UniformValidStateSampler>(si);
            });
        b.setExperimentName(benchmarkName + "_uniform_sampler");
        b.benchmark(request);
        b.saveResultsToFile();
    }

    return 0;
}
