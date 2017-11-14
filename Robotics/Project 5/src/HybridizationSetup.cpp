///////////////////////////////////////
// COMP/ELEC/MECH 450/550
// Project 5
// Authors: Dave Coleman, Keren Zhou
// Date: 11/11/2017
//////////////////////////////////////

#include "HybridizationSetup.h"

namespace ompl
{
    namespace geometric
    {
        void HybridizationSetup::optimizeSolution(double duration)
        {
            if (pdef_)
            {
                const base::PathPtr &p = pdef_->getSolutionPath();
                if (p)
                {
                    time::point start = time::now();
                    auto &path = static_cast<geometric::PathGeometric &>(*p);
                    double initCost = 0.0, bestCost = 0.0;
                    size_t initState = path.getStateCount();
                    // do whatever you want for optimization
                    //
                    // code
                    simplifyTime_ = time::seconds(time::now() - start);
                    OMPL_INFORM("PerburbingSetup: Path optimization took %f seconds and changed from %d to %d states, from %f to %f cost",
                                simplifyTime_, initState, path.getStateCount(), initCost, bestCost);
                    return;
                }
            }
            OMPL_WARN("No solution to optimize");
        }

    }  // namespace tools
}  // namespace ompl
