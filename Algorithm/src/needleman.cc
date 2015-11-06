#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>

static const int N = 110;
static const int LEN = 110;
//static const int SPACE = -3;
//static const int MIS_MATCH = -1;
//static const int MATCH = 1;
/*
 * if we change the score to the follow case, the problem would be transformed to LCS problem
 */
static const int SPACE = 0;
static const int MIS_MATCH = 0;
static const int MATCH = 1;

char str_a[N][LEN];
char str_b[N][LEN];
int score[N];

void needleman(int n)
{
	int dp[LEN][LEN];

	for (int t = 0; t < n; t++) {
		const char* s1 = (const char*)str_a[t];
		int len1 = strlen(str_a[t]);
		const char* s2 = (const char*)str_b[t];
		int len2 = strlen(str_b[t]);

		for (int i = 0; i <= len1 - 1; i++) {
			dp[i + 1][0] = dp[i][0] + SPACE;
		}

		for (int i = 0; i < len2 - 1; i++) {
			dp[0][i + 1] = dp[0][i] + SPACE;
		}

		for (int i = 1; i <= len1; i++) {
			for (int j = 1; j <= len2; j++) {
				int tmp = 0;
				if (s1[i - 1] == s2[j - 1]) {
					tmp = MATCH;
				} else {
					tmp = MIS_MATCH;
				}

				dp[i][j] = std::max(dp[i - 1][j - 1] + tmp,
									dp[i - 1][j] + SPACE);
				dp[i][j] = std::max(dp[i][j],
									dp[i][j - 1] + SPACE);
			}
		}

		score[t] = dp[len1][len2];
	}
}

void buffer_sep(FILE *fp, char (*str)[LEN], int& ncount)
{
	while (fscanf(fp, "%s", str[ncount]) != EOF) {
		ncount++;
	}
}

void inline buffer_out(FILE* fp, int ncount) 
{
	for (int i = 0; i < ncount; i++) {
		fprintf(fp, "%d\n", score[i]);
	}
}

int main()
{
	FILE* fp = fopen("../data/gene1_seq.txt", "r+");

	if (fp == NULL) {
		fprintf(stderr, "Open file gene1_seq error!");
		exit(1);
	}

	int ncount = 0;
	buffer_sep(fp, str_a, ncount);

	fp = fopen("../data/gene2_seq.txt", "r+");

	if (fp == NULL) {
		fprintf(stderr, "Open file gene2_seq error!");
		exit(1);
	}

	ncount = 0;
	buffer_sep(fp, str_b, ncount);
		
	needleman(ncount);

	fp = fopen("../output/needleman.txt", "w+");

	buffer_out(fp, ncount);

	if (fp == NULL) {
		fprintf(stderr, "Writing file needleman error!");
		exit(1);
	}

	fclose(fp);

	return 0;
}
