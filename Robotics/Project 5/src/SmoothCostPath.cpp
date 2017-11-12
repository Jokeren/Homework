///////////////////////////////////////
// COMP/ELEC/MECH 450/550
// Project 5
// Authors: Keren Zhou
// Date: 11/11/2017
//////////////////////////////////////

#include "SmoothCostPath.h"
#include <cmath>

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
                std::vector<base::State *> &states = path_.getStates();
                double a = path_.getSpaceInformation()->distance(states[index - 1], state);
                double b = path_.getSpaceInformation()->distance(state, states[index + 1]);
                double c = path_.getSpaceInformation()->distance(states[index - 1], states[index + 1]);
                double acosValue = (a * a + b * b - c * c) / (2.0 * a * b);
            	if (acosValue > -1.0 && acosValue < 1.0)
				{
					// the smoothness is actually the outside angle of the one we compute
					double angle = (M_PI - acos(acosValue));
					// and we normalize by the length of the segments
					double k = 2.0 * angle / (a + b);
					return k * k;
				}
				return 0;
            }
        }
    }
}
