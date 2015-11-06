#include <iostream>
#include <vector>
#include "../include/base/reader_graph.h"
#include "../include/base/graph.h"
#include "../include/classic/tarjan.h"


using namespace smm;

int main(int argc, char *argv[])
{
	GraphReader<int> reader;
	reader.open(argv[1]);

	Graph<int> graph = reader.get_instance();

	std::vector<size_t> pre(graph.size(), 0);
	std::vector<size_t> low(graph.size(), 0);
	std::vector<size_t> cut(graph.size(), 0);

	//degree
	tarjan(graph, 0, pre, low, cut);
	int nbridges = 0;
	for (size_t i = 0; i < pre.size(); ++i) {
		if (pre[i] == low[i] && i != 0) {
			++nbridges;
		}
	}
	std::cout<< "nbridges : "<< nbridges << std::endl;

	return 0;
}
