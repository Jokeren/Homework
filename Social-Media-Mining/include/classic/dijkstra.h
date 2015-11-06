#ifndef SMM_DIJKSTRA_H
#define SMM_DIJKSTRA_H
#include <stack>
#include <set>
#include "../base/concept.h"
#include "../base/graph.h"

namespace smm {
	//internal struct
	struct heap_node_t {
		std::size_t node_id;
		std::size_t prev_id;
		double dist;

		heap_node_t() {};

		heap_node_t(std::size_t _node_id, std::size_t _prev_id, double _dist) : 
			node_id(_node_id), prev_id(_prev_id), dist(_dist) {};

		bool operator < (const struct heap_node_t& node) const 
		{
			return this->dist > node.dist;
		}	
	};

	//TODO:hash_map
	template <class Graph>
		void single_source_dijkstra_full(Graph& graph, std::size_t from_id, std::stack<std::size_t>& nodes, 
				std::vector<std::vector<std::size_t> >& paths, std::map<std::size_t, double>& dist,
				std::vector<double>& sigma)
		{
			//check
			SMM_CHECK_GREATER_EQUAL(paths.size(), graph.size());	
			SMM_CHECK_GREATER_EQUAL(sigma.size(), graph.size());	
			SMM_CHECK_LESS(from_id, graph.size());

			sigma[from_id] = 1.0f;

			std::priority_queue<struct heap_node_t> pq;	
			pq.push(heap_node_t(from_id, from_id, 0.0f)); //tmp variable in the stack

			std::set<std::size_t> seen;
			while (!pq.empty()) {
				struct heap_node_t node = pq.top(); 

				pq.pop();

				std::size_t u = node.node_id;
				if (seen.find(u) != seen.end())//already in
					continue;

				//insert back
				nodes.push(u);
				sigma[u] += sigma[node.prev_id];
				seen.insert(u);
				dist[u] = node.dist;

				typename Graph::vertex_edge_iterator veb = graph.vertex_edge_begin(u);
				typename Graph::vertex_edge_iterator vee = graph.vertex_edge_end(u);
				typename Graph::vertex_edge_iterator ve;
				for (ve = veb; ve != vee; ++ve) {
					std::size_t v = (*ve)->to_id;
					double uv_dist = node.dist + (graph.is_weighted() ? 
							(*ve)->weight : 1.0f);

					if (seen.find(v) == seen.end() && 
							(dist.find(v) == dist.end() || uv_dist < dist[v])) {
						dist[v] = uv_dist;

						//tmp variable on the stack
						pq.push(heap_node_t(v, u, uv_dist));	

						sigma[v] = 0.0f;
						paths[v].clear();
						paths[v].push_back(u);
					} else if (uv_dist == dist[v]) {//equal path
						sigma[v] += sigma[u];
						paths[v].push_back(u);
					}
				}
			}
		}
}//smm

#endif
