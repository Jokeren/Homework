///////////////////////////////////////
// COMP/ELEC/MECH 450/550
// Project 5
// Authors: Keren Zhou
// Date: 11/11/2017
//////////////////////////////////////

#include "ClearanceCostPath.h"
#include <cmath>

namespace ompl
{
    namespace geometric
    {
        void ClearanceCostPath::initCost()
        {
            std::vector<base::State *> &states = path_.getStates();
            costs_.resize(states.size());
            cost_ = 0;
            for (int i = 0; i < states.size(); ++i)
            {
                // do whatever you want
                // 
                // code
                costs_[i] = 0;
                cost_ += costs_[i];
            }
        }

        bool ClearanceCostPath::updateCost(int index, base::State *&state)
        {
        }
    }
}
