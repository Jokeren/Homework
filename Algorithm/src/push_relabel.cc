#include <vector>
#include <queue>
#include <map>
#include <cstdio>
#include <cstdlib>
static const int LEN = 1024;
static const int N = 100;

std::vector<int> graph[N];
std::vector<std::pair<int, int> > query;
int forward[N][N];
int residual[N][N];
int capacity[N][N];
int height[N];
int excess[N];

int nnodes, nedges;
int source, sink;
void config()
{
	printf("source %d\n", source);
	printf("sink %d\n", sink);
	printf("nnodes %d\n", nnodes);
	printf("nedges %d\n", nedges);

	for (int i = 1; i <= nnodes; ++i) {
		printf("node %d\n", i);
		for (int j = 0; j < graph[i].size(); ++j) {
			printf("%d capacity %d\n", graph[i][j], capacity[i][graph[i][j]]);
		}
	}
}

void buffer_sep(FILE *fp)
{
	char str[LEN + 1];
	while (fgets(str, LEN, fp) != NULL) {
		if (str[0] == '\0' || str[0] == 'c')
			continue;

		if (str[0] == 'p') {
			char tmp_s[LEN];
			char tmp_c;
			sscanf(str, "%c %s %d %d", &tmp_c, tmp_s, &nnodes, &nedges);
		} else if (str[0] == 'n') {
			char tmp_c1, tmp_c2;	
			int tmp_i;
			sscanf(str, "%c %d %c", &tmp_c1, &tmp_i, &tmp_c2); 

			if (tmp_c2 == 's') {
				source = tmp_i;	
			} else if (tmp_c2 == 't') {
				sink = tmp_i;
			} else {
				//do nothing
			}

		} else if (str[0] == 'a') {
			char tmp_c;
			int tmp_i1, tmp_i2, tmp_i3;
			sscanf(str, "%c %d %d %d", &tmp_c, &tmp_i1, &tmp_i2, &tmp_i3);

			graph[tmp_i1].push_back(tmp_i2);
			graph[tmp_i2].push_back(tmp_i1);
			capacity[tmp_i1][tmp_i2] = tmp_i3;
			residual[tmp_i1][tmp_i2] = tmp_i3;
			query.push_back(std::make_pair(tmp_i1, tmp_i2));
		}
	}

#ifdef DEBUG
	config();
#endif
}

void init()
{
	height[source] = nnodes;
	for (int i = 0; i < graph[source].size(); ++i) 
		excess[source] += capacity[source][graph[source][i]];

	for (int i = 0; i < graph[source].size(); ++i) {
		int v = graph[source][i];
		forward[source][v] = capacity[source][v];
		forward[v][source] = -capacity[source][v];
		excess[source] -= capacity[source][v];
		excess[v] = capacity[source][v];
		residual[source][v] = 0;
		residual[v][source] = capacity[source][v];
	}
}

void push(int u, int v)
{
	int tmp = std::min(excess[u], residual[u][v]);

	forward[u][v] += tmp;
	forward[v][u] = -forward[u][v];
	excess[u] -= tmp;
	excess[v] += tmp;
	residual[u][v] = capacity[u][v] - forward[u][v];
	residual[v][u] = -forward[v][u];
}

void print()
{
	printf("round\n");
//	for (int i = 0; i < nnodes; ++i) {
//		printf("height %d : %d\n", i, height[i]);
//		printf("excess %d : %d\n", i, excess[i]);
//	}

	for (int i = 0; i < query.size(); ++i) {
		printf("from %d, to %d, cap %d, flow %d\n", 
				query[i].first,
				query[i].second,
				capacity[query[i].first][query[i].second],
				forward[query[i].first][query[i].second]);
	}
}


void maxflow()
{
	init();

	std::queue<int> q;
	bool *visit = new bool[nnodes + 1]();
	
	for (int i = 0; i < graph[source].size(); ++i) {
		//why no sink? sink will not be pushed into the queue
		if (graph[source][i] != sink) {
			q.push(graph[source][i]);
			visit[graph[source][i]] = true;
		}
	}

	//combine relabel and bfs together
	while (!q.empty()) {
		int u = q.front();
		int m = -1;

		for (int i = 0; i < graph[u].size() && excess[u] > 0;++i) {
			int v = graph[u][i];

			if (residual[u][v] > 0) {
				if (height[u] > height[v]) {
					push(u, v);
					if (!visit[v] && v != source && v != sink) {
						visit[v] = true;
						q.push(v);
					}
				} else if (m == -1) {
					m = height[v];
				} else {
					m = std::min(m, height[v]);
				}
			}
		}

		if (excess[u] != 0)
			//relabel
			height[u] = 1 + m;
		else {
			visit[u] = false;
			q.pop();
		}
		print();
	}
}

void buffer_out(FILE *fp)
{
	fprintf(fp, "s %d\n", excess[sink]);	

	for (int i = 0; i < query.size(); ++i) {
		fprintf(fp, "f %d %d %d\n", query[i].first, query[i].second, 
				forward[query[i].first][query[i].second]);
	}
}

int main()
{
	FILE* fp_in = fopen("../data/max_flow.txt", "r+");

	if (fp_in == NULL) {
		fprintf(stderr, "Open file max_flow error!");
		exit(1);
	}

	buffer_sep(fp_in);

	maxflow();

	FILE *fp_out = fopen("../output/push_label.txt", "w+");

	if (fp_out == NULL) {
		fprintf(stderr, "Writing file push_label error!");
		exit(1);
	}

	buffer_out(fp_out);

	fclose(fp_in);
	fclose(fp_out);

	return 0;
}
