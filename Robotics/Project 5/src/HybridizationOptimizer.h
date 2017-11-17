///////////////////////////////////////
// COMP/ELEC/MECH 450/550
// Project 5
// Authors: Dave Coleman, Keren Zhou
// Date: 11/11/2017
//////////////////////////////////////

#ifndef SRC_HYBRIDIZATION_OPTIMIZER_H_
#define SRC_HYBRIDIZATION_OPTIMIZER_H_

#include <ompl/geometric/SimpleSetup.h>
#include <ompl/geometric/PathSimplifier.h>

#include "Optimizer.h"

namespace ompl
{
    namespace geometric
    {
        class HybridizationOptimizer : public Optimizer
        {
            public:
                explicit HybridizationOptimizer(std::shared_ptr<CostPath> costPath) : Optimizer(costPath) {}

                // RVO
                virtual PathGeometric optimizeSolution(const SimpleSetupPtr ss, double duration = 0.0);
        };
    }  // namespace tools
}  // namespace ompl

#endif
