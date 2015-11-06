#ifndef SMM_UTILS_H
#define SMM_UTILS_H
#include <iostream>
#include "graph.h"

namespace smm {
	template <class Graph>
		print_graph(Graph graph)
		{
			typename Graph::vertex_iterator v_begin = graph.vertex_begin();
			typename Graph::vertex_iterator v_end = graph.vertex_end();
			typename Graph::vertex_iterator v_it;

			std::cout << "start print " << std::endl;
			for (v_it = v_begin; v_it != v_end; ++v_it) {
				std::cout << "tag : "<< v_it->label << " links " << std::endl;
				typename Graph::vertex_edge_iterator veb = v_it->edges.begin();
				typename Graph::vertex_edge_iterator vee = v_it->edges.end();
				typename Graph::vertex_edge_iterator ve;

				for (ve = veb; ve != vee; ++ve) {
					std::cout << (*ve)->to_label << std::endl;
				}
			}
			std::cout << "end print " << std::endl;
			std::cout<<std::endl;
		}
}
#endif
