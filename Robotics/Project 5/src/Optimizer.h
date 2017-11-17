///////////////////////////////////////
// COMP/ELEC/MECH 450/550
// Project 5
// Authors: Dave Coleman, Keren Zhou
// Date: 11/11/2017
//////////////////////////////////////

#ifndef SRC_OPTIMIZER_H_
#define SRC_OPTIMIZER_H_

#include <ompl/geometric/SimpleSetup.h>
#include <ompl/geometric/PathSimplifier.h>

#include "CostPath.h"

namespace ompl
{
    namespace geometric
    {
        class Optimizer
        {
            public:
                explicit Optimizer(std::shared_ptr<CostPath> costPath) : costPath_(costPath) {};

                // RVO
                virtual PathGeometric optimizeSolution(const SimpleSetupPtr ss, double duration = 0.0) = 0;

            protected:
                std::shared_ptr<CostPath> costPath_;
        };
    }  // namespace tools
}  // namespace ompl

#endif
