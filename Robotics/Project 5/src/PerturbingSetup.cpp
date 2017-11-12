///////////////////////////////////////
// COMP/ELEC/MECH 450/550
// Project 5
// Authors: Dave Coleman, Keren Zhou
// Date: 11/11/2017
//////////////////////////////////////

#include "PerturbingSetup.h"

namespace ompl
{
    namespace geometric
    {
        void PerturbingSetup::optimizeSolutionRandom(double duration)
        {
            if (pdef_)
            {
                const base::PathPtr &p = pdef_->getSolutionPath();
                if (p)
                {
                    time::point start = time::now();
                    auto &path = static_cast<geometric::PathGeometric &>(*p);
                    std::size_t numStates = path.getStateCount();
                    // Keep two copies of path
                    if (costPath_)
                    {
                        costPath_->setPath(path);
                        costPath_->initCost();
                        double initCost = costPath_->getCost();
                        if (duration < std::numeric_limits<double>::epsilon())
                        {
                            ompl::base::PlannerTerminationCondition neverTerminate = base::plannerNonTerminatingCondition();
                            perturbeRandom(neverTerminate);
                        }
                        else
                        {
                            perturbeRandom(base::timedPlannerTerminationCondition(duration));
                        }
                        // Find optimization
                        if (costPath_->getCost() < initCost)
                        {
                            path = costPath_->getPath();
                        }
                        else  // No need to update path
                        {
                            OMPL_WARN("cost was not reduced");
                        } 
                    }
                    else
                    {
                        OMPL_WARN("costPath has not been setup");
                    }
                    simplifyTime_ = time::seconds(time::now() - start);
                    OMPL_INFORM("PerburbingSetup: Path optimization took %f seconds and changed from %d to %d states",
                                simplifyTime_, numStates, path.getStateCount());
                    return;
                }
            }
            OMPL_WARN("No solution to optimize");
        }


        void PerturbingSetup::optimizeSolutionGradient(double duration)
        {
            if (pdef_)
            {
                const base::PathPtr &p = pdef_->getSolutionPath();
                if (p)
                {
                    time::point start = time::now();
                    auto &path = static_cast<geometric::PathGeometric &>(*p);
                    std::size_t numStates = path.getStateCount();
                    costPath_->setPath(path);
                    if (duration < std::numeric_limits<double>::epsilon())
                    {
                        ompl::base::PlannerTerminationCondition neverTerminate = base::plannerNonTerminatingCondition();
                        perturbeGradient(neverTerminate);
                    }
                    else
                        perturbeGradient(base::timedPlannerTerminationCondition(duration));
                    simplifyTime_ = time::seconds(time::now() - start);
                    OMPL_INFORM("PerburbingSetup: Path optimization took %f seconds and changed from %d to %d states",
                                simplifyTime_, numStates, path.getStateCount());
                    return;
                }
            }
            OMPL_WARN("No solution to optimize");
        }


        void PerturbingSetup::randomState(const base::State *startState, const base::State *endState,
                                          base::State *midState, base::State *newState)
        {
            double dist = si_->distance(startState, endState);
            // find middle point
            si_->getStateSpace()->interpolate(startState, endState, 0.5, midState);
            // TODO(keren): try other samplers
            sampler_->sampleUniformNear(newState, midState, dist);
        }

        void PerturbingSetup::perturbeRandom(const base::PlannerTerminationCondition &ptc)
        {
            if (!sampler_)
                sampler_ = si_->allocStateSampler();

            if (costPath_->getStateCount() < 3)
                return;

            // interpolate
            if (ptc == false)
                costPath_->interpolate();

            base::State *newState = si_->allocState();
            base::State *midState = si_->allocState();
            for (size_t iter = 0; iter < MAX_ITERATIONS_ && ptc; ++iter)
            {
                if (costPath_->getStateCount() > 2)
                {
                    for (size_t i = 2; i < costPath_->getStateCount() && ptc; ++i)
                    {
                        for (size_t t = 0; t < MAX_RANDOM_TIMES_ && ptc; ++t)
                        {
                            randomState(costPath_->getState(i), costPath_->getState(i - 2), midState, newState);
                            if (si_->checkMotion(costPath_->getState(i), newState) && 
                                si_->checkMotion(newState, costPath_->getState(i - 2)) &&
                                costPath_->cost(i - 1, newState) < costPath_->getCost())
                            {
                                costPath_->updateCost(i - 1, newState);
                                break;
                            }
                        }
                    }
                }
            }
            si_->freeState(midState);
            si_->freeState(newState);
        }


        void PerturbingSetup::perturbeGradient(const base::PlannerTerminationCondition &ptc)
        {
        }

    }  // namespace tools
}  // namespace ompl
