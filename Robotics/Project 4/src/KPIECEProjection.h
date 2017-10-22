///////////////////////////////////////
// COMP/ELEC/MECH 450/550
// Project 4
// Authors: Keren Zhou
// Date: 10/21/2017
//////////////////////////////////////

#ifndef SRC_KPIECE_PROJECTION_H_
#define SRC_KPIECE_PROJECTION_H_

#include <ompl/base/ProjectionEvaluator.h>

namespace ob = ompl::base;
namespace oc = ompl::control;

class PendulumProjection : public ob::ProjectionEvaluator
{
public:
	PendulumProjection(const ob::StateSpacePtr &space) : ob::ProjectionEvaluator(space)
	{
	}
	
	virtual unsigned int getDimension(void) const
	{
	    return 2;
	}
	
	virtual void defaultCellSizes(void)
	{
	    cellSizes_.resize(2);
	    cellSizes_[0] = 0.1;
	    cellSizes_[1] = 0.25;
	}
	
	virtual void project(const ob::State *state, ob::EuclideanProjection &projection) const
	{
	    const double value = state->as<ob::CompoundStateSpace::StateType>()->as<ob::SO2StateSpace::StateType>(0)->value;
	    const double *values = state->as<ob::CompoundStateSpace::StateType>()->as<ob::RealVectorStateSpace::StateType>(1)->values;
	    projection(0) = value;
	    projection(1) = values[1];
	}

};


class CarProjection : public ob::ProjectionEvaluator
{
public:
	CarProjection(const ob::StateSpacePtr &space) : ob::ProjectionEvaluator(space)
	{
	}
	
	virtual unsigned int getDimension(void) const
	{
	    return 2;
	}
	
	virtual void defaultCellSizes(void)
	{
	    cellSizes_.resize(2);
	    cellSizes_[0] = 0.1;
	    cellSizes_[1] = 0.25;
	}
	
	virtual void project(const ob::State *state, ob::EuclideanProjection &projection) const
	{
	    const double *values = state->as<ob::CompoundStateSpace::StateType>()
                                   ->as<ob::SE2StateSpace::StateType>(0)
                                   ->as<ob::RealVectorStateSpace::StateType>(0)->values;
	    const double theta = state->as<ob::CompoundStateSpace::StateType>()
                                   ->as<ob::SE2StateSpace::StateType>(1)
                                   ->as<ob::SO2StateSpace::StateType>(1)->value;
	    const double *speed = state->as<ob::CompoundStateSpace::StateType>()->as<ob::RealVectorStateSpace::StateType>(1)->values;
	    projection(0) = values[0] * cos(theta) * speed[0];
	    projection(1) = values[1] * sin(theta) * speed[0];
	}

};

#endif  // SRC_KPIECE_PROJECTION_H_
