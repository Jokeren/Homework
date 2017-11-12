///////////////////////////////////////
// COMP/ELEC/MECH 450/550
// Project 5
// Authors: Dave Coleman, Keren Zhou
// Date: 11/11/2017
//////////////////////////////////////

#ifndef SRC_PERTURBING_SETUP_H_
#define SRC_PERTURBING_SETUP_H_

#include <ompl/geometric/SimpleSetup.h>
#include <ompl/geometric/PathSimplifier.h>

#include "CostPath.h"

namespace ompl
{
    namespace geometric
    {
        OMPL_CLASS_FORWARD(PerturbingSetup);

        class PerturbingSetup : public SimpleSetup
        {
        public:
            /** \brief Constructor needs the state space used for planning. */
            explicit PerturbingSetup(const base::SpaceInformationPtr &si) : SimpleSetup(si) {};

            /** \brief Constructor needs the state space used for planning. */
            explicit PerturbingSetup(const base::StateSpacePtr &space) : SimpleSetup(space) {};

            void setCostPath(std::shared_ptr<CostPath> costPath)
            {
                this->costPath_ = costPath;
            }

            void optimizeSolutionRandom(double duration = 0.0);

            void optimizeSolutionGradient(double duration = 0.0);

        private:
            void randomState(const base::State *startState, const base::State *endState, base::State *newState);

            void perturbeRandom(const base::PlannerTerminationCondition &ptc);

            void perturbeGradient(const base::PlannerTerminationCondition &ptc);

            std::shared_ptr<CostPath> costPath_;

            base::StateSamplerPtr sampler_;

            size_t MAX_ITERATIONS_ = 10;

            size_t MAX_RANDOM_TIMES_ = 100;
        };
    }  // namespace tools
}  // namespace ompl

#endif
