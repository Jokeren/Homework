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
            std::vector<base::State *> &states = path_->getStates();
            costs_.resize(states.size());
            cost_ = 0;
            for (int i = 0; i < states.size(); ++i)
            {
                costs_[i] = -si_->getStateValidityChecker()->clearance(states[i]);
                cost_ += costs_[i];
            }
            if (states.empty())
            {
                cost_ = -std::numeric_limits<double>::infinity();
            }
        }

        bool ClearanceCostPath::updateCost(int index, base::State *&state)
        {
            std::vector<base::State *> &states = path_->getStates();
            double cost = -si_->getStateValidityChecker()->clearance(state);
            if (cost < costs_[index])
            {
                std::swap(states[index], state);
                cost_ += cost - costs_[index];
                costs_[index] = cost;
                return true;
            }
            return false;
        }
    }
}
