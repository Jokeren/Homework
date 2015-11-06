#ifndef SMM_CLOSENESS_H
#define SMM_CLOSENESS_H
#include "../base/concept.h"
#include "../base/graph.h"
#include "../classic/dijkstra.h"

namespace smm {
	template <class Graph>
		void closeness_centrality(Graph& graph, std::vector<double>& result)
		{
			SMM_CHECK_GREATER_EQUAL(result.size(), graph.size());

			for (std::size_t i = 0; i < graph.size(); ++i) {
				std::stack<std::size_t> nodes;
				std::vector<std::vector<std::size_t> > paths(graph.size());
				std::map<std::size_t, double> dist;
				std::vector<double> sigma(graph.size());

				single_source_dijkstra_full(graph, i, nodes, paths, dist, sigma);
				
				//sum up
				double sum = 0.0f;
				for (std::map<std::size_t, double>::iterator it = dist.begin(); 
						it != dist.end(); ++it) {
					sum += it->second;
				}		

				//typical scaling
				double scale = 1.0f / (graph.size() - 1);
				result[i] = 1 / (sum * scale);
			}
		}
}//smm

#endif
