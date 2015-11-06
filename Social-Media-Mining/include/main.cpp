#include <iostream>
#include <vector>
#include "./base/reader_graph.h"
#include "./base/graph.h"
#include "./centrality/eigen.h"
int main(int argc, char *argv[])
{
	{
		using namespace smm;
		GraphReader<double> gr;
		gr.open(argv[1]);
		Graph<double> graph(gr.get_instance());
		std::vector<double> result(graph.size(), 0);
		eigen_centrality(graph, result);
		for (int i = 0; i < result.size(); ++i) {
			std::cout << result[i] << "  " << std::endl;
		}
		return 0;
	}		
}
