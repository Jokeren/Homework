///////////////////////////////////////
// COMP/ELEC/MECH 450/550
// Project 5
// Authors: Keren Zhou
// Date: 11/11/2017
//////////////////////////////////////

#ifndef SRC_SMOOTH_COST_PATH_H_
#define SRC_SMOOTH_COST_PATH_H_

#include "CostPath.h"

namespace ompl
{
    namespace geometric
    {
        class SmoothCostPath : public CostPath
        {
        public:
            explicit SmoothCostPath(const base::SpaceInformationPtr &si) : CostPath(si) {}

            explicit SmoothCostPath(const PathGeometric &path) : CostPath(path) {}

            // Compute cost
            virtual bool updateCost(int index, base::State *&state);

            // Init cost before using CostPath
            virtual void initCost();
        
        protected:
            const double LAMBDA_ = 0.01;
        };
    }  // namespace tools
}  // namespace ompl

#endif
