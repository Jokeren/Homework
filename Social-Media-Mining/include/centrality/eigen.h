#ifndef SMM_EIGEN_CENTRAL_H
#define SMM_EIGEN_CENTRAL_H

#include <cmath>
#include <iostream>
#include "../base/concept.h"
#include "../base/graph.h"

namespace smm {

	/*
	 * test
	 */
	template <class Graph>
		void update_vector(Graph& graph, std::vector<double>& m_vec)
		{
			SMM_CHECK_GREATER_EQUAL(graph.size(), m_vec.size());
			typename Graph::vertex_iterator v_begin = graph.vertex_begin();
			typename Graph::vertex_iterator v_end = graph.vertex_end();
			typename Graph::vertex_iterator v_it;
			std::vector<double> new_vec(m_vec.size(),0);
		
			for (v_it = v_begin; v_it != v_end; ++v_it) {
				typename Graph::vertex_edge_iterator veb = v_it->edges.begin();
				typename Graph::vertex_edge_iterator vee = v_it->edges.end();
				typename Graph::vertex_edge_iterator ve;
				size_t v_id = v_it->vertex_id;
				
				for (ve = veb; ve != vee; ++ve) {
					size_t to_id = (*ve)->to_id;
					new_vec[to_id] += m_vec[v_id];
				}
			}
			m_vec = new_vec;
			return;
		}
		
		void normlize_vector(std::vector<double>& m_vec)
		{
			double sum = 0;
			for (int i = 0; i < m_vec.size(); ++i) {
				sum += m_vec[i]*m_vec[i];
			}
			sum = pow(sum, 0.5);
			if (sum == 0) {
				std::cerr << "error: zero vector input" << std::endl;
				return;
			}
			for (int i = 0; i < m_vec.size(); ++i) {
				m_vec[i] /= sum;
			}
			return;
		}
		
	template <class Graph>
		void eigen_centrality(Graph& graph, std::vector<double>& result)
		{
			SMM_CHECK_GREATER_EQUAL(result.size(), graph.size());
			
			std::vector<double> old_vector(graph.size(), 1.0/graph.size());
			normlize_vector(old_vector);
			std::vector<double> new_vector(old_vector);
			double delta = 1.0f;
			double eps = 0.0f;
			
			while (delta > eps) {
				update_vector(graph, new_vector);
				update_vector(graph, new_vector);
				normlize_vector(new_vector);
				delta = 0.0f;
				for (int i = 0; i < old_vector.size(); ++i) {
					delta += fabs(new_vector[i] - old_vector[i]);
				}
				old_vector = new_vector;
			}
			
			update_vector(graph, new_vector);
			normlize_vector(new_vector);
			for (int i = 0; i < result.size(); ++i) {
				result[i] = (new_vector[i] + old_vector[i]) / 2;
			}
			
			normlize_vector(result);
			return;
		}
}//smm

#endif 
//another update test
