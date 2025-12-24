#ifndef BMSSP_HPP
#define BMSSP_HPP

#include "metrics.hpp"

#include <vector>
#include <queue>
#include <limits>
#include <cmath>
#include <algorithm>

namespace spp {

template<typename T>
class bmssp {
public:
    static constexpr T INF = std::numeric_limits<T>::max() / 4;

private:
    int n;
    std::vector<std::vector<std::pair<int,T>>> adj;

    std::vector<T> dist;
    std::vector<int> pred;
    std::vector<bool> visited;

    int max_depth;
    Metrics* M;   // contador de instrucciones

public:
    bmssp(int n_, Metrics* metrics = nullptr)
        : n(n_), M(metrics)
    {
        adj.assign(n, {});
        dist.assign(n, INF);
        pred.assign(n, -1);
        visited.assign(n, false);

        if (M) M->assign(4 * n);

        max_depth = std::max(1, (int)std::log2(std::max(2, n)));
        if (M) M->assign();
    }

    void addEdge(int u, int v, T w) {
        adj[u].emplace_back(v, w);
        if (M) {
            M->access();
            M->assign(3);
        }
    }

    void prepare_graph(bool /*unused*/) {}

    std::pair<std::vector<T>, std::vector<int>> execute(int source) {
        std::fill(dist.begin(), dist.end(), INF);
        std::fill(pred.begin(), pred.end(), -1);
        std::fill(visited.begin(), visited.end(), false);
        if (M) M->assign(3 * n);

        dist[source] = 0;
        pred[source] = source;
        if (M) M->assign(2);

        std::vector<int> S = {source};
        if (M) M->assign();

        bmsspRec(S, INF, 0);

        // Corrección final (asegura optimalidad)
        finalCorrectionDijkstra();

        return {dist, pred};
    }

private:
    // =====================================================
    // Base case: Dijkstra acotado
    // =====================================================
    void baseCase(int src, T B, int limit = 8) {
        using State = std::pair<T,int>;
        std::priority_queue<State, std::vector<State>, std::greater<State>> pq;

        pq.push({dist[src], src});
        if (M) {
            M->call();
            M->access();
        }

        int processed = 0;
        if (M) M->assign();

        while (!pq.empty() && processed < limit) {
            if (M) M->compare(2);

            auto [d,u] = pq.top();
            pq.pop();
            if (M) {
                M->call(2);
                M->assign(2);
            }

            if (visited[u]) continue;
            if (d != dist[u]) continue;
            if (M) {
                M->compare(2);
                M->access();
            }

            visited[u] = true;
            processed++;
            if (M) M->assign(2);

            for (auto &[v,w] : adj[u]) {
                if (dist[u] + w < dist[v] && dist[u] + w < B) {
                    if (M) {
                        M->compare(2);
                        M->arithmetic();
                        M->access(2);
                        M->assign(2);
                    }

                    dist[v] = dist[u] + w;
                    pred[v] = u;
                    pq.push({dist[v], v});
                    if (M) M->call();
                }
            }
        }
    }

    // =====================================================
    // Selección heurística de pivotes
    // =====================================================
    std::vector<int> findPivots(const std::vector<int>& S, T B) {
        std::vector<int> pivots;
        if (M) M->assign();

        for (int u : S) {
            int relax = 0;
            if (M) M->assign();

            for (auto &[v,w] : adj[u]) {
                if (dist[u] + w < dist[v] && dist[u] + w < B) {
                    relax++;
                    if (M) {
                        M->compare(2);
                        M->arithmetic();
                        M->access(2);
                        M->assign();
                    }
                }
            }

            if (relax >= 2) {
                pivots.push_back(u);
                if (M) {
                    M->compare();
                    M->assign();
                }
            }
        }

        if (pivots.empty()) {
            pivots = S;
            if (M) M->assign();
        }

        return pivots;
    }

    // =====================================================
    // BMSSP recursivo
    // =====================================================
    void bmsspRec(const std::vector<int>& S, T B, int depth) {
        if (S.empty() || depth >= max_depth) {
            if (M) M->compare(2);
            return;
        }

        if (S.size() == 1) {
            if (M) M->compare();
            baseCase(S[0], B);
            return;
        }

        auto pivots = findPivots(S, B);
        if (M) M->assign();

        using State = std::pair<T,int>;
        std::priority_queue<State, std::vector<State>, std::greater<State>> pq;

        for (int u : pivots) {
            pq.push({dist[u], u});
            if (M) {
                M->call();
                M->access();
            }
        }

        std::vector<int> nextS;
        int quota = 4 * (int)S.size();
        if (M) M->assign(2);

        while (!pq.empty() && (int)nextS.size() < quota) {
            if (M) M->compare(2);

            auto [d,u] = pq.top();
            pq.pop();
            if (M) {
                M->call(2);
                M->assign(2);
            }

            if (visited[u]) continue;
            visited[u] = true;
            nextS.push_back(u);
            if (M) M->assign(2);

            for (auto &[v,w] : adj[u]) {
                if (dist[u] + w < dist[v] && dist[u] + w < B) {
                    if (M) {
                        M->compare(2);
                        M->arithmetic();
                        M->access(2);
                        M->assign(2);
                    }

                    dist[v] = dist[u] + w;
                    pred[v] = u;
                    pq.push({dist[v], v});
                    if (M) M->call();
                }
            }
        }

        bmsspRec(nextS, B, depth + 1);
    }

    // =====================================================
    // Corrección final con Dijkstra completo
    // =====================================================
    void finalCorrectionDijkstra() {
        using State = std::pair<T,int>;
        std::priority_queue<State, std::vector<State>, std::greater<State>> pq;

        for (int i = 0; i < n; i++) {
            if (dist[i] < INF) {
                pq.push({dist[i], i});
                if (M) {
                    M->compare();
                    M->call();
                }
            }
        }

        while (!pq.empty()) {
            auto [d,u] = pq.top();
            pq.pop();
            if (M) {
                M->call(2);
                M->assign(2);
            }

            if (d != dist[u]) continue;
            if (M) M->compare();

            for (auto &[v,w] : adj[u]) {
                if (dist[u] + w < dist[v]) {
                    if (M) {
                        M->compare();
                        M->arithmetic();
                        M->access(2);
                        M->assign(2);
                    }

                    dist[v] = dist[u] + w;
                    pred[v] = u;
                    pq.push({dist[v], v});
                    if (M) M->call();
                }
            }
        }
    }
};

} // namespace spp

#endif // BMSSP_HPP
