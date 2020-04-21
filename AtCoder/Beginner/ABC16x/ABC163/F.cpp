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

template<typename T>
struct edge {
    pair<int, int> arrow;
    T cost;

    edge(int source, int to, T cost) : arrow(source, to), cost(cost) {}
    edge(int to, T cost) : arrow(-1, to), cost(cost) {}

    edge &operator=(const int &vertex) {
        arrow.second = vertex;
        return *this;
    }

    operator int() const { return arrow.second; }
};

template<typename T>
struct UnWeightedGraph : vector<vector<edge<T>>> {
    bool is_directed_graph;
    explicit UnWeightedGraph(int size, bool is_directed_graph) : vector<vector<edge<T>>>(size), is_directed_graph(is_directed_graph) {}

    void add_edge(int source, int to) {
        vector<vector<edge<T>>>::operator[](source).emplace_back(to, 0);
        if (!is_directed_graph) vector<vector<edge<T>>>::operator[](to).emplace_back(source, 0);
    }
};

template<typename T>
struct WeightedGraph : UnWeightedGraph<T> {
    explicit WeightedGraph(int size, bool is_directed_graph) : UnWeightedGraph<T>(size, is_directed_graph) {}

    void add_edge(int source, int to, T cost) {
        UnWeightedGraph<T>::operator[](source).emplace_back(to, cost);
        if (!UnWeightedGraph<T>::is_directed_graph) UnWeightedGraph<T>::operator[](to).emplace_back(source, cost);
    }
};

template<typename T>
class EulerTour {
private:
    vector<int> euler_data, euler_in, euler_out;
    size_t data_size;
    UnWeightedGraph<T> &graph;

    void dfs(int now_vertex, int before_vertex, int &reference_count) {
        euler_in[now_vertex] = reference_count;
        euler_data[reference_count++] = now_vertex;
        for (auto next_vertex : graph[now_vertex]) {
            if (next_vertex == before_vertex) continue;
            dfs(next_vertex, now_vertex, reference_count);
            euler_data[reference_count++] = now_vertex;
        }
        euler_out[now_vertex] = reference_count;
    }

public:
    explicit EulerTour(UnWeightedGraph<T> &graph, size_t data_size) : graph(graph), euler_data(2 * data_size), euler_in(data_size), euler_out(data_size), data_size(0) {}

    void build(int start_vertex = 0) {
        int reference_count = 0;
        dfs(start_vertex, -1, reference_count);
        data_size = reference_count;
    }

    // [left, right) = [first, second)
    pair<int, int> index_of_vertex(const int vertex) { return {euler_in[vertex], euler_out[vertex]}; }

    int pre_oreder(const int vertex) {return euler_in[vertex]; }

    // When using it in a query, the closed-interval.
    int post_order(const int vertex) {return euler_out[vertex] - 1; }

    // read only
    const int &operator[] (const int index) const { return euler_data[index]; }

    size_t size() const noexcept { return data_size; }
};

template<typename T>
class BinaryIndexedTree {
private:
    vector<T> data;

public:
    explicit BinaryIndexedTree(int data_size) : data(data_size + 1, 0) {}

    void update(int index, T value) {
        for (int now_index = index + 1; now_index < data.size(); now_index += (now_index & -now_index)) {
            data[now_index] += value;
        } 
    }

    T sum(int index) {
        T count_up = 0;
        for (int now_index = index + 1; now_index > 0; now_index -= (now_index & -now_index)) {
            count_up += data[now_index];
        }
        return count_up;
    }

    T sum_all() {
        return sum(data.size() - 1);
    }

    // [left, right)
    T query(int left_index, int right_index) {
        return sum(right_index - 1) - sum(left_index - 1);
    }
};

template<typename T, typename bit_type>
class CountUpEulerTour : public EulerTour<T> {
private:
    BinaryIndexedTree<bit_type> count_on_tree;
    size_t vertex_size;

    void reset_bit() { count_on_tree = BinaryIndexedTree<bit_type>(EulerTour<T>::size()); }

    void vertex_update(int index, bit_type value, vector<bool> &is_already) {
        if (is_already[EulerTour<T>::operator[](index)]) return;
        is_already[EulerTour<T>::operator[](index)] = true;
        count_on_tree.update(index, value);
    }

public:
    explicit CountUpEulerTour(UnWeightedGraph<T> &graph, size_t vertex_size) : EulerTour<T>(graph, vertex_size), count_on_tree(2 * vertex_size), vertex_size(vertex_size) {}

    void init_pre_order_count(bit_type base) {
        vector<bool> is_already_appeared(vertex_size, false);
        for (int index = 0; index < EulerTour<T>::size(); index++) vertex_update(index, base, is_already_appeared);
    }

    void init_post_order_count(bit_type base) {
        vector<bool> is_already_appeared(vertex_size, false);
        for (int index = EulerTour<T>::size() - 1; index >= 0; index--) vertex_update(index, base, is_already_appeared);
    }

    void reset_pre_order_count(bit_type base) {
        reset_bit();
        init_pre_order(base);
    }

    void reset_post_order_count(bit_type base) {
        reset_bit();
        init_post_order(base);
    }

    void reset_count() { reset_bit(); }

    void update(int partial_tree_root, bit_type value) {
        count_on_tree.update(EulerTour<T>::index_of_vertex(partial_tree_root).first, value);
    }

    bit_type query(pair<int, int> index) {
        return count_on_tree.query(index.first, index.second);
    }

    bit_type query(int partial_tree_root) {
        auto index = EulerTour<T>::index_of_vertex(partial_tree_root);
        return count_on_tree.query(index.first, index.second);
    }
};

int main() {
    int N;
    cin >> N;
    vector<vector<int>> color(N);
    for (int i = 0; i < N; i++) {
        int temp;
        cin >> temp;
        temp--;
        color[temp].push_back(i);
    }
    auto graph = UnWeightedGraph<int>(N, false);
    for (int i = 0; i < N - 1; i++) {
        int a, b;
        cin >> a >> b;
        a--; b--;
        graph.add_edge(a, b);
    }
    auto euler_tour = CountUpEulerTour<int, ll>(graph, N);
    euler_tour.build();
    euler_tour.init_pre_order_count(1);
    auto calc = [](ll n) {
        return n * (n + 1) / 2;
    };
    ll maxValue = calc(N);
    for (int i = 0; i < N; i++) {
        ll ans = maxValue;
        vector<int> &explore = color[i];
        sort(explore.begin(), explore.end(), [&](int a, int b){
            return euler_tour.pre_oreder(a) > euler_tour.pre_oreder(b);
        });
        vector<pair<int, ll>> temp;
        for (auto now : explore) {
            int count_up = 1;
            for (int next : graph[now]) {
                if (euler_tour.pre_oreder(next) < euler_tour.pre_oreder(now)) continue;
                int sum = euler_tour.query(next);
                ans -= calc(sum);
                count_up += sum;
            }
            euler_tour.update(now, -count_up);
            temp.emplace_back(now, count_up);
        }
        ans -= calc(euler_tour.query(0));
        // undo
        for (auto now : temp) euler_tour.update(now.first, now.second);
        cout << ans << endl;
    }
}
