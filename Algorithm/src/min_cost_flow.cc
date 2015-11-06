#define DEBUG
#include <vector>
#include <queue>
#include <cstring>
#include <cstdio>
#include <cstdlib>

static const int N = 100 << 1;
static const int LEN = 1024;
static const int INF = 0x3f3f3f3f;

int source, sink;
int nnodes, nedges;
int k = INF;
bool query[N][N];

struct edge {
	int from, to, cap, flow, cost;
};

struct mcmf {
	int n, m, s, t, supply;
	std::vector<struct edge> edges;
	std::vector<int> g[N];
	bool inq[N];
	int d[N];
	int p[N];
	int a[N];

	void set_supply(int k)
	{
		supply = k;
	}

	void print()
	{
		printf("round\n");
		for (int i = 0; i < edges.size(); ++i) {
			if (query[edges[i].from][edges[i].to])
			printf("from %d, to %d, cap %d, flow %d, cost %d\n",
					edges[i].from,
					edges[i].to,
					edges[i].cap,
					edges[i].flow,
					edges[i].cost);
		}
	}
	void init(int n) 
	{
		this->n = n;
		for (int i = 0; i <= n; ++i) 
			g[i].clear();

		edges.clear();
	}

	void add_edge(int from, int to, int cap, int cost) 
	{
		edges.push_back((struct edge){from, to, cap, 0, cost});
		edges.push_back((struct edge){to, from, 0, 0, -cost});

		m = edges.size();
		g[from].push_back(m - 2);
		g[to].push_back(m - 1);
	}

	bool spfa(int s, int t, int& flow, int& cost) 
	{
		for (int i = 0; i <= n; ++i) 
			d[i] = INF;

		memset(inq, false, sizeof(inq));
		d[s] = 0; 
		inq[s] = 1; 
		p[s] = 0; 
		a[s] = INF;

		std::queue<int> q;
		q.push(s);

		while (!q.empty()) {
			int u = q.front();
			q.pop();

			inq[u] = false;
			for (int i = 0; i < g[u].size(); ++i) {
				struct edge& e = edges[g[u][i]];
				if (e.cap > e.flow && d[e.to] > d[u] + e.cost) {
					d[e.to] = d[u] + e.cost;
					p[e.to] = g[u][i];
					a[e.to] = std::min(a[u], e.cap - e.flow);
					if (!inq[e.to]) {
						q.push(e.to);
						inq[e.to] = true;
					}

				}
			}
		}

		if (d[t] == INF)
			return false;

		//terminate when it reaches supply

		int aug = a[t];
		bool ret = true;

		if (aug + flow >= supply) {
			aug = supply - flow;
			flow = supply;
			ret = false;
		} else {
			flow += aug;
		}

		cost += d[t] * aug;

		int u = t;
		while (u != s) {
			edges[p[u]].flow += aug;
			edges[p[u] ^ 1].flow -= aug;
			u = edges[p[u]].from;
		}

		print();
		printf("flow %d\n", flow);


		return ret;
	}

	int min_cost(int s, int t) 
	{
		int flow = 0, cost = 0;
		while (spfa(s, t, flow, cost));
		return cost;
	}
} min_cost_flow;


void config()
{
	printf("source %d\n", source);
	printf("sink %d\n", sink);
	printf("nnodes %d\n", nnodes);
	printf("nedges %d\n", nedges);
	printf("k %d\n", k);
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
			min_cost_flow.init(nnodes);
		} else if (str[0] == 'n') {
			char tmp_c;
			int tmp_i1, tmp_i2;
			sscanf(str, "%c %d %d", &tmp_c, &tmp_i1, &tmp_i2); 

			if (tmp_i2 > 0) {
				source = tmp_i1;	
				k = std::min(k, tmp_i2);
			} else if (tmp_i2 < 0) {
				sink = tmp_i1;
				k = std::min(k, -tmp_i2);
			} else {
				//do nothing
			}

		} else if (str[0] == 'a') {
			char tmp_c;
			int tmp_i1, tmp_i2, tmp_i3, tmp_i4, tmp_i5;
			sscanf(str, "%c %d %d %d %d %d", &tmp_c, &tmp_i1, &tmp_i2, &tmp_i3, &tmp_i4, &tmp_i5);

			query[tmp_i1][tmp_i2] = true;
			min_cost_flow.add_edge(tmp_i1, tmp_i2, tmp_i4, tmp_i5);
		}
	}
	min_cost_flow.set_supply(k);

#ifdef DEBUG
	config();
#endif
}

void buffer_out(FILE *fp, int ans)
{
	fprintf(fp, "s %d\n", ans);

	std::vector<struct edge>& edges = min_cost_flow.edges;

	for (int i = 0; i < edges.size(); ++i) {
		if (query[edges[i].from][edges[i].to])
			fprintf(fp, "f %d %d %d\n", edges[i].from, edges[i].to, edges[i].flow);
	}
	
}

int main()
{
	FILE* fp_in = fopen("../data/min_cost.txt", "r+");

	if (fp_in == NULL) {
		fprintf(stderr, "Open file min_cost error!");
		exit(1);
	}

	buffer_sep(fp_in);
	min_cost_flow.print();

	int ans = min_cost_flow.min_cost(source, sink);

	FILE *fp_out = fopen("../output/min_cost.txt", "w+");

	if (fp_out == NULL) {
		fprintf(stderr, "Writing file min_cost error!");
		exit(1);
	}

	buffer_out(fp_out, ans);

	fclose(fp_in);
	fclose(fp_out);

	return 0;
}
