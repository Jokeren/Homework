///////////////////////////////////////
// COMP/ELEC/MECH 450/550
// Project 5
// Authors: Dave Coleman, Keren Zhou
// Date: 11/11/2017
//////////////////////////////////////

#include "HybridizationOptimizer.h"

#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <utility>

namespace ompl
{
	namespace magic
	{
		static const double GAP_COST_FRACTION = 0.05;
	}

	namespace geometric
	{
		PathGeometric HybridizationOptimizer::optimizeSolution(const SimpleSetupPtr sp, double duration)
		{
			PathGeometric &initPath = sp->getSolutionPath();
			PathGeometric bestPath(initPath);
			double initCost = 0.0, bestCost = 0.0;
			size_t initState = initPath.getStateCount();
			OMPL_INFORM("init %d points", initState);
			time::point start = time::now();
			if (costPath_)
			{
                std::shared_ptr<PathGeometric> pathPtr(new PathGeometric(initPath));
                costPath_->setPath(pathPtr);
				costPath_->initCost();
				initCost = costPath_->getCost();
				bestCost = costPath_->getCost();
                computeHybridPath();
                costPath_->setPath(hpath_); 
                costPath_->initCost();
                if (bestCost > costPath_->getCost())
                {
                    bestCost = costPath_->getCost();
                    bestPath = *hpath_;
                }
			}
			else
			{
				OMPL_WARN("costPath has not been setup");
				return bestPath;
			}
			double optimizeTime = time::seconds(time::now() - start);
			OMPL_INFORM("HybridizationSetup: Path optimization took %f seconds and changed from %d to %d states, from %f to %f cost",
					optimizeTime, initState, initPath.getStateCount(), initCost, bestCost);
			return bestPath;
		}


		HybridizationOptimizer::HybridizationOptimizer(std::shared_ptr<CostPath> costPath, base::SpaceInformationPtr si)
			: Optimizer(costPath), si_(std::move(si)), stateProperty_(boost::get(vertex_state_t(), g_)), name_("PathHybridization")
		{
			root_ = boost::add_vertex(g_);
			stateProperty_[root_] = nullptr;
			goal_ = boost::add_vertex(g_);
			stateProperty_[goal_] = nullptr;
		}

		void HybridizationOptimizer::clear()
		{
			hpath_.reset();
			paths_.clear();

			g_.clear();
			root_ = boost::add_vertex(g_);
			stateProperty_[root_] = nullptr;
			goal_ = boost::add_vertex(g_);
			stateProperty_[goal_] = nullptr;
		}

		void HybridizationOptimizer::print(std::ostream &out) const
		{
			out << "Path hybridization is aware of " << paths_.size() << " paths" << std::endl;
			int i = 1;
			for (auto it = paths_.begin(); it != paths_.end(); ++it, ++i)
				out << "  path " << i << " of length " << it->length_ << std::endl;
			if (hpath_)
				out << "Hybridized path of length " << hpath_->length() << std::endl;
		}

		const std::string &HybridizationOptimizer::getName() const
		{
			return name_;
		}

