#include <cstdio>
#include <cstdlib>
static const int N = 100010;
int data[N];
int data_copy[N];
long long ncount;

void merge_sort(int l, int len)
{
	if (len == 1 || len == 0) {
		return;
	}
	merge_sort(l, len / 2);
	merge_sort(l + len / 2, len - len / 2);
	
	int i = l, j = l + len / 2, idx = l;

	while (i < l + len / 2 && j < l + len) {
		if (data[i] > data[j]) {
			data_copy[idx] = data[j];
			j++;
			idx++;
			ncount += (len / 2 + l - i);
		} else {
			data_copy[idx] = data[i];
			i++;
			idx++;
		}
	}	
	while (i < l + len / 2) {
		data_copy[idx++] = data[i++];
	}

	while (j < l + len) {
		data_copy[idx++] = data[j++];
	}

	for (idx =  0; idx < len; idx++) {
		data[l + idx] = data_copy[l + idx];
	}
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
	FILE* fp = fopen("../data/inversions.txt", "r+");

	if (fp == NULL) {
		fprintf(stderr, "Open file inversions error!");
		exit(1);
	}

	int nlen = 0;
	buffer_sep(fp, nlen);

	merge_sort(0, nlen);

	fp = fopen("../output/inversions_merge.txt", "w+");

	if (fp == NULL) {
		fprintf(stderr, "Writing file inversions_merge error!");
		exit(1);
	}

	buffer_out(fp);

	fclose(fp);
	return 0;
}
