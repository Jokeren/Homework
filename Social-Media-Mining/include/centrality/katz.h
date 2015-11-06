#ifndef SMM_KATZ_H
#define SMM_KATZ_H
#include <cmath>
#include "../base/concept.h"
#include "../base/graph.h"

namespace smm {
	template<class Graph>
		void pagerank_centrality(Graph& graph, std::vector<double>& result, double alpha,
				double beta, double epsilion = 0.005f)
		{
			SMM_CHECK_GREATER_EQUAL(result.size(), graph.size());

			katz_centrality_impl(graph, result, alpha, beta, epsilion, true);
		}

	template<class Graph>
		void katz_centrality(Graph& graph, std::vector<double>& result, double alpha,
				double beta, double epsilion = 0.005f)
		{
			SMM_CHECK_GREATER_EQUAL(result.size(), graph.size());

			katz_centrality_impl(graph, result, alpha, beta, epsilion, false);
		}

	template<class Graph>
		void katz_centrality_impl(Graph& graph, std::vector<double>& result, double alpha,
				double beta, double epsilion, bool has_degree)
		{
			unsigned int n_steps = 0;
			std::vector<double> r(graph.size());
			std::vector<double> pr(graph.size(), 1.0f / graph.size());
			double diff = 0.0f;

			typename Graph::vertex_iterator v_begin = graph.vertex_begin();
			typename Graph::vertex_iterator v_end = graph.vertex_end();
			do {
				typename Graph::vertex_iterator v_it;

				for (v_it = v_begin; v_it != v_end; ++v_it) {
					typename Graph::vertex_edge_iterator veb = v_it->edges.begin();
					typename Graph::vertex_edge_iterator vee = v_it->edges.end();
					typename Graph::vertex_edge_iterator ve;
					size_t v_id = v_it->vertex_id;

					for (ve = veb; ve != vee; ++ve) {
						size_t to_id = (*ve)->to_id;
						size_t degree = has_degree ? (v_begin + to_id)->edges.size() : 1;

						r[v_id] += alpha * pr[to_id] / degree;
					}
					r[v_id] += beta;
				}

				diff = 0.0f;
				for (size_t i = 0; i < graph.size(); ++i) {
					diff += fabs(r[i] - pr[i]);
					pr[i] = r[i];
					r[i] = 0.0f;
				}
				//XXX
//				std::cout << "diff " <<  diff << std::endl;

				++n_steps;
				//XXX
//				std::cout << "n_steps " << n_steps << std::endl;
			}while (diff > epsilion);

			//copy back
			std::copy(pr.begin(), pr.end(), result.begin());
		}
}//smm

#endif
