#ifndef SMM_DEGREE_H
#define SMM_DEGREE_H
#include <map>
#include "../base/concept.h"
#include "../base/graph.h"

namespace smm {
	template <class Graph>
		void degree_centrality(Graph& graph, std::vector<double>& result)
		{
			SMM_CHECK_GREATER_EQUAL(result.size(), graph.size());

			typename Graph::vertex_iterator v_begin = graph.vertex_begin();
			typename Graph::vertex_iterator v_end = graph.vertex_end();
			typename Graph::vertex_iterator v_it;

			for (v_it = v_begin; v_it != v_end; ++v_it) {
				std::size_t degree = v_it->edges.size();
				result[v_it->vertex_id] = static_cast<double>(degree) / (graph.size() - 1);
			}
		}
}//smm

#endif
