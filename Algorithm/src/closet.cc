#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <set>
static const int N = 10010;
static const int INF = 0x3f3f3f3f;

std::pair<double, double> data[N];
struct point {
	double pos;
	int idx;
} data_xorder[N], data_yorder[N]; 
int mp_xorder[N];
int mp_yorder[N];

double inline calc_dist(int p1, int p2)
{
	double dist = sqrt((data[p1].first - data[p2].first) * 
			(data[p1].first - data[p2].first) + 
			(data[p1].second - data[p2].second) *
		   	(data[p1].second - data[p2].second));
	return dist;
}

bool cmp_point(const struct point& p1, const struct point& p2)
{
	return p1.pos < p2.pos;
}

double find_closet(int l, int len, int ntotal)
{
	if (len == 2) {
		return calc_dist(data_xorder[l].idx, data_xorder[l + 1].idx);
	} else if (len == 0 || len == 1) {
		return INF;
	}

	double mid = (data_xorder[l + len - 1].pos + data_xorder[l].pos) / 2.0;
	double lmin = find_closet(l, len / 2, ntotal);
	double rmin = find_closet(l + len / 2, len - len / 2, ntotal);
	double theta = std::min(lmin, rmin);
	double res = theta;

	for (int i = l; i < l + len; i++) {
		if (data_xorder[i].pos < mid + theta &&
				data_xorder[i].pos > mid - theta) {
			int idx = mp_yorder[data_xorder[i].idx];
			for (int j = 1; j < 12 && idx + j < ntotal; j++) {
				res = std::min(res,
					   	calc_dist(data_yorder[idx + j].idx, 
							data_xorder[i].idx));
			}
		}
	}

	return res;
}

void buffer_sep(FILE* fp, int& n)
{
	while (fscanf(fp, "%lf %lf", 
				&data[n].first, &data[n].second) != EOF) {
		data_xorder[n].pos = data[n].first;
		data_xorder[n].idx = n;
		data_yorder[n].pos = data[n].second;
		data_yorder[n].idx = n;
		n++;
	}

	std::sort(data_xorder, data_xorder + n, cmp_point);
	for (int i = 0; i < n; i++) {
		//printf("x_order %f\n", data_xorder[i].pos);
		mp_xorder[data_xorder[i].idx] = i;
	}

	std::sort(data_yorder, data_yorder + n, cmp_point);
	for (int i = 0; i < n; i++) {
		//printf("y_order %f\n", data_yorder[i].pos);
		mp_yorder[data_yorder[i].idx] = i;
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
	ans = find_closet(0, nlen, nlen);

	fp = fopen("../output/closet.txt", "w+");

	if (fp == NULL) {
		fprintf(stderr, "Writing file closet error!");
		exit(1);
	}

	buffer_out(fp, ans);

	fclose(fp);

	return 0;
}