		void HybridizationOptimizer::computeHybridPath()
		{
			//boost::vector_property_map<Vertex> prev(boost::num_vertices(g_));
			//boost::dijkstra_shortest_paths(g_, root_, boost::predecessor_map(prev));
			//if (prev[goal_] != goal_)
			//{
			//	auto h(std::make_shared<PathGeometric>(si_));
			//	for (Vertex pos = prev[goal_]; prev[pos] != pos; pos = prev[pos])
			//		h->append(stateProperty_[pos]);
			//	h->reverse();
			//	hpath_ = h;
			//}
            size_t nvertices = boost::num_vertices(g_);
            double *dis = (double *)malloc(nvertices * nvertices * sizeof(double));
            size_t *next = (size_t *)malloc(nvertices * nvertices * sizeof(size_t));

            for (Vertex u = 0; u < boost::num_vertices(g_); ++u)
            {
                for (Vertex v = 0; v < boost::num_vertices(g_); ++v)
                {
                    std::pair<Edge, bool> p = boost::edge(u, v, g_);
                    if (u == v)
                    {
                        dis[u * nvertices + v] = 0;
                    }
                    else if (p.second == true)
                    {
                        base::State *stateU = stateProperty_[u];
                        base::State *stateV = stateProperty_[v];
                        if (stateU == NULL || stateV == NULL)
                        {
                            dis[u * nvertices + v] = 0;
                        }
                        else
                        {
                            double cost = costPath_->computeCost(stateU, stateV);
                            dis[u * nvertices + v] = cost;
                        }
                        next[u * nvertices + v] = v;
                    }
                    else
                    {
                        dis[u * nvertices + v] = std::numeric_limits<double>::max() / 2 - 1;
                    }
                }
            }

            for (Vertex i = 0; i < nvertices; ++i)
            {
                for (Vertex u = 0; u < nvertices; ++u)
                {
                    for (Vertex v = 0; v < nvertices; ++v)
                    {
                        if (u == v)
                            continue;

                        base::State *stateU = stateProperty_[u];
                        base::State *stateV = stateProperty_[v];
                        base::State *stateI = stateProperty_[i];

                        if (dis[u * nvertices + i] != std::numeric_limits<double>::max() && dis[i * nvertices + v] != std::numeric_limits<double>::max())
                        {
                            double angleCost = 0.0;
                            if (stateU != NULL && stateV != NULL && stateI != NULL)
                            {
                                angleCost = costPath_->computeCost(stateU, stateI, stateV);
                            }
                            if (dis[u * nvertices + v] > dis[u * nvertices + i] + dis[i * nvertices + v] + angleCost)
                            {
                                dis[u * nvertices + v] = dis[u * nvertices + i] + dis[i * nvertices + v] + angleCost;
                                next[u * nvertices + v] = next[u * nvertices + i];
                            } 
                        }
                    }
                }
            }
			auto h(std::make_shared<PathGeometric>(costPath_->getSpaceInformation()));
            Vertex pos = next[root_ * nvertices + goal_];
            while (pos != goal_)
            {
                h->append(stateProperty_[pos]);
                pos = next[pos * nvertices + goal_];
            }
            hpath_ = h;
            free(next);
            free(dis);
		}

		const ompl::base::PathPtr &HybridizationOptimizer::getHybridPath() const
		{
			return hpath_;
		}

		unsigned int HybridizationOptimizer::recordPath(const std::shared_ptr<PathGeometric> pp, bool matchAcrossGaps)
		{
			auto *p = pp.get();
			if (!p)
			{
				OMPL_ERROR("Path hybridization only works for geometric paths");
				return 0;
			}

			if (p->getSpaceInformation() != si_)
			{
				OMPL_ERROR("Paths for hybridization must be from the same space information");
				return 0;
			}

			// skip empty paths
			if (p->getStateCount() == 0)
				return 0;

			PathInfo pi(pp);

			// if this path was previously included in the hybridization, skip it
			if (paths_.find(pi) != paths_.end())
				return 0;

			// the number of connection attempts
			unsigned int nattempts = 0;

			// start from virtual root
			Vertex v0 = boost::add_vertex(g_);
			stateProperty_[v0] = pi.states_[0];
			pi.vertices_.push_back(v0);

			// add all the vertices of the path, and the edges between them, to the HGraph
			// also compute the path length for future use (just for computational savings)
			const HGraph::edge_property_type prop0(0.0);
			boost::add_edge(root_, v0, prop0, g_);
			double length = 0.0;
			for (std::size_t j = 1; j < pi.states_.size(); ++j)
			{
				Vertex v1 = boost::add_vertex(g_);
				stateProperty_[v1] = pi.states_[j];
				double weight = si_->distance(pi.states_[j - 1], pi.states_[j]);
				const HGraph::edge_property_type properties(weight);
				boost::add_edge(v0, v1, properties, g_);
				length += weight;
				pi.vertices_.push_back(v1);
				v0 = v1;
			}

			// connect to virtual goal
			boost::add_edge(v0, goal_, prop0, g_);
			pi.length_ = length;

			// find matches with previously added paths
			for (const auto &path : paths_)
			{
				const auto *q = static_cast<const PathGeometric *>(path.path_.get());
				std::vector<int> indexP, indexQ;
				matchPaths(*p, *q, (pi.length_ + path.length_) / (2.0 / magic::GAP_COST_FRACTION), indexP, indexQ);
				if (matchAcrossGaps)
				{
					int lastP = -1;
					int lastQ = -1;
					int gapStartP = -1;
					int gapStartQ = -1;
					bool gapP = false;
					bool gapQ = false;
					for (std::size_t i = 0; i < indexP.size(); ++i)
					{
						// a gap is found in p
						if (indexP[i] < 0)
						{
							// remember this as the beginning of the gap, if needed
							if (!gapP)
								gapStartP = i;
							// mark the fact we are now in a gap on p
							gapP = true;
						}
						else
						{
							// check if a gap just ended;
							// if it did, try to match the endpoint with the elements in q
							if (gapP)
								for (std::size_t j = gapStartP; j < i; ++j)
								{
									attemptNewEdge(pi, path, indexP[i], indexQ[j]);
									++nattempts;
								}
							// remember the last non-negative index in p
							lastP = i;
							gapP = false;
						}
						if (indexQ[i] < 0)
						{
							if (!gapQ)
								gapStartQ = i;
							gapQ = true;
						}
						else
						{
							if (gapQ)
								for (std::size_t j = gapStartQ; j < i; ++j)
								{
									attemptNewEdge(pi, path, indexP[j], indexQ[i]);
									++nattempts;
								}
							lastQ = i;
							gapQ = false;
						}

						// try to match corresponding index values and gep beginnings
						if (lastP >= 0 && lastQ >= 0)
						{
							attemptNewEdge(pi, path, indexP[lastP], indexQ[lastQ]);
							++nattempts;
						}
					}
				}
				else
				{
					// attempt new edge only when states align
					for (std::size_t i = 0; i < indexP.size(); ++i)
						if (indexP[i] >= 0 && indexQ[i] >= 0)
						{
							attemptNewEdge(pi, path, indexP[i], indexQ[i]);
							++nattempts;
						}
				}
			}

			// remember this path is part of the hybridization
			paths_.insert(pi);
			return nattempts;
		}

