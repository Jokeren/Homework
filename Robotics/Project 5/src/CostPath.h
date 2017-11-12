///////////////////////////////////////
// COMP/ELEC/MECH 450/550
// Project 5
// Authors: Keren Zhou
// Date: 11/11/2017
//////////////////////////////////////

#ifndef SRC_PATH_COST_H_
#define SRC_PATH_COST_H_

#include <ompl/geometric/PathGeometric.h>

namespace ompl
{
    namespace geometric
    {
        // A wrapper of PathGeometric, do not use inheritation
        class CostPath
        {
        public:
            explicit CostPath(const base::SpaceInformationPtr &si) : path_(si), cost_(0.0) {}

            explicit CostPath(const PathGeometric &path) : path_(path), cost_(0.0) {}

            // Copy path
            void setPath(const PathGeometric &path)
            {
                this->path_ = path;
            }

            const PathGeometric &getPath()
            {
                return this->path_;
            }

            double getCost()
            {
                return this->cost_;
            }

            size_t getStateCount()
            {
                return this->path_.getStateCount();
            }

            void interpolate()
            {
                this->path_.interpolate();
            }

            base::State *getState(size_t index)
            {
                return this->path_.getState(index);
            }

            // Compute cost
            virtual double cost(size_t index, base::State *state) = 0;

            void updateCost(size_t index, base::State *state)
            {
                std::vector<base::State *> &states = path_.getStates();
                double oldCost = cost(index, states[index]);
                double newCost = cost(index, state);
                // Special operation, save space
                std::swap(states[index], state);
                cost_ += newCost - oldCost;
            }

            // Init cost before using CostPath
            void initCost()
            {
                std::vector<base::State *> &states = path_.getStates();
                for (size_t i = 0; i < states.size(); ++i)
                {
                    cost_ += this->cost(i, states[i]);
                }
            }

        protected:
            PathGeometric path_;
            double cost_;
        };
    }  // namespace tools
}  // namespace ompl

#endif

