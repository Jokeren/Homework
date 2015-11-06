#define DEBUG
#include <vector>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
static const int M = 110;
static const int INF = 0x3f3f3f3f;

double A[M][M];
std::vector<int> B;
std::vector<int> N;
std::vector<double> b;
double z;
struct my_vector_t {
	std::vector<double> _m_vector;

	bool less_than_zero() {
		for (int i = 0; i < _m_vector.size(); ++i) {
			if (_m_vector[i] < 0) 
				return true;
		}
		return false;
	}

	int idx_less_than_zero() {
		for (int i = 0; i < _m_vector.size(); ++i) {
			if (_m_vector[i] < 0) 
				return i;
		}
		return -1;
	}
}c;

bool simplex(int m, int n);
bool initialize_simplex(int m, int n);
void initialize(FILE *fp, int& m, int& n);
void pivot(struct my_vector_t& t, int m, int l, int e, double& zz);
void print(struct my_vector_t& t, int m, int n);
void transform(int m, int n);
void output(std::vector<double> x);

bool simplex(int m, int n, FILE *fp)
{
	if (!initialize_simplex(m, n))
		return false;

	while (c.less_than_zero()) {
		int e = c.idx_less_than_zero();
		double ret = INF;
		int ret_idx = -1;
		for (int i = 0; i < m; ++i) {
			if (A[i][e] > 0) {
				if (b[i] / A[i][e] < ret) {
					ret = b[i] / A[i][e];
					ret_idx = i;
				}
			} 
		}
		printf("ret : %lf\n", ret);
		if (ret == INF) {
			printf("unbounded solution!\n");
			return false;
		}
		pivot(c, m, ret_idx, e, z);
#ifdef DEBUG
		print(c, m, n);
#endif
	}

	std::vector<double> x;
	x.resize(m + n);
	for (int i = 0; i < B.size(); ++i) {
		x[B[i]] = b[i];
	}

	for (int i = 0; i < x.size(); ++i) {
		fprintf(fp, "%f ", x[i]);
	}

	return true;
}

bool initialize_simplex(int m, int n)
{
	//has one less than zero
	double min = INF;
	int min_idx = -1;
	for (int i = 0; i < b.size(); ++i) {
		if (min > b[i]) {
			min = b[i];
			min_idx = i;
		}
	}

	if (min >= 0)
		return true;

	//resize t
	my_vector_t t;
	t._m_vector.resize(n + 1, 0);
	t._m_vector[0] = 1.0f;
	double z1 = 0.0f;
	for (int i = 0; i < m; ++i) {
		for (int j = m + n; j >= 1; --j) {
			A[i][j] = A[i][j - 1];
		}

		A[i][0] = -1;
	}	
	++n;
	
	//resize B
	for (int i = 0; i < B.size(); ++i) {
		++B[i];
	}

	//resize N
	N.resize(N.size() + 1);
	for (int i = N.size() + 1; i >= 1; --i) {
		N[i] = N[i - 1] + 1;
	}
	N[0] = 0;

#ifdef DEBUG
	print(t, m, n);
#endif

	pivot(t, m, min_idx, 0, z1);

#ifdef DEBUG
	print(t, m, n);
#endif

	while (t.less_than_zero()) {
		int e = t.idx_less_than_zero();
		double ret = INF;
		int ret_idx = -1;
		for (int i = 0; i < m; ++i) {
			if (A[i][e] > 0) {
				if (b[i] / A[i][e] < ret) {
					ret = b[i] / A[i][e];
					ret_idx = i;
				}
			} 
		}
		if (ret == INF) {
			printf("unbounded solution!\n");
			return false;
		}
		pivot(t, m, ret_idx, e, z1);
	}

#ifdef DEBUG
	print(t, m, n);
#endif

	std::vector<double> x;
	x.resize(m + n);
	for (int i = 0; i < B.size(); ++i) {
		x[B[i]] = b[i];
	}

	if (x[0] == 0.0f) {
		bool flag = true;
		for (int i = 0; i < B.size(); ++i) {
			if (0 == B[i]) {
				flag = false;
				break;
			}
		}	
		if (!flag) {
			//degenerate, How to implement?
		//	int e = t.idx_less_than_zero();
		//	double ret = -INF;
		//	int ret_idx = -1;
		//	for (int i = 0; i < ; ++i) {
		//		if (A[i][e] > 0) {
		//			if (b[i] / A[i][e] < ret) {
		//				ret = b[i] / b[i][e];
		//				ret_idx = i;
		//			}
		//		} 
		//	}
		//	if (ret == -INF) {
		//		printf("unbounded solution!\n");
		//		return false;
		//	}
		//	pivot(ret_idx, 0);
		//	transform();
		}

		transform(m, n);
		return true;
	} else {
		printf("no initial feasible solution!\n");
		return false;
	}
}

