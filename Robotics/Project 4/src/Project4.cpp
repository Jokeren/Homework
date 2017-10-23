/*********************************************************************
* Software License Agreement (BSD License)
*
*  Copyright (c) 2012, Rice University
*  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   * Redistributions in binary form must reproduce the above
*     copyright notice, this list of conditions and the following
*     disclaimer in the documentation and/or other materials provided
*     with the distribution.
*   * Neither the name of the Rice University nor the names of its
*     contributors may be used to endorse or promote products derived
*     from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
*  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
*  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
*  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
*  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
*  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*********************************************************************/

/* Modifier: Keren Zhou */
/* Date: 20/10/2017 */
/* Author: Ryan Luna */

#include <ompl/control/SpaceInformation.h>
#include <ompl/base/spaces/SE2StateSpace.h>
#include <ompl/control/ODESolver.h>
#include <ompl/control/spaces/RealVectorControlSpace.h>
#include <ompl/control/SimpleSetup.h>
#include <ompl/config.h>
// Planners
#include <ompl/control/planners/rrt/RRT.h>
#include <ompl/control/planners/kpiece/KPIECE1.h>
// std
#include <iostream>
#include <valarray>
#include <limits>

#include "../utils/Setup.h"
#include "../utils/CollisionChecking.h"
#include "KPIECEProjection.h"
#include "RGRRT.h"

namespace ob = ompl::base;
namespace oc = ompl::control;

// Definition of the ODE for the kinematic car.
// This method is analogous to the above KinematicCarModel::ode function.
void KinematicCarODE(const oc::ODESolver::StateType& q, const oc::Control* control, oc::ODESolver::StateType& qdot)
{
    const double *u = control->as<oc::RealVectorControlSpace::ControlType>()->values;

    const double theta = q[2];
    const double speed = q[3];
    const double omega = u[0];
    const double accelerate = u[1];

    // Zero out qdot
    qdot.resize(q.size(), 0);

    qdot[0] = speed * cos(theta);
    qdot[1] = speed * sin(theta);
    qdot[2] = omega;
    qdot[3] = accelerate;
}


// This is a callback method invoked after numerical integration.
void KinematicCarPostIntegration(const ob::State* /*state*/, const oc::Control* /*control*/, const double /*duration*/, ob::State *result)
{
    // Normalize orientation between 0 and 2*pi
    ob::SO2StateSpace SO2;
    SO2.enforceBounds(result->as<ob::CompoundStateSpace::StateType>()
                            ->as<ob::SE2StateSpace::StateType>(0)
                            ->as<ob::SO2StateSpace::StateType>(1));
}


bool isCarStateValid(const ob::State *state, const oc::SpaceInformation *si, const std::vector<Rectangle>& obstacles)
{
    //    ob::ScopedState<ob::SE2StateSpace>
    /// cast the abstract state type to the type we expect
    const auto *cs = state->as<ob::CompoundStateSpace::StateType>();
    const auto *se2state = cs->as<ob::SE2StateSpace::StateType>(0);

    /// extract the first component of the state and cast it to what we expect
    const auto *pos = se2state->as<ob::RealVectorStateSpace::StateType>(0);

    /// extract the second component of the state and cast it to what we expect
    const auto *rot = se2state->as<ob::SO2StateSpace::StateType>(1);

    /// check validity of state defined by pos & rot
    double x = pos->values[0];
    double y = pos->values[1];
    double theta = rot->value;

    // return a value that is always true but uses the two variables we define, so we avoid compiler warnings
    return si->satisfiesBounds(state) && isValidSquare(x, y, theta, CAR_LENGTH, obstacles) && (const void*)rot != (const void*)pos;
}


