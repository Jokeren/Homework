///////////////////////////////////////
// COMP/ELEC/MECH 450/550
// Project 5
// Authors: Keren Zhou
// Date: 11/11/2017
//////////////////////////////////////

#ifndef SRC_CLEARANCE_COST_PATH_H_
#define SRC_CLEARANCE_COST_PATH_H_

#include "CostPath.h"

namespace ompl
{
    namespace geometric
    {
        class ClearanceCostPath : public CostPath
        {
            public:
                explicit ClearanceCostPath(const base::SpaceInformationPtr &si) : CostPath(si) {}

                explicit ClearanceCostPath(const PathGeometric &path) : CostPath(path) {}

                // Compute cost
                virtual bool updateCost(int index, base::State *&state);

                // Init cost before using CostPath
                virtual void initCost();
        };
    }  // namespace tools
}  // namespace ompl

#endif
