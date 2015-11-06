#ifndef SMM_BETWEENESS_H
#define SMM_BETWEENESS_H
#include <map>
#include "../base/concept.h"
#include "../base/graph.h"
#include "../classic/dijkstra.h"

namespace smm {
	template <class Graph>
		void betweeness_centrality(Graph& graph, std::vector<double>& result)
		{
			SMM_CHECK_GREATER_EQUAL(result.size(), graph.size());

			for (std::size_t i = 0; i < graph.size(); ++i) {
				std::stack<std::size_t> nodes;
				std::vector<std::vector<std::size_t> > paths(graph.size());
				std::map<std::size_t, double> dist;
				std::vector<double> sigma(graph.size());

				single_source_dijkstra_full(graph, i, nodes, paths, dist, sigma);

				//accumulate
				std::vector<double> delta(graph.size());
				while (!nodes.empty()) {
					std::size_t u = nodes.top();
					nodes.pop();

					double coeff = (1.0f + delta[u]) / sigma[u];
					//XXX
//					std::cout << "node : " << graph.get_vertex_name(u) << std::endl;
					for (std::size_t j = 0; j < paths[u].size(); ++j) {
						std::size_t v = paths[u][j];
//						std::cout << "prev " << graph.get_vertex_name(v) << std::endl;
						delta[v] += sigma[v] * coeff;
					}
					
					if (u != i) {
						result[u] += delta[u];
					}
				}
			}

			if (result.size() <= 2)
				return;

			double scale = 1.0f / ((graph.size() - 1) * (graph.size() - 2));

			//TODO: lambda
			for (std::size_t i = 0; i < result.size(); ++i) {
				result[i] *= scale;
			}
		}
}//smm

#endif