// Definition of the ODE for the kinematic pendulum.
// This method is analogous to the above KinematicCarModel::ode function.
void KinematicPendulumODE(const oc::ODESolver::StateType& q, const oc::Control* control, oc::ODESolver::StateType& qdot)
{
    const double *u = control->as<oc::RealVectorControlSpace::ControlType>()->values;
    const double theta = q[0];
    const double omega = q[1];

    // Zero out qdot
    qdot.resize(2, 0);

    qdot[0] = omega;
    qdot[1] = -9.8 * cos(theta) + u[0];
}


// This is a callback method invoked after numerical integration.
void KinematicPendulumPostIntegration(const ob::State* /*state*/, const oc::Control* /*control*/, const double /*duration*/, ob::State *result)
{
    // Normalize orientation between 0 and 2*pi
    ob::SO2StateSpace SO2;
    SO2.enforceBounds(result->as<ob::CompoundStateSpace::StateType>()->as<ob::SO2StateSpace::StateType>(1));
}


bool isPendulumStateValid(const oc::SpaceInformation *si, const ob::State *state)
{
    //    ob::ScopedState<ob::SE2StateSpace>
    /// cast the abstract state type to the type we expect
    const auto *cs = state->as<ob::CompoundStateSpace::StateType>();

    /// extract the first component of the state and cast it to what we expect
    const auto *pos = cs->as<ob::RealVectorStateSpace::StateType>(0);

    /// extract the second component of the state and cast it to what we expect
    const auto *rot = cs->as<ob::SO2StateSpace::StateType>(1);

    /// check validity of state defined by pos & rot

    // return a value that is always true but uses the two variables we define, so we avoid compiler warnings
    return si->satisfiesBounds(state) && (const void*)rot != (const void*)pos;
}


class CarControlSpace : public oc::RealVectorControlSpace
{
public:
    CarControlSpace(const ob::StateSpacePtr &stateSpace) : oc::RealVectorControlSpace(stateSpace, 2)
    {
    }
};


class PendulumControlSpace : public oc::RealVectorControlSpace
{
public:
    PendulumControlSpace(const ob::StateSpacePtr &stateSpace) : oc::RealVectorControlSpace(stateSpace, 1)
    {
    }
};


ob::PlannerPtr getPlannerPtr(int alg, int choice, const oc::SimpleSetup& ss)
{
    switch (alg) {
        case 1:  // RRT
        {
            return ob::PlannerPtr(new oc::RRT(ss.getSpaceInformation()));
            break;
        }
        case 2:  // KPIECE
        {
            ob::StateSpacePtr space = ss.getStateSpace();
            ob::PlannerPtr planner(new oc::KPIECE1(ss.getSpaceInformation()));
            if (choice == 1) {
                space->registerProjection("pendulumProjection", ob::ProjectionEvaluatorPtr(new PendulumProjection(space)));
                planner->as<oc::KPIECE1>()->setProjectionEvaluator("pendulumProjection");
                return planner;
            } else if (choice == 2) {
                space->registerProjection("carProjection", ob::ProjectionEvaluatorPtr(new CarProjection(space)));
                planner->as<oc::KPIECE1>()->setProjectionEvaluator("carProjection");
                return planner;
            }
            break;
        }
        case 3:  // RGRRT
        {
            return ob::PlannerPtr(new oc::RGRRT(ss.getSpaceInformation()));
        }
        default:
        {
            return NULL;
            break;
        }
    }
    std::cerr << "No such options" << std::endl;
}


