#ifndef BMSSP_HPP
#define BMSSP_HPP

#include <vector>
#include <queue>
#include <limits>
#include <algorithm>
#include "metrics.hpp"

namespace spp {

template<typename T>
class bmssp {
private:
    struct Edge {
        int to;
        T w;
    };

    int n;              // número de vértices
    int k;              // tamaño de bloque
    Metrics* M;         // métricas (opcional)

    std::vector<std::vector<Edge>> adj;
    std::vector<T> dist;

    static constexpr T INF = std::numeric_limits<T>::max();

    /* ===================== Dijkstra (caso base) ===================== */
    void dijkstra_base(int source, const std::vector<int>& vertices) {
        using P = std::pair<T,int>;
        std::priority_queue<P, std::vector<P>, std::greater<P>> pq;

        for (int v : vertices) {
            dist[v] = INF;
            if (M) M->assign();
        }

        dist[source] = 0;
        pq.push({0, source});
        if (M) M->call();

        while (!pq.empty()) {
            auto [d, u] = pq.top();
            pq.pop();
            if (M) M->call(2);

            if (d > dist[u]) {
                if (M) M->compare();
                continue;
            }

            for (const auto& e : adj[u]) {
                if (dist[u] != INF && dist[u] + e.w < dist[e.to]) {
                    if (M) {
                        M->arithmetic();
                        M->compare();
                    }
                    dist[e.to] = dist[u] + e.w;
                    pq.push({dist[e.to], e.to});
                    if (M) {
                        M->assign();
                        M->call();
                    }
                }
            }
        }
    }

    /* ===================== BMSSP recursivo ===================== */
    void bmssp_recursive(int source, const std::vector<int>& vertices) {
        if ((int)vertices.size() <= k) {
            dijkstra_base(source, vertices);
            return;
        }

        // dividir en bloques de tamaño k
        for (size_t i = 0; i < vertices.size(); i += k) {
            std::vector<int> block;
            for (size_t j = i; j < i + k && j < vertices.size(); ++j) {
                block.push_back(vertices[j]);
                if (M) M->assign();
            }

            // solo procesar bloque que contiene a la fuente
            if (std::find(block.begin(), block.end(), source) == block.end()) {
                if (M) M->compare(block.size());
                continue;
            }

            bmssp_recursive(source, block);
        }

        // fase de combinación (relajación global)
        for (int u : vertices) {
            if (dist[u] == INF) {
                if (M) M->compare();
                continue;
            }

            for (const auto& e : adj[u]) {
                if (dist[u] + e.w < dist[e.to]) {
                    if (M) {
                        M->arithmetic();
                        M->compare();
                        M->assign();
                    }
                    dist[e.to] = dist[u] + e.w;
                }
            }
        }
    }

public:
    /* ===================== Constructor ===================== */
    bmssp(int vertices, int k_param, Metrics* metrics = nullptr)
        : n(vertices), k(k_param), M(metrics) {
        adj.resize(n);
        dist.resize(n, INF);
    }

    /* ===================== API benchmark ===================== */
    void addEdge(int u, int v, T w) {
        adj[u].push_back({v, w});
        if (M) M->assign();
    }

    void prepare_graph(bool = false) {
        if (M) M->call();
    }

    void execute(int source) {
        std::vector<int> all_vertices(n);
        for (int i = 0; i < n; ++i) {
            all_vertices[i] = i;
            if (M) M->assign();
        }

        for (int i = 0; i < n; ++i) {
            dist[i] = INF;
            if (M) M->assign();
        }

        bmssp_recursive(source, all_vertices);
    }

    const std::vector<T>& getDistances() const {
        return dist;
    }
};

} // namespace spp

#endif
