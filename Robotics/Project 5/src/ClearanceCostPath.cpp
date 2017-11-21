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
        double ClearanceCostPath::computeCost(const base::State *from, const base::State *to)
        {
            return 1 / si_->getStateValidityChecker()->clearance(to);
        }

        double ClearanceCostPath::computeCost(const base::State *from, const base::State *mid, const base::State *to)
        {
            return 0.0;
        }

        void ClearanceCostPath::initCost()
        {
            std::vector<base::State *> &states = path_->getStates();
            costs_.resize(states.size());
            cost_ = 0;
            for (int i = 0; i < states.size(); ++i)
            {
                costs_[i] = 1 / si_->getStateValidityChecker()->clearance(states[i]);
                cost_ += costs_[i];
            }
            if (states.empty())
            {
                cost_ = 0;
            }
        }

        bool ClearanceCostPath::updateCost(int index, base::State *&state)
        {
            std::vector<base::State *> &states = path_->getStates();
            double cost = 1 / si_->getStateValidityChecker()->clearance(state);
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
