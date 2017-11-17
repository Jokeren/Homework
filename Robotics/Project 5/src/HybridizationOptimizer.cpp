///////////////////////////////////////
// COMP/ELEC/MECH 450/550
// Project 5
// Authors: Dave Coleman, Keren Zhou
// Date: 11/11/2017
//////////////////////////////////////

#include "HybridizationOptimizer.h"

namespace ompl
{
    namespace geometric
    {
        PathGeometric HybridizationOptimizer::optimizeSolution(const SimpleSetupPtr sp, double duration)
        {
            PathGeometric &path = sp->getSolutionPath();
            PathGeometric bestPath(path);
            double initCost = 0.0, bestCost = 0.0;
            size_t initState = path.getStateCount();
            OMPL_INFORM("init %d points", initState);
            time::point start = time::now();
            if (costPath_)
            {
                costPath_->setPath(path);
                costPath_->initCost();
                initCost = costPath_->getCost();
                bestCost = costPath_->getCost();
            }
            else
            {
                OMPL_WARN("costPath has not been setup");
                return bestPath;
            }
            double optimizeTime = time::seconds(time::now() - start);
            OMPL_INFORM("PerburbingSetup: Path optimization took %f seconds and changed from %d to %d states, from %f to %f cost",
                    optimizeTime, initState, path.getStateCount(), initCost, bestCost);
            return bestPath;
        }

    }  // namespace tools
}  // namespace ompl
