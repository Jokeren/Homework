#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>

static const int N = 100;

int ncase;
int total;
int data[N];
int dp[N];
int res[N];

int inline mul_three(int num)
{
	return num * num * num;
}

void print_neatly(int ncount, int idx)
{
	dp[0] = 0;
	dp[1] = mul_three(total - data[1]);

	for (int i = 2; i <= ncount; i++) {
		dp[i] = 0x3f3f3f3f;
		int tmp = data[i];
		for (int j = i - 1; j >= 0 && total - tmp >= 0; j--) {
			dp[i] = std::min(dp[i], dp[j] + mul_three(total - tmp));
			tmp += data[j] + 1;
		}
	}

	int ans = 0x3f3f3f3f;
	int tmp = data[ncount];
	for (int i = ncount - 1; i >= 0 && total - tmp >= 0; i--) {
		ans = std::min(ans, dp[i]);	
		tmp += data[i] + 1;
	}

	res[idx] = ans;
	memset(dp, 0, sizeof(dp));
}

void buffer_sep(FILE* fp, int& ncount)
{
	int n;
	fscanf(fp, "%d %d", &n, &total);

	for (int i = 1; i <= n; i++) {
		fscanf(fp, "%d", &data[i]);
	}
	ncount = n;
}	

void buffer_out(FILE* fp, int ncase)
{
	for (int i = 1; i <= ncase; i++) {
		fprintf(fp, "%d\n", res[i]);
	}
}

int main()
{
	FILE* fp = fopen("../data/print_neatly.txt", "r+");

	if (fp == NULL) {
		fprintf(stderr, "Open file print_neatly error!");
		exit(1);
	}

	fscanf(fp, "%d", &ncase);

	for (int i = 0; i < ncase; i++) {
		int ncount = 0;
		buffer_sep(fp, ncount);
		print_neatly(ncount, i + 1);
	}

	fp = fopen("../output/print_neatly.txt", "w+");

	if (fp == NULL) {
		fprintf(stderr, "Writing file print_neatly error!");
		exit(1);
	}

	buffer_out(fp, ncase);

	fclose(fp);

	return 0;
}