		void HybridizationOptimizer::attemptNewEdge(const PathInfo &p, const PathInfo &q, int indexP, int indexQ)
		{
			if (si_->checkMotion(p.states_[indexP], q.states_[indexQ]))
			{
				double weight = si_->distance(p.states_[indexP], q.states_[indexQ]);
				const HGraph::edge_property_type properties(weight);
				boost::add_edge(p.vertices_[indexP], q.vertices_[indexQ], properties, g_);
			}
		}

		std::size_t HybridizationOptimizer::pathCount() const
		{
			return paths_.size();
		}

		void HybridizationOptimizer::matchPaths(const PathGeometric &p, const PathGeometric &q, double gapCost,
				std::vector<int> &indexP, std::vector<int> &indexQ) const
		{
			std::vector<std::vector<double>> C(p.getStateCount());
			std::vector<std::vector<char>> T(p.getStateCount());

			for (std::size_t i = 0; i < p.getStateCount(); ++i)
			{
				C[i].resize(q.getStateCount(), 0.0);
				T[i].resize(q.getStateCount(), '\0');
				for (std::size_t j = 0; j < q.getStateCount(); ++j)
				{
					// as far as I can tell, there is a bug in the algorithm as presented in the paper
					// so I am doing things slightly differently ...
					double match = si_->distance(p.getState(i), q.getState(j)) + ((i > 0 && j > 0) ? C[i - 1][j - 1] : 0.0);
					double up = gapCost + (i > 0 ? C[i - 1][j] : 0.0);
					double left = gapCost + (j > 0 ? C[i][j - 1] : 0.0);
					if (match <= up && match <= left)
					{
						C[i][j] = match;
						T[i][j] = 'm';
					}
					else if (up <= match && up <= left)
					{
						C[i][j] = up;
						T[i][j] = 'u';
					}
					else
					{
						C[i][j] = left;
						T[i][j] = 'l';
					}
				}
			}
			// construct the sequences with gaps (only index positions)
			int m = p.getStateCount() - 1;
			int n = q.getStateCount() - 1;

			indexP.clear();
			indexQ.clear();
			indexP.reserve(std::max(n, m));
			indexQ.reserve(indexP.size());

			while (n >= 0 && m >= 0)
			{
				if (T[m][n] == 'm')
				{
					indexP.push_back(m);
					indexQ.push_back(n);
					--m;
					--n;
				}
				else if (T[m][n] == 'u')
				{
					indexP.push_back(m);
					indexQ.push_back(-1);
					--m;
				}
				else
				{
					indexP.push_back(-1);
					indexQ.push_back(n);
					--n;
				}
			}
			while (n >= 0)
			{
				indexP.push_back(-1);
				indexQ.push_back(n);
				--n;
			}
			while (m >= 0)
			{
				indexP.push_back(m);
				indexQ.push_back(-1);
				--m;
			}
		}

	}  // namespace tools
}  // namespace ompl
