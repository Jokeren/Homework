///////////////////////////////////////
// COMP/ELEC/MECH 450/550
// Project 5
// Authors: Dave Coleman, Keren Zhou
// Date: 11/11/2017
//////////////////////////////////////

#ifndef SRC_PERTURBING_OPTIMIZER_H_
#define SRC_PERTURBING_OPTIMIZER_H_

#include <ompl/geometric/SimpleSetup.h>
#include <ompl/geometric/PathSimplifier.h>

#include "Optimizer.h"
#include "CostPath.h"

namespace ompl
{
    namespace geometric
    {
        class PerturbingOptimizer : public Optimizer
        {
            public:
                explicit PerturbingOptimizer(std::shared_ptr<CostPath> costPath) : Optimizer(costPath) {}

                // RVO
                virtual PathGeometric optimizeSolution(const SimpleSetupPtr ss, double duration = 0.0);

            private:
                void randomState(const base::SpaceInformationPtr si,
                        const base::State *startState, const base::State *endState,
                        base::State *midState, base::State *newState);

                void perturbeRandom(const base::SpaceInformationPtr si, const base::PlannerTerminationCondition &ptc);

                base::StateSamplerPtr sampler_;

                const size_t MAX_ITERATIONS_ = 1000;

                const size_t MAX_RANDOM_TIMES_ = 10;

                const size_t MAX_INTERPOLATION_ = 8;
        };
    }  // namespace tools
}  // namespace ompl

#endif