void transform(int m, int n)
{
	for (int i = 0; i < B.size(); ++i) {
		--B[i];
	}

	for (int i = 0; i < N.size(); ++i) {
		if (N[i] == 0) {
			N.erase(N.begin() + i);
			break;
		}
	}
	for (int i = 0; i < N.size(); ++i) {
		--N[i];
	}

	--n;
	for (int i = 0; i < m; ++i) {
		for (int j = 0; j < m + n; ++j) {
			A[i][j] = A[i][j + 1];
		}
	}	

	for (int i = 0; i < c._m_vector.size(); ++i) {
		bool flag = false;

		int idx = -1;
		if (c._m_vector[i] != 0.0f) {
			for (int j = 0; j < B.size(); ++j) {
				if (i == B[j]) {
					flag = true;
					idx = j;
					break;
				}
			}
		}

		if (flag) {
			//transform
			//see introduction to algorithm 3rd version
			//the form is derived as  0 = z - x1 - x2 - x3
			double tmp = c._m_vector[i];
			c._m_vector[i] = 0;
			for (int j = 0; j < N.size(); ++j) {
				c._m_vector[N[j]] -= tmp * A[idx][N[j]];
			}
			z -= tmp * b[idx];
		}
	}


#ifdef DEBUG
	print(c, m, n);
#endif 
}

void pivot(struct my_vector_t& t, int m, int l, int e, double& zz)
{
	//row l
	b[l] /= A[l][e];

	double tmp = A[l][e];
	for (int i = 0; i < N.size(); ++i) {
		A[l][N[i]] /= tmp;
	}

	for (int i = 0; i < B.size(); ++i) {
		A[l][B[i]] /= tmp;
	}

	//other rows
	for (int i = 0; i < m; ++i) {
		if (i == l)
			continue;

		tmp = A[i][e] / A[l][e];

		for (int j = 0; j < N.size(); ++j) {
			A[i][N[j]] -= A[l][N[j]] * tmp;	
		}

		for (int j = 0; j < B.size(); ++j) {
			A[i][B[j]] -= A[l][B[j]] * tmp;
		}

		//b
		b[i] -= b[l] * tmp;
	}	

	//c
	tmp = t._m_vector[e] / A[l][e];
	for (int i = 0; i < t._m_vector.size(); ++i) {
		t._m_vector[i] -= tmp * A[l][i];
	}	
	zz -= b[l] * tmp;	

	//update B and N
	for (int i = 0; i < N.size(); ++i) {
		if (N[i] == e) {
			N[i] = B[l];
			break;
		}
	}
	B[l] = e;
}

void initialize(FILE *fp, int& m, int& n)
{
	//m * n matrix
	fscanf(fp, "%d %d", &m, &n);
	
	c._m_vector.resize(m + n, 0);	
	b.resize(m);

	for (int i = 0; i < n; ++i) {
		fscanf(fp, "%lf", &c._m_vector[i]);	
	}	
	z = 0.0f;

	//set N and B
	for (int i = 0; i < m; ++i) {
		for (int j = 0; j < n; ++j) {
			fscanf(fp, "%lf", &A[i][j]);
		}
		for (int j = n; j < n + m; ++j) {
			A[i][j] = 0;
		}
		A[i][n + i] = 1;

		fscanf(fp, "%lf", &b[i]);
	}

	N.resize(n);
	B.resize(m);
	for (int i = 0; i < n; ++i) {
		N[i] = i;
	}

	for (int i = 0; i < m; ++i) {
		B[i] = n + i;
	}
#ifdef DEBUG
	print(c, m, n);
#endif 
}

void print(struct my_vector_t& t, int m, int n)
{
	for (int i = 0; i < n + m; ++i) {
		printf("%f ", t._m_vector[i]);
	}
	printf(" = %f\n", z);

	for (int i = 0; i < m; ++i) {
		for (int j = 0; j < n + m; ++j) {
			printf("%f ", A[i][j]);
		}
		printf(" = %f\n", b[i]);
	}

	printf("B:\n");
	for (int i = 0; i < B.size(); ++i) {
		printf("%d ", B[i]);
	}
	printf("\n");

	printf("N:\n");
	for (int i = 0; i < N.size(); ++i) {
		printf("%d ", N[i]);
	}
	printf("\n\n");
}

void output(std::vector<double> x)
{
	printf("output:\n");
	for (int i = 0; i < x.size(); ++i) {
		printf("%f ", x[i]);
	}
	printf("\n");
	printf("\n");
}

int main()
{
	FILE *fp_in = fopen("../data/simplex.txt", "r+");

	if (fp_in == NULL) {
		fprintf(stderr, "Open file simplex error!");
		exit(1);
	}

	int m, n;
	initialize(fp_in, m, n);

#ifdef DEBUG
	print(c, m, n);	
#endif 

	FILE *fp_out = fopen("../output/simplex.txt", "w+");

	if (fp_out == NULL) {
		fprintf(stderr, "Writing file simplex error!");
		exit(1);
	}

	simplex(m, n, fp_out);


	fclose(fp_in);
	fclose(fp_out);
	return 0;
}
