#include <vector>
#include <queue>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/time.h>

static const int GMAX = 0x3f3f3f3f;
static const int N = 20100;

struct edge_t {
    int to, cost;
};
std::vector<struct edge_t> g[N];
std::vector<int> father[N];
bool vertices[N];
bool vs[N];

void dijkstra(int start, int end)
{
	int path[N];

	for(int i = 0;i<=N;i++){
		path[i] = GMAX;
	}

	path[start]=0;

	for(int i=0;i<N;i++){
		int min_dist=GMAX;
		int tmp_u=-1;
		for(int j=0;j<=N;j++){
			if(vs[j]==false&&min_dist>path[j]){
				tmp_u=j;
				min_dist=path[j];
			}
		}

		if(tmp_u!=-1){
			vs[tmp_u]=true;
			for(int j=0;j<g[tmp_u].size();j++){
				struct edge_t tmp = g[tmp_u][j];
				if (path[tmp.to] == (path[tmp_u] + tmp.cost)) {
					father[tmp.to].push_back(tmp_u);
				}

				if (path[tmp.to]>(path[tmp_u]+tmp.cost)){
					father[tmp.to].clear();
					father[tmp.to].push_back(tmp_u);
					path[tmp.to]=path[tmp_u]+tmp.cost;
				}
			}
		}
	}
	printf("dist %d\n", path[end]);
}

int ncount[N];
void dfs(int root) 
{
	vs[root] = true;
	ncount[root]++;
	if (father[root].size() == 0) {
		printf("%d\n", root);
	}
	for (int i = 0; i < father[root].size(); i++) {
		printf("%d ", root);
		dfs(father[root][i]);
	}
	vs[root] = false;
}

void buffer_sep(FILE *fp)
{
	int from, to, dist;

	while (fscanf(fp, "%d %d %d", &from, &to, &dist) != EOF) {
		struct edge_t edge;
		edge.to = to;
		edge.cost = dist;
		g[from].push_back(edge);
		edge.to = from;
		g[to].push_back(edge);

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
		if (vertices[i] && i != start && i != end && ncount[i]) {
			fprintf(fp, "%d : %d\n", i, ncount[i]);
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
	buffer_sep(fp_in);
	int start = 0, end = 100;
	memset(vs, false, sizeof(vs));

	//dijkstra
	gettimeofday(&t1, NULL);
	dijkstra(start, end);
	gettimeofday(&t2, NULL);
	elapsed_time = (t2.tv_sec - t1.tv_sec) * 1000.0; 
	elapsed_time += (t2.tv_usec - t1.tv_usec) / 1000.0;
	printf("elapsed time %f\n", elapsed_time);

	//dfs
	memset(vs, false, sizeof(vs));
	dfs(end);

	//output 
	FILE *fp_out = fopen("../output/graph_list.txt", "w+");

	buffer_out(fp_out, start, end);

	if (fp_out == NULL) {
		fprintf(stderr, "Writing file graph_list error!");
		exit(1);
	}

	fclose(fp_out);
	fclose(fp_in);

	return 0;
}
