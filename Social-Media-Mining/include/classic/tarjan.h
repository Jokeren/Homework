#ifndef SMM_TARJAN_H
#define SMM_TARJAN_H
#include "../base/concept.h"
#include "../base/graph.h"

namespace smm {
	template <class Graph>
		void tarjan(Graph& graph, std::size_t from_id, std::vector<std::size_t>& pre, 
				std::vector<std::size_t>& low, std::vector<std::size_t>& cut)
		{
			SMM_CHECK_GREATER_EQUAL(pre.size(), graph.size());
			SMM_CHECK_GREATER_EQUAL(low.size(), graph.size());
			SMM_CHECK_GREATER_EQUAL(cut.size(), graph.size());

			tarjan_impl(graph, from_id, from_id, pre, low, cut, true, 0);
		}

	//TODO: implement by stack
	template <class Graph>
		std::size_t tarjan_impl(Graph& graph, std::size_t from_id, std::size_t father, std::vector<std::size_t>& pre, 
				std::vector<std::size_t>& low, std::vector<std::size_t>& cut, bool is_first, 
				std::size_t dfs_clock)
		{
			std::size_t lowu = pre[from_id] = ++dfs_clock;	

			typename Graph::vertex_edge_iterator veb = graph.vertex_edge_begin(from_id);
			typename Graph::vertex_edge_iterator vee = graph.vertex_edge_end(from_id);
			typename Graph::vertex_edge_iterator ve;

			std::size_t nchild = 0;
			for (ve = veb; ve != vee; ++ve) {
				std::size_t to_id = (*ve)->to_id;

				if (pre[to_id] == 0) {
					++nchild;
					std::size_t lowv = tarjan_impl(graph, to_id, from_id, pre, low, 
							cut, false, dfs_clock);
					lowu = std::min(lowv, lowu);
					
					if (lowv >= pre[from_id]) {
						cut[from_id]++;
					}
				} else if (pre[to_id] < pre[from_id] && to_id != father) {
					lowu = std::min(lowu, pre[to_id]);
				}
			}

			if (is_first && nchild == 1) {
				cut[from_id] = 0;
			}

			low[from_id] = lowu;
			return lowu;
		}	
}//smm
#endif