void planPendulum(int torque_bound, int alg, int choice, double seconds = 10.0)
{
    /// construct the state space we are planning in
    ompl::base::StateSpacePtr so2(new ompl::base::SO2StateSpace());
    ompl::base::StateSpacePtr rv(new ompl::base::RealVectorStateSpace(1));
    ompl::base::StateSpacePtr space = so2 + rv;

    /// so2 bound is set automatically
    /// set the bounds for rv
    ob::RealVectorBounds bounds(1);
    bounds.setLow(-10);
    bounds.setHigh(10);

    rv->as<ob::RealVectorStateSpace>()->setBounds(bounds);

    // create a control space
    auto cspace(std::make_shared<PendulumControlSpace>(space));

    // set the bounds for the control space
    ob::RealVectorBounds cbounds(1);
    cbounds.setLow(-torque_bound);
    cbounds.setHigh(torque_bound);

    cspace->setBounds(cbounds);

    // define a simple setup class
    oc::SimpleSetup ss(cspace);

    ob::PlannerPtr planner = getPlannerPtr(alg, choice, ss);
    std::cout << "herehere\n" << std::endl;

    // set state validity checking for this space
    oc::SpaceInformation *si = ss.getSpaceInformation().get();
    ss.setStateValidityChecker(
        [si](const ob::State *state) { return isPendulumStateValid(si, state); });

    auto odeSolver(std::make_shared<oc::ODEBasicSolver<>>(ss.getSpaceInformation(), &KinematicPendulumODE));
    ss.setStatePropagator(oc::ODESolver::getStatePropagator(odeSolver, &KinematicPendulumPostIntegration));

    /// create a start state
    ob::ScopedState<ob::CompoundStateSpace> start(space);
    start->as<ob::SO2StateSpace::StateType>(0)->value = -M_PI / 2;
    start->as<ob::RealVectorStateSpace::StateType>(1)->values[0] = 0;

    /// create a  goal state; use the hard way to set the elements
    ob::ScopedState<ob::CompoundStateSpace> goal(space);
    goal->as<ob::SO2StateSpace::StateType>(0)->value = M_PI / 2;
    goal->as<ob::RealVectorStateSpace::StateType>(1)->values[0] = 0;

    /// set the start and goal states
    ss.setStartAndGoalStates(start, goal, 0.05);

    /// set planner
    ss.setPlanner(planner);

    /// attempt to solve the problem within one second of planning time
    ob::PlannerStatus solved = ss.solve(seconds);

    if (solved)
    {
        std::cout << "Found solution:" << std::endl;
        /// print the path to screen

        ss.getSolutionPath().asGeometric().printAsMatrix(std::cout);
    }
    else
        std::cout << "No solution found" << std::endl;
}


