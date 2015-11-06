#include <iostream>
#include <vector>
#include "../include/base/reader_graph.h"
#include "../include/base/graph.h"
#include "../include/centrality/betweeness.h"
#include "../include/centrality/closeness.h"
#include "../include/centrality/degree.h"
#include "../include/centrality/eigen.h"
#include "../include/centrality/katz.h"

using namespace smm;

int main(int argc, char *argv[])
{
	GraphReader<int> reader;
	reader.open(argv[1]);

	Graph<int> graph = reader.get_instance();

	std::vector<double> result(graph.size(), 0);

	//degree
	degree_centrality(graph, result);
	for (size_t i = 0; i < result.size(); ++i) {
		std::cout<< "degree "<< graph.get_vertex_name(i) << ":" << result[i] << std::endl;
	}
	result.assign(result.size(), 0);

	//eigenvector
	eigen_centrality(graph, result);
	for (size_t i = 0; i < result.size(); ++i) {
		std::cout<< "eigen "<< graph.get_vertex_name(i) << ":" << result[i] << std::endl;
	}
	result.assign(result.size(), 0);
	
	//katz
	katz_centrality(graph, result, 0.3, 0.3);
	for (size_t i = 0; i < result.size(); ++i) {
		std::cout<< "katz "<< graph.get_vertex_name(i) << ":" << result[i] << std::endl;
	}
	result.assign(result.size(), 0);

	//pagerank
	pagerank_centrality(graph, result, 0.3, 0.3);
	for (size_t i = 0; i < result.size(); ++i) {
		std::cout<< "pagerank "<< graph.get_vertex_name(i) << ":" << result[i] << std::endl;
	}
	result.assign(result.size(), 0);
	

	//betweeness
	betweeness_centrality(graph, result);
	for (size_t i = 0; i < result.size(); ++i) {
		std::cout<< "betweeness "<< graph.get_vertex_name(i) << ":" << result[i] << std::endl;
	}
	result.assign(result.size(), 0);

	//closeness
	closeness_centrality(graph, result);
	for (size_t i = 0; i < result.size(); ++i) {
		std::cout<< "closeness "<< graph.get_vertex_name(i) << ":" << result[i] << std::endl;
	}
	result.assign(result.size(), 0);

	return 0;
}
