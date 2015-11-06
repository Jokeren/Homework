#include <vector>
#include <queue>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/time.h>

static const int GMAX = 0x3f3f3f3f;
static const int N = 20100;

struct edge_t {
    int from, to, cost;
};

struct heap_node {
	int v, u;
	bool operator < (const struct heap_node& node) const {
		return v > node.v;
	}
};

bool vertices[N];

struct t_dijkstra {
	int nvertice, m;
	std::vector<struct edge_t> edges;
	std::vector<int> g[N];
	std::vector<int> father[N];
	bool visit[N];
	int dist[N];
	int ncount[N];

	void init(int n) 
	{
	    nvertice = n;
		for (int i = 0; i <= nvertice; i++) {
			g[i].clear();
		}
		edges.clear();
		memset(visit, false, sizeof(visit));
		for (int i = 0; i <= nvertice; i++) {
			dist[i] = GMAX;
		}
		memset(ncount, 0, sizeof(ncount));
	}

	void add_edge(int from, int to, int cost)
   	{
		edges.push_back((struct edge_t){from, to, cost});
		m = edges.size();
		g[from].push_back(m - 1);
	}

	void push_father(int u, int to, int calc_dist) 
	{
		if (dist[to] != GMAX && dist[to] == calc_dist) {
			father[to].push_back(u);
		}
	}

    bool relax(int &w, int v)
    {
		if (w > v) {
			w = v;
			return true;
		} else {
			return false;
		}
    }

	void dijkstra(int start, int end)
   	{
		std::priority_queue<struct heap_node> pq;

		pq.push((struct heap_node){0, start});

		dist[start] = 0;
		while (!pq.empty()) {
			struct heap_node x = pq.top();
			pq.pop();

			int u = x.u;
			if (visit[u])
				continue;
			visit[u] = true;

			for (int i = 0; i < g[u].size(); i++) {
				struct edge_t e = edges[g[u][i]];
				push_father(u, e.to, dist[u] + e.cost);

				if (relax(dist[e.to], dist[u] + e.cost)) {
					father[e.to].clear();//important
					father[e.to].push_back(u);

					pq.push((struct heap_node){dist[e.to], e.to});
				}
			}
		}
		printf("dist %d\n", dist[end]);
	}

	void dfs(int root) 
	{
		visit[root] = true;
		ncount[root]++;
		if (father[root].size() == 0) {
			printf("%d\n", root);
		}
		for (int i = 0; i < father[root].size(); i++) {
			printf("%d ", root);
			dfs(father[root][i]);
		}
		visit[root] = false;
	}
} dij;

int buffer_sep(FILE *fp)
{
	int from, to, dist;

	while (fscanf(fp, "%d %d %d", &from, &to, &dist) != EOF) {
		dij.add_edge(from, to, dist);	
		dij.add_edge(to, from, dist);
		if (!vertices[to]) {
			vertices[to] = true;
		}
		if (!vertices[from]) {
			vertices[from] = true;
		}
	}
}

void buffer_out(FILE *fp, int start, int end) 
{
	for (int i = 0; i < N; i++) {
		if (vertices[i] && i != start && i != end && dij.ncount[i]) {
			fprintf(fp, "%d : %d\n", i, dij.ncount[i]);
		}
	}	
}

int main()
{
	FILE *fp_in = fopen("../data/graph.txt", "r+");

	if (fp_in == NULL) {
		fprintf(stderr, "Open file graph error!");
		exit(1);
	}

	timeval t1, t2;
	double elapsed_time = 0.0f;

	//read input
	memset(vertices, false, sizeof(vertices));
	dij.init(N);
	int nnodes = buffer_sep(fp_in);
	int start = 0, end = 100;

	//dijkstra
	gettimeofday(&t1, NULL);
	dij.dijkstra(start, end);
	gettimeofday(&t2, NULL);
	elapsed_time = (t2.tv_sec - t1.tv_sec) * 1000.0; 
	elapsed_time += (t2.tv_usec - t1.tv_usec) / 1000.0;
	printf("elapsed time %f\n", elapsed_time);

	//dfs
	memset(dij.visit, false, sizeof(dij.visit));
	dij.dfs(end);

	//output 
	FILE *fp_out = fopen("../output/graph_heap.txt", "w+");

	buffer_out(fp_out, start, end);

	if (fp_out == NULL) {
		fprintf(stderr, "Writing file graph_heap error!");
		exit(1);
	}

	fclose(fp_out);
	fclose(fp_in);

	return 0;
}
