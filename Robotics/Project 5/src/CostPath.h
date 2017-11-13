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

            int getStateCount()
            {
                return this->path_.getStateCount();
            }

            void interpolate(size_t count)
            {
                this->path_.interpolate(count);
            }

            base::State *getState(int index)
            {
                return this->path_.getState(index);
            }

            // Compute cost
            virtual bool updateCost(int index, base::State *&state) = 0;

            // Init cost before using CostPath
            virtual void initCost() = 0;

        protected:
            PathGeometric path_;
            std::vector<double> costs_;
            double cost_;
        };
    }  // namespace tools
}  // namespace ompl

#endif

