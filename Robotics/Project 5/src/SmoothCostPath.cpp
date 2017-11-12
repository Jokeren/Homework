///////////////////////////////////////
// COMP/ELEC/MECH 450/550
// Project 5
// Authors: Keren Zhou
// Date: 11/11/2017
//////////////////////////////////////

#include "SmoothCostPath.h"

namespace ompl
{
    namespace geometric
    {
        double SmoothCostPath::cost(size_t index, base::State *state)
        {
            if (index == 0 || index >= path_.getStateCount() - 1)
            {
                return 0;
            }
            else 
            {
                return 0;
            }
        }
    }
}
