#ifndef DIJKSTRA_HPP
#define DIJKSTRA_HPP

#include "metrics.hpp"

#include <vector>
#include <queue>
#include <limits>
#include <algorithm>

template<typename T>
class Dijkstra {
private:
    struct Edge {
        int to;
        T weight;
        Edge(int t, T w) : to(t), weight(w) {}
    };
    
    int n;  // número de vértices
    std::vector<std::vector<Edge>> adj;  // lista de adyacencia
    std::vector<T> dist;
    std::vector<int> pred;
    
    static constexpr T INF = std::numeric_limits<T>::max();

public:
    Dijkstra(int vertices) : n(vertices), adj(vertices) {}
    
    // Agregar arista dirigida
    void add_edge(int u, int v, T weight) {
        if (u >= 0 && u < n && v >= 0 && v < n) {
            adj[u].emplace_back(v, weight);
        }
    }
    
    // Ejecutar Dijkstra desde un nodo fuente
    std::pair<std::vector<T>, std::vector<int>> execute(int source, Metrics* M = nullptr) {
        if (source < 0 || source >= n) {
            return {{}, {}};
        }

        dist.assign(n, INF);
        pred.assign(n, -1);
        if (M) M->assign(2 * n);   // inicialización de vectores

        dist[source] = 0;
        if (M) M->assign();

        using pii = std::pair<T, int>;
        std::priority_queue<pii, std::vector<pii>, std::greater<pii>> pq;
        pq.push({0, source});
        if (M) {
            M->call();     // push
            M->assign(2);
        }

        while (!pq.empty()) {
            if (M) M->compare();

            auto [d, u] = pq.top();
            pq.pop();
            if (M) {
                M->call(2);    // top + pop
                M->assign(2);
            }

            if (d > dist[u]) {
                if (M) {
                    M->compare();
                    M->access();
                }
                continue;
            }

            for (const auto& edge : adj[u]) {
                if (M) M->access(2);

                int v = edge.to;
                T new_dist = dist[u] + edge.weight;
                if (M) {
                    M->assign(2);
                    M->arithmetic();
                    M->access();
                }

                if (new_dist < dist[v]) {
                    if (M) {
                        M->compare();
                        M->assign(2);
                        M->access(2);
                    }

                    dist[v] = new_dist;
                    pred[v] = u;
                    pq.push({new_dist, v});
                    if (M) {
                        M->call();
                        M->assign(2);
                    }
                }
            }
        }

        return {dist, pred};
    }
    
    // Reconstruir el camino más corto hasta un destino
    std::vector<int> get_shortest_path(int destination) const {
        if (destination < 0 || destination >= n || pred.empty()) {
            return {};
        }
        
        if (dist[destination] == INF) {
            return {};  // No hay camino
        }
        
        std::vector<int> path;
        int current = destination;
        
        while (current != -1) {
            path.push_back(current);
            current = pred[current];
        }
        
        std::reverse(path.begin(), path.end());
        return path;
    }
    
    // Verificar si hay camino a un destino
    bool has_path(int destination) const {
        return destination >= 0 && destination < n && 
               !dist.empty() && dist[destination] != INF;
    }
    
    // Obtener distancia a un destino
    T get_distance(int destination) const {
        if (destination >= 0 && destination < n && !dist.empty()) {
            return dist[destination];
        }
        return INF;
    }
    
    // Limpiar el grafo
    void clear() {
        for (auto& v : adj) {
            v.clear();
        }
        dist.clear();
        pred.clear();
    }
    
    // Getters
    int get_vertices() const { return n; }
    const std::vector<T>& get_distances() const { return dist; }
    const std::vector<int>& get_predecessors() const { return pred; }
    
    // Valor infinito público para comparaciones
    static T infinity() { return INF; }
};

#endif // DIJKSTRA_HPP