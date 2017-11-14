///////////////////////////////////////
// COMP/ELEC/MECH 450/550
// Project 5
// Authors: Dave Coleman, Keren Zhou
// Date: 11/11/2017
//////////////////////////////////////

#ifndef SRC_HYBRIDIZATION_SETUP_H_
#define SRC_HYBRIDIZATION_SETUP_H_

#include <ompl/geometric/SimpleSetup.h>
#include <ompl/geometric/PathSimplifier.h>

#include "CostPath.h"

namespace ompl
{
    namespace geometric
    {
        OMPL_CLASS_FORWARD(HybridizationSetup);

        class HybridizationSetup : public SimpleSetup
        {
        public:
            /** \brief Constructor needs the state space used for planning. */
            explicit HybridizationSetup(const base::SpaceInformationPtr &si) : SimpleSetup(si) {};

            /** \brief Constructor needs the state space used for planning. */
            explicit HybridizationSetup(const base::StateSpacePtr &space) : SimpleSetup(space) {};

            void setCostPath(std::shared_ptr<CostPath> costPath)
            {
                this->costPath_ = costPath;
            }

            void optimizeSolution(double duration = 0.0);

        private:
            std::shared_ptr<CostPath> costPath_;
        };
    }  // namespace tools
}  // namespace ompl

#endif
