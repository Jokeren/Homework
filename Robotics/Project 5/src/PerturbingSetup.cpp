///////////////////////////////////////
// COMP/ELEC/MECH 450/550
// Project 5
// Authors: Dave Coleman, Keren Zhou
// Date: 11/11/2017
//////////////////////////////////////

#include "PerturbingSetup.h"

namespace ompl
{
    namespace tools
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
                    if (duration < std::numeric_limits<double>::epsilon())
                    {
                        ompl::base::PlannerTerminationCondition neverTerminate = base::plannerNonTerminatingCondition();
                        perturbeRandom(static_cast<geometric::PathGeometric &>(*p), neverTerminate);
                    }
                    else
                        perturbeRandom(static_cast<geometric::PathGeometric &>(*p), base::timedPlannerTerminationCondition(duration));
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
                    if (duration < std::numeric_limits<double>::epsilon())
                    {
                        ompl::base::PlannerTerminationCondition neverTerminate = base::plannerNonTerminatingCondition();
                        perturbeGradient(static_cast<geometric::PathGeometric &>(*p), neverTerminate);
                    }
                    else
                        perturbeGradient(static_cast<geometric::PathGeometric &>(*p), base::timedPlannerTerminationCondition(duration));
                    simplifyTime_ = time::seconds(time::now() - start);
                    OMPL_INFORM("PerburbingSetup: Path optimization took %f seconds and changed from %d to %d states",
                                simplifyTime_, numStates, path.getStateCount());
                    return;
                } }
            OMPL_WARN("No solution to optimize");
        }


        void PerturbingSetup::perturbeRandom(geometric::PathGeometric &path, const base::PlannerTerminationCondition &ptc)
        {
        }


        void PerturbingSetup::perturbeGradient(geometric::PathGeometric &path, const base::PlannerTerminationCondition &ptc)
        {
        }

    }  // namespace tools
}  // namespace ompl
