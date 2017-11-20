///////////////////////////////////////
// COMP/ELEC/MECH 450/550
// Project 5
// Authors: Dave Coleman, Keren Zhou
// Date: 11/11/2017
//////////////////////////////////////

#ifndef SRC_HYBRIDIZATION_OPTIMIZER_H_
#define SRC_HYBRIDIZATION_OPTIMIZER_H_

#include <ompl/geometric/SimpleSetup.h>
#include <ompl/geometric/PathSimplifier.h>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <iostream>
#include <set>

#include "Optimizer.h"

namespace ompl
{
	namespace geometric
	{

		class HybridizationOptimizer : public Optimizer
		{
			public:
				HybridizationOptimizer(std::shared_ptr<CostPath> costPath, base::SpaceInformationPtr si);

				const base::PathPtr &getHybridPath() const;

				void computeHybridPath();

				unsigned int recordPath(const base::PathPtr &pp, bool matchAcrossGaps);

				std::size_t pathCount() const;

				void matchPaths(const geometric::PathGeometric &p, const geometric::PathGeometric &q, double gapCost,
						std::vector<int> &indexP, std::vector<int> &indexQ) const;

				// RVO
				virtual PathGeometric optimizeSolution(const SimpleSetupPtr ss, double duration = 0.0);

				void clear();

				void print(std::ostream &out = std::cout) const;

				const std::string &getName() const;

			private:
				struct vertex_state_t
				{
					typedef boost::vertex_property_tag kind;
				};

				typedef boost::adjacency_list<
					boost::vecS, boost::vecS, boost::undirectedS,
					boost::property<vertex_state_t, base::State *,
					boost::property<boost::vertex_predecessor_t, unsigned long int,
					boost::property<boost::vertex_rank_t, unsigned long int>>>,
					boost::property<boost::edge_weight_t, double>>
						HGraph;

				typedef boost::graph_traits<HGraph>::vertex_descriptor Vertex;
				typedef boost::graph_traits<HGraph>::edge_descriptor Edge;

				struct PathInfo
				{
					PathInfo(const base::PathPtr &path)
						: path_(path), states_(static_cast<PathGeometric *>(path.get())->getStates()), length_(0.0)
					{
						vertices_.reserve(states_.size());
					}

					bool operator==(const PathInfo &other) const
					{
						return path_ == other.path_;
					}

					bool operator<(const PathInfo &other) const
					{
						return path_ < other.path_;
					}

					base::PathPtr path_;
					const std::vector<base::State *> &states_;
					double length_;
					std::vector<Vertex> vertices_;
				};

				void attemptNewEdge(const PathInfo &p, const PathInfo &q, int indexP, int indexQ);

				base::SpaceInformationPtr si_;
				HGraph g_;
				boost::property_map<HGraph, vertex_state_t>::type stateProperty_;
				Vertex root_;
				Vertex goal_;
				std::set<PathInfo> paths_;
				base::PathPtr hpath_;

				std::string name_;
		};
	}  // namespace tools
}  // namespace ompl

#endif