void planCar(int alg, int choice,
        const Robot& startCar, const Robot& goalCar,
        const std::vector<Rectangle>& obstacles,
        double spaceLow, double spaceHigh,
        double speedLow, double speedHigh,
        double omegaLow, double omegaHigh,
        double accelerateLow, double accelerateHigh)
{
    /// construct the state space we are planning in
    ompl::base::StateSpacePtr se2(new ompl::base::SE2StateSpace());
    ompl::base::StateSpacePtr rv(new ompl::base::RealVectorStateSpace(1));
    ompl::base::StateSpacePtr space = se2 + rv;

    /// so2 bound is set automatically
    /// set the bounds for the R^2 part of SE(2)
    ob::RealVectorBounds bounds(2);
    bounds.setLow(spaceLow);
    bounds.setHigh(spaceHigh);

    se2->as<ob::SE2StateSpace>()->setBounds(bounds);

    // set the bounds for the R part
    ob::RealVectorBounds speedBounds(1);
    speedBounds.setLow(speedLow);
    speedBounds.setHigh(speedHigh);
    rv->as<ob::RealVectorStateSpace>()->setBounds(speedBounds);

    // create a control space
    auto cspace(std::make_shared<CarControlSpace>(space));

    // set the bounds for the control space
    ob::RealVectorBounds cbounds(2);
    cbounds.setLow(0, omegaLow);
    cbounds.setHigh(0, omegaHigh);
    cbounds.setLow(1, accelerateLow);
    cbounds.setHigh(1, accelerateHigh);

    cspace->setBounds(cbounds);

    // define a simple setup class
    oc::SimpleSetup ss(cspace);

    ob::PlannerPtr planner = getPlannerPtr(alg, choice, ss);

    // set state validity checking for this space
    oc::SpaceInformation *si = ss.getSpaceInformation().get();
    ss.setStateValidityChecker(std::bind(isCarStateValid, std::placeholders::_1, si, obstacles));

    // Use the ODESolver to propagate the system.  Call KinematicCarPostIntegration
    // when integration has finished to normalize the orientation values.
    auto odeSolver(std::make_shared<oc::ODEBasicSolver<>>(ss.getSpaceInformation(), &KinematicCarODE));
    ss.setStatePropagator(oc::ODESolver::getStatePropagator(odeSolver, &KinematicCarPostIntegration));

    /// create a start state
    ob::ScopedState<ob::CompoundStateSpace> start(space);
    start->as<ob::SE2StateSpace::StateType>(0)->setX(startCar.x);
    start->as<ob::SE2StateSpace::StateType>(0)->setY(startCar.y);
    start->as<ob::SE2StateSpace::StateType>(0)->setYaw(startCar.theta);
    start->as<ob::RealVectorStateSpace::StateType>(1)->values[0] = 0;

    /// create a  goal state; use the hard way to set the elements
    ob::ScopedState<ob::CompoundStateSpace> goal(space);
    goal->as<ob::SE2StateSpace::StateType>(0)->setX(goalCar.x);
    goal->as<ob::SE2StateSpace::StateType>(0)->setY(goalCar.y);
    goal->as<ob::SE2StateSpace::StateType>(0)->setYaw(goalCar.theta);
    goal->as<ob::RealVectorStateSpace::StateType>(1)->values[0] = 0;

    /// set the start and goal states
    ss.setStartAndGoalStates(start, goal, 0.05);

    /// set planner
    ss.setPlanner(planner);

    /// attempt to solve the problem within one second of planning time
    ob::PlannerStatus solved = ss.solve(20.0);

    if (solved)
    {
        std::cout << "Found solution:" << std::endl;
        /// print the path to screen

        ss.getSolutionPath().asGeometric().printAsMatrix(std::cout);
    }
    else
        std::cout << "No solution found" << std::endl;
}


int main(int /*argc*/, char ** /*argv*/)
{
    std::cout << "OMPL version: " << OMPL_VERSION << std::endl;

    int choice;
    do
    {
        std::cout << "Plan by: "<< std::endl;
        std::cout << " (1) Pendulum" << std::endl;
        std::cout << " (2) Car" << std::endl;

        std::cin >> choice;
    } while (choice < 1 || choice > 3);

    int alg;
    do
    {
        std::cout << "Plan by: "<< std::endl;
        std::cout << " (1) RRT" << std::endl;
        std::cout << " (2) KPIECE" << std::endl;
        std::cout << " (3) RG-RRT" << std::endl;

        std::cin >> alg;
    } while (alg < 1 || alg > 4);


    switch (choice)
    {
        case 1:
        {
            int torque;
            std::cout << "Torque: "<< std::endl;
            std::cin >> torque;
            planPendulum(torque, alg, choice); 
            break;
        }
        case 2:
        {
            Robot startCar, goalCar;
            double spaceLow, spaceHigh;
            double speedLow, speedHigh;
            double omegaLow, omegaHigh;
            double accelerateLow, accelerateHigh;
            std::vector<Rectangle> obstacles;

            getStart(startCar);
            getGoal(goalCar);
            getBound(spaceLow, spaceHigh);
            getSpeedBound(speedLow, speedHigh);
            getOmegaBound(omegaLow, omegaHigh);
            getAccelerateBound(accelerateLow, accelerateHigh);
            getObstacles(obstacles);

            planCar(alg, choice,
                    startCar, goalCar,
                    obstacles,
                    spaceLow, spaceHigh,
                    speedLow, speedHigh,
                    omegaLow, omegaHigh,
                    accelerateLow, accelerateHigh);
            break;
        }
        default:
            break;
    }

    return 0;
}
