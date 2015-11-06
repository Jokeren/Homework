#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <set>
static const int N = 10010;
static const int INF = 0x3f3f3f3f;

std::pair<double, double> data[N];

double inline calc_dist(int p1, int p2)
{
	double dist = sqrtf((data[p1].first - data[p2].first) * 
			(data[p1].first - data[p2].first) + 
			(data[p1].second - data[p2].second) *
		   	(data[p1].second - data[p2].second));
	return dist;
}

double find_closet(int ntotal)
{
	double res = INF;
	for (int i = 0; i < ntotal; i++) {
		for (int j = 0; j < ntotal; j++) {
			if (i != j) 
				res = std::min(res, calc_dist(i, j));
		}
	}
	return res;
}

void buffer_sep(FILE* fp, int& n)
{
	while (fscanf(fp, "%lf %lf", 
				&data[n].first, &data[n].second) != EOF) {
		n++;
	}
}

void inline buffer_out(FILE* fp, double ans)
{
	fprintf(fp, "%f\n", ans);
}

int main()
{
	FILE* fp = fopen("../data/closet.txt", "r+");

	if (fp == NULL) {
		fprintf(stderr, "Open file closet error!");
		exit(1);
	}

	int nlen = 0;
	buffer_sep(fp, nlen);

	double ans = INF;
	ans = find_closet(nlen);

	fp = fopen("../output/closet.txt", "w+");

	if (fp == NULL) {
		fprintf(stderr, "Writing file closet error!");
		exit(1);
	}

	buffer_out(fp, ans);

	fclose(fp);

	return 0;
}
