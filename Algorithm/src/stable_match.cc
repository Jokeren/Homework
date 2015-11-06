#define _USE_MATH_DEFINES

#ifdef ONLINE_JUDGE
#define FINPUT(file) 0
#define FOUTPUT(file) 0
#else
#define FINPUT(file) freopen(file,"r",stdin)
#define FOUTPUT(file) freopen(file,"w",stdout)
#endif

#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <set>
#include <stack>
#include <string>
#include <map>
#include <vector>
#include <queue>
#include <algorithm>
#include <functional>

typedef long long ll;
typedef long double ld;
static const int N = 510;
static const int M = 100100;
static const int LEN = 50;
static const int MAX = 0x7fffffff;
static const int GMAX = 0x3f3f3f3f;
static const int MIN = ~MAX;
static const double EPS = 1e-9;
static const int BASE = 1000000007;
static const int CH = 258;

std::map<std::string, int> boy_names;
std::map<std::string, int> girl_names;
std::map<int, std::string> name_boys;
std::map<int, std::string> name_girls;
int next_girl[N][N];
int girls_rankings[N][N];
int girls_parterner[N];
int boys_parterner[N];
int match_index[N];

void gale_shapley(int n)
{
    std::queue<int> boys_queue;

    //init boys queue
    for (int i = 1; i <= n; i++) {
        boys_queue.push(i);
        match_index[i] = 1;
    }

    while (!boys_queue.empty()) {
        int cur_boy = boys_queue.front();
        boys_queue.pop();

        int cur_girl = next_girl[cur_boy][match_index[cur_boy]];
        if (girls_parterner[cur_girl] == 0) {
            girls_parterner[cur_girl] = cur_boy;
            boys_parterner[cur_boy] = cur_girl;
        } else if (girls_rankings[cur_girl][girls_parterner[cur_girl]]
                   > girls_rankings[cur_girl][cur_boy]) {
            int tmp = girls_parterner[cur_girl];
            girls_parterner[cur_girl] = cur_boy;
            boys_parterner[cur_boy] = cur_girl;
            boys_queue.push(tmp);
            boys_parterner[tmp] = 0;
        } else {
            boys_queue.push(cur_boy);
            //do nothing
        }
        ++match_index[cur_boy];
    }
}

int main()
{
    FINPUT("in.txt");
    FOUTPUT("out.txt");

    int n;
    while (scanf("%d", &n) != EOF) {
        char name[LEN];
        int boy_idx = 1;
        int girl_idx = 1;
        for (int i = 1; i <= n; ++i) {
            scanf("%s", name);
            std::string boy_name = name;
            boy_names[boy_name] = boy_idx;
            name_boys[boy_idx] = boy_name;
            ++boy_idx;

            for (int j = 1; j <= n; ++j) {
                scanf("%s", name);
                std::string girl_name = name;
                if (!girl_names[girl_name]) {
                    girl_names[girl_name] = girl_idx;
                    name_girls[girl_idx] = girl_name;
                    ++girl_idx;
                }
                next_girl[boy_idx - 1][j] = girl_names[girl_name];
            }
        }

        for (int i = 1; i <= n; ++i) {
            scanf("%s", name);
            std::string girl_name = name;
            girl_idx = girl_names[girl_name];
            for (int j = 1; j <= n; ++j) {
                scanf("%s", name);
                std::string boy_name = name;
                girls_rankings[girl_idx][boy_names[boy_name]] = j;
            }
        }

        gale_shapley(n);

        for (int i = 1; i <= n; ++i) {
            std::string name1 = name_boys[i];
            std::string name2 = name_girls[boys_parterner[i]];
            printf("%s %s\n", name1.c_str(), name2.c_str());
        }
        printf("\n");

        memset(girls_parterner, 0, sizeof(girls_parterner));
        memset(boys_parterner, 0, sizeof(boys_parterner));
        memset(next_girl, 0, sizeof(next_girl));
        memset(girls_rankings, 0, sizeof(girls_rankings));
        boy_names.clear();
        girl_names.clear();
        name_boys.clear();
        name_girls.clear();
    }
    return 0;
}
