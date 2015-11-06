#include <cstdio>
#include <cstdlib>
static const int N = 100010;
int data[N];
long long ncount;

inline void m_swap(int *a, int *b)
{
	int tmp = *b;
	*b = *a;
	*a = tmp;
}

int partition(int l, int len)
{
	int pivot = data[l + len - 1];
	int left = l;
	for (int i = l; i < l + len; i++) {
		if (data[i] < pivot) {
			m_swap(&data[i], &data[left]);
			ncount += i - left;
			left++;
		}
	}
	m_swap(&data[l + len - 1], &data[left]);
	ncount += l + len - 1 - left;
	return left;
}

void q_sort(int l, int len)
{
	if (len == 1 || len == 0)
		return;
	int mid = partition(l, len);
	q_sort(l, mid - l);
	q_sort(mid + 1, len - (mid - l + 1));	
}

void inline buffer_sep(FILE* fp, int& n)
{
	int tmp;
	while (fscanf(fp, "%d", &tmp) != EOF) {
		data[n++] = tmp;
	}
}

void inline buffer_out(FILE* fp, int n)
{
	for (int i = 0; i < n; i++) {
		fprintf(fp, "%d\n", data[i]);
	}
}

void inline buffer_out(FILE* fp)
{
	fprintf(fp, "%lld\n", ncount);
}

int main()
{
	FILE* fp = fopen("../data/inversions_small.txt", "r+");

	if (fp == NULL) {
		fprintf(stderr, "Open file inversions error!");
		exit(1);
	}

	int nlen = 0;
	buffer_sep(fp, nlen);

	q_sort(0, nlen);

	fp = fopen("../output/inversions_qsort.txt", "w+");

	if (fp == NULL) {
		fprintf(stderr, "Writing file inversions_qsort error!");
		exit(1);
	}

	buffer_out(fp);

	fclose(fp);
	return 0;
}
