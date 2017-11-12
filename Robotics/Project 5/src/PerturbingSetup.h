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
#include <ompl/tools/config/MagicConstants.h>

namespace ompl
{
    namespace tools
    {
        OMPL_CLASS_FORWARD(PerturbingSetup);

        class PerturbingSetup : public geometric::SimpleSetup
        {
        public:
            /** \brief Constructor needs the state space used for planning. */
            explicit PerturbingSetup(const base::SpaceInformationPtr &si) : geometric::SimpleSetup(si) {};

            /** \brief Constructor needs the state space used for planning. */
            explicit PerturbingSetup(const base::StateSpacePtr &space) : geometric::SimpleSetup(space) {};

            void setCostFunction(std::function<double(const geometric::PathGeometric&)> costFunction)
            {
                this->costFunction_ = costFunction;
            }

            void optimizeSolutionRandom(double duration = 0.0);

            void optimizeSolutionGradient(double duration = 0.0);

        private:
            void perturbeRandom(geometric::PathGeometric &path, const base::PlannerTerminationCondition &ptc);

            void perturbeGradient(geometric::PathGeometric &path, const base::PlannerTerminationCondition &ptc);

            std::function<double(const geometric::PathGeometric&)> costFunction_;
        };
    }  // namespace tools
}  // namespace ompl

#endif
