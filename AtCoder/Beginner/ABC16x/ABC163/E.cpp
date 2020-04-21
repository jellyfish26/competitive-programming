#include <bits/stdc++.h>
using namespace std;
#define ll long long
#define PI acos(-1)
#define vi vector<int>
#define pi pair<int, int>
#define pl pair<ll, ll>
template<class T> istream &operator>>(istream&is,vector<T>&v){for(auto &elemnt:v)is>>elemnt;return is;}
template<class T,class U> istream &operator>>(istream&is,pair<T,U>&p){is>>p.first>>p.second;return is;}
const int MOD = 1e9+ 7;
const int INF = numeric_limits<int>::max() - (int)1e8;
const ll INFLL = numeric_limits<ll>::max() - (ll)1e17;

int main() {
    int N;
    cin >> N;
    vector<pair<ll, int>> A(N);
    for (int i = 0; i < N; i++) {
        int temp;
        cin >> temp;
        A[i] = {temp, i + 1};
    }
    sort(A.rbegin(), A.rend());
    vector<vector<ll>> dp(N + 1, vector<ll>(N + 1, 0));
    for (int i = 0; i < N; i++) {
        for (int right = 0; right <= i; right++) {
            int left = i - right;
            dp[left + 1][right] = max(dp[left + 1][right], dp[left][right] + A[i].first * abs((left + 1) - A[i].second));
            dp[left][right + 1] = max(dp[left][right + 1], dp[left][right] + A[i].first * abs((N - right) - A[i].second));
        }
    }
    ll ans = 0;
    for (int i = 0; i <= N; i++) ans = max(ans, dp[i][N - i]);
    cout << ans << endl;
}
