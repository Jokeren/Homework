/*
Source Code

Problem: 2292  User: robinho364 
Memory: 680K  Time: 16MS 
Language: G++  Result: Accepted 
*/
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
static const int N = 35;
static const int M = 20;
static const int LEN = 50;
static const int MAX = 0x7fffffff;
static const int GMAX = 0x3f3f3f3f;
static const int MIN = ~MAX;
static const double EPS = 1e-7;
static const int BASE = 10007;
static const int CH = 258;
int value[CH];
int mp[CH];
int dp[N][14];
int s[N][14];

void inline init()
{
    for (int i = 0; i < 26; i++) {
        mp[i] = 'a' + i;
    }
    mp[26] = '+';
    mp[27] = '*';
    mp[28] = '/';
    mp[29] = '?';
}

void solve()
{
    dp[0][0] = value[mp[0]];
    for (int i = 1; i < 30; i++) {
        dp[i][0] = dp[i - 1][0] + value[mp[i]] * (i + 1);
        for (int j = 1; j <= 11 && j <= i; j++) {
            for (int k = j - 1; k < i; k++) {
                int sum = 0;

                for (int t = k + 1; t <= i; t++) {
                    sum += value[mp[t]] * (t - k);
                }

                if (dp[k][j - 1] + sum < dp[i][j]){
                    dp[i][j] = dp[k][j - 1] + sum;
                    s[i][j] = k + 1;
                }
            }
        }
    }

    std::string str;
    int start = 29;
    for (int i = 11; i > 0; i--) {
        str.push_back(mp[s[start][i]]);
        start = s[start][i] - 1;
    }

    for (int j = str.size() - 1; j >= 0; j--) {
        printf("%c", str[j]);
    }
    printf("\n");
}

int main()
{
    init();
    int t;
    while (scanf("%d", &t) != EOF) {
        while (t--) {
            int m;
            scanf("%d ", &m);

            char str[LEN];
            for (int i = 0; i < m; i++) {
                scanf("%s", str);

                int len = strlen(str);
                for (int j = 0; j < len; j++) {
                    value[str[j]]++;
                }
            }

            memset(dp, GMAX, sizeof(dp));

            solve();

            memset(value, 0, sizeof(value));
        }
    }

    return 0;
}


