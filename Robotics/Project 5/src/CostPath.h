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
                explicit CostPath(const base::SpaceInformationPtr si) : si_(si), cost_(0.0) {}

                // Copy path
                void setPath(std::shared_ptr<PathGeometric> path)
                {
                    this->path_ = path;
                }

                const std::shared_ptr<PathGeometric> getPath()
                {
                    return this->path_;
                }

                double getCost()
                {
                    return this->cost_;
                }


                int getStateCount()
                {
                    return this->path_->getStateCount();
                }

                void interpolate(size_t count)
                {
                    this->path_->interpolate(count);
                }

                base::State *getState(int index)
                {
                    return this->path_->getState(index);
                }

                std::vector<base::State *> &getStates()
                {
                    return this->path_->getStates();
                }

                base::SpaceInformationPtr getSpaceInformation()
                {
                    return this->si_;
                }

                // Compute cost
                virtual double computeCost(const base::State *from, const base::State *to) = 0;

                virtual double computeCost(const base::State *from, const base::State *mid, const base::State *to) = 0;

                virtual bool updateCost(int index, base::State *&state) = 0;

                // Init cost before using CostPath
                virtual void initCost() = 0;

            protected:
                base::SpaceInformationPtr si_;
                std::shared_ptr<PathGeometric> path_;
                std::vector<double> costs_;
                double cost_;
        };
    }  // namespace tools
}  // namespace ompl

#endif

