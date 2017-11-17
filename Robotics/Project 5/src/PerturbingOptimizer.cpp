///////////////////////////////////////
// COMP/ELEC/MECH 450/550
// Project 5
// Authors: Dave Coleman, Keren Zhou
// Date: 11/11/2017
//////////////////////////////////////

#include "PerturbingOptimizer.h"

namespace ompl
{
    namespace geometric
    {
        PathGeometric PerturbingOptimizer::optimizeSolution(const SimpleSetupPtr ss, double duration)
        {
            // Copy a path
            PathGeometric &path = ss->getSolutionPath();
            PathGeometric bestPath(path);
            double initCost = 0.0, bestCost = 0.0;
            size_t initState = path.getStateCount();
            OMPL_INFORM("init %d points", initState);
            // Perturbing the initial path
            time::point start = time::now();
            if (costPath_)
            {
                costPath_->setPath(path);
                costPath_->initCost();
                initCost = costPath_->getCost();
                bestCost = costPath_->getCost();
                if (duration < std::numeric_limits<double>::epsilon())
                {
                    base::PlannerTerminationCondition neverTerminate = base::plannerNonTerminatingCondition();
                    perturbeRandom(ss->getSpaceInformation(), neverTerminate);
                }
                else
                {
                    perturbeRandom(ss->getSpaceInformation(), base::timedPlannerTerminationCondition(duration));
                }
                // Find optimization by perturbing
                if (costPath_->getCost() < bestCost)
                {
                    bestCost = costPath_->getCost();
                    bestPath = costPath_->getPath();
                }
                else  // No need to update path
                {
                    OMPL_WARN("cost was not reduced by perturbing");
                } 
            }
            else
            {
                OMPL_WARN("costPath has not been setup");
                return path;
            }
            // Perburbing and interpolation
            for (size_t i = 1; i < MAX_INTERPOLATION_; ++i)
            {
                size_t interPoints = initState + (1 << i);
                OMPL_INFORM("change to %d points", interPoints);
                costPath_->setPath(path);
                costPath_->interpolate(interPoints);
                costPath_->initCost();
                // Find optimization without perturbing
                if (costPath_->getCost() < bestCost)
                {
                    bestCost = costPath_->getCost();
                    bestPath = costPath_->getPath();
                }
                if (duration < std::numeric_limits<double>::epsilon())
                {
                    ompl::base::PlannerTerminationCondition neverTerminate = base::plannerNonTerminatingCondition();
                    perturbeRandom(ss->getSpaceInformation(), neverTerminate);
                }
                else
                {
                    perturbeRandom(ss->getSpaceInformation(), base::timedPlannerTerminationCondition(duration));
                }
                // Find optimization by perturbing
                if (costPath_->getCost() < bestCost)
                {
                    bestCost = costPath_->getCost();
                    bestPath = costPath_->getPath();
                }
                else  // No need to update path
                {
                    OMPL_WARN("cost %f was not reduced by perturbing", costPath_->getCost());
                } 
            }
            double optimizeTime = time::seconds(time::now() - start);
            OMPL_INFORM("PerburbingSetup: Path optimization took %f seconds and changed from %d to %d states, from %f to %f cost",
                    optimizeTime, initState, path.getStateCount(), initCost, bestCost);
            return bestPath;
        }


        void PerturbingOptimizer::randomState(const base::SpaceInformationPtr si,
                const base::State *startState, const base::State *endState,
                base::State *midState, base::State *newState)
        {
            double dist = si->distance(startState, endState);
            // find middle point
            si->getStateSpace()->interpolate(startState, endState, 0.5, midState);
            // TODO(keren): try other samplers
            sampler_->sampleUniformNear(newState, midState, dist / 2);
        }


        void PerturbingOptimizer::perturbeRandom(const base::SpaceInformationPtr si, const base::PlannerTerminationCondition &ptc)
        {
            if (!sampler_)
                sampler_ = si->allocStateSampler();

            if (costPath_->getStateCount() < 3)
                return;

            base::State *newState = si->allocState();
            base::State *midState = si->allocState();
            for (size_t iter = 0; iter < MAX_ITERATIONS_ && !ptc; ++iter)
            {
                if (costPath_->getStateCount() > 2)
                {
                    for (size_t i = 2; i < costPath_->getStateCount() && !ptc; ++i)
                    {
                        for (size_t t = 0; t < MAX_RANDOM_TIMES_ && !ptc; ++t)
                        {
                            randomState(si, costPath_->getState(i), costPath_->getState(i - 2), midState, newState);
                            if (si->checkMotion(costPath_->getState(i), newState) && 
                                    si->checkMotion(newState, costPath_->getState(i - 2)))
                            {
                                if (costPath_->updateCost(i - 1, newState))
                                    break;
                            }
                        }
                    }
                }
            }
            si->freeState(midState);
            si->freeState(newState);
        }
    }  // namespace tools
}  // namespace ompl
