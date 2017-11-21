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
        static double calculateSmoothness(double a, double b, double c)
        {
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


        double SmoothCostPath::computeCost(const base::State *from, const base::State *to)
        {
            return 0.0;
        }


        double SmoothCostPath::computeCost(const base::State *from, const base::State *mid, const base::State *to)
        {
            double a = si_->distance(from, mid);
            double b = si_->distance(mid, to);
            double c = si_->distance(from, to);
            return calculateSmoothness(a, b, c);
        }


        void SmoothCostPath::initCost()
        {
            std::vector<base::State *> &states = path_->getStates();
            costs_.resize(states.size());
            cost_ = 0;
            for (int i = 1; i < states.size() - 1; ++i)
            {
                double a = si_->distance(states[i - 1], states[i]);
                double b = si_->distance(states[i], states[i + 1]);
                double c = si_->distance(states[i - 1], states[i + 1]);
                costs_[i] = calculateSmoothness(a, b, c);
                cost_ += costs_[i];
            }
            cost_ += states.size() * LAMBDA_;
        }

        bool SmoothCostPath::updateCost(int index, base::State *&state)
        {
            if (index <= 0 || index >= path_->getStateCount() - 1)
            {
                return false;
            }
            else 
            {
                // Initial cost
                double prevTotal = 0.0;
                if (index - 1 > 0)
                {
                    prevTotal += costs_[index - 1];
                }
                prevTotal += costs_[index];
                if (index + 1 < path_->getStateCount() - 1)
                {
                    prevTotal += costs_[index + 1];
                }

                std::vector<base::State *> &states = path_->getStates();

                double total = 0.0;
                double a = 0.0, b = 0.0, c = 0.0;
                double cost1 = 0.0, cost2 = 0.0, cost3 = 0.0;
                if (index - 1 > 0)
                {
                    a = si_->distance(states[index - 2], states[index - 1]);
                    b = si_->distance(states[index - 1], states[index]);
                    c = si_->distance(states[index - 2], states[index]);
                    cost1 = calculateSmoothness(a, b, c);
                    total += cost1;
                }
                a = si_->distance(states[index - 1], state);
                b = si_->distance(state, states[index + 1]);
                c = si_->distance(states[index - 1], states[index + 1]);
                cost2 = calculateSmoothness(a, b, c);
                total += cost2;
                if (index + 1 < path_->getStateCount() - 1)
                {
                    a = si_->distance(states[index], states[index + 1]);
                    b = si_->distance(states[index + 1], states[index + 2]);
                    c = si_->distance(states[index], states[index + 2]);
                    cost3 = calculateSmoothness(a, b, c);
                    total += cost3;
                }

                if (total < prevTotal)
                {
                    std::swap(states[index], state);
                    cost_ += total - prevTotal;
                    costs_[index - 1] = cost1;
                    costs_[index] = cost2;
                    costs_[index + 1] = cost3;
                    return true;
                }
                else
                {
                    return false;
                }
            }
        }
    }
}
