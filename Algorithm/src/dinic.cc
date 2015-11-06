#include <vector>
#include <queue>
#include <cstring>
#include <cstdio>
#include <cstdlib>

static const int LEN = 1024;
static const int N = 100 << 1;
static const int INF = 0x3f3f3f3f;

int nnodes, nedges;
int source, sink;
bool query[N][N];

void config()
{
	printf("source %d\n", source);
	printf("sink %d\n", sink);
	printf("nnodes %d\n", nnodes);
	printf("nedges %d\n", nedges);
}

struct edge {
	int from, to, cap, flow;
};

struct dinic {
	int n, m, s, t;
	std::vector<struct edge> edges;
	std::vector<int> g[N];
	bool vis[N];
	int d[N];
	int cur[N];

	void init(int n, int s, int t)
	{
		this->n = n;
		this->s = s;
		this->t = t;
	}

	void print()
	{
		printf("round\n");
		for (int i = 0; i < edges.size(); ++i) {
			if (query[edges[i].from][edges[i].to])
			printf("from %d, to %d, cap %d, flow %d\n",
					edges[i].from,
					edges[i].to,
					edges[i].cap,
					edges[i].flow);
		}
	}

	void add_edge(int from, int to, int cap)
	{
		edges.push_back((struct edge){from, to, cap, 0});
		edges.push_back((struct edge){to, from, 0, 0});

		m = edges.size();
		g[from].push_back(m - 2);
		g[to].push_back(m - 1);
	}

	bool bfs()
   	{
		memset(vis, false, sizeof(vis));
		std::queue<int> q;
		q.push(s);
		
		d[s] = 0;
		vis[s] = true;

		while (!q.empty()) {
			int x = q.front();
			q.pop();

			for (int i = 0; i < g[x].size(); ++i) {
				struct edge& e = edges[g[x][i]];

				//only consider residual edges
				if (!vis[e.to] && e.cap > e.flow) {
					vis[e.to] = 1;
					d[e.to] = d[x] + 1;
					q.push(e.to);
				}
			}
		}

		return vis[t];
	}

	int dfs(int x, int a)
	{
		if (x == t || a == 0)
			return a;

		int flow = 0, f;
		for (int& i = cur[x]; i < g[x].size(); ++i, cur[x] = i) {
			struct edge& e = edges[g[x][i]];
			if (d[x] + 1 == d[e.to] && 
					(f = dfs(e.to, std::min(a, e.cap - e.flow))) > 0) {
				e.flow += f;
				edges[g[x][i] ^ 1].flow -= f;
				flow += f;
				a -= f;
				if (a == 0)
					break;
			}
		}

		return flow;
	}

	int max_flow()
	{
		int flow = 0;
		while (bfs()) {
			flow += dfs(s, INF);
			print();
		}

		return flow;
	}
} dinic_flow;

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
				dinic_flow.init(nnodes, source, sink);
			} else {
				//do nothing
			}

		} else if (str[0] == 'a') {
			char tmp_c;
			int tmp_i1, tmp_i2, tmp_i3;
			sscanf(str, "%c %d %d %d", &tmp_c, &tmp_i1, &tmp_i2, &tmp_i3);

			dinic_flow.add_edge(tmp_i1, tmp_i2, tmp_i3);
			query[tmp_i1][tmp_i2] = true;
		}
	}

#ifdef DEBUG
	config();
#endif
}

void buffer_out(FILE *fp, int ans)
{
	fprintf(fp, "s %d\n", ans);

	std::vector<struct edge>& edges = dinic_flow.edges;

	for (int i = 0; i < edges.size(); ++i) {
		if (query[edges[i].from][edges[i].to])
			fprintf(fp, "f %d %d %d\n", edges[i].from, edges[i].to, edges[i].flow);
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

	int ans = dinic_flow.max_flow();

	FILE *fp_out = fopen("../output/dinic.txt", "w+");

	if (fp_out == NULL) {
		fprintf(stderr, "Writing file dinic error!");
		exit(1);
	}

	buffer_out(fp_out, ans);

	fclose(fp_in);
	fclose(fp_out);

	return 0;
}
