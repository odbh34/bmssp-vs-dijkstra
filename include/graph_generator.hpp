#ifndef GRAPH_GENERATOR_HPP
#define GRAPH_GENERATOR_HPP

#include <vector>
#include <random>
#include <set>
#include <cmath>
#include <iostream>
#include <fstream>

template<typename T>
class GraphGenerator {
public:
    struct Edge {
        int u, v;
        T weight;
    };

private:
    int n;
    std::vector<Edge> edges;
    std::mt19937 rng;

    T random_weight() {
        static std::uniform_int_distribution<T> dist(1, 100);
        return dist(rng);
    }

public:
    GraphGenerator(unsigned seed = std::random_device{}())
        : n(0), rng(seed) {}

    // =====================================================
    // Grafo con densidad BAJA: E ≈ V
    // =====================================================
    void generate_low_density(int vertices) {
        n = vertices;
        edges.clear();

        int E = n;
        generate_connected_base();

        add_random_edges(E - (n - 1));
        print_info("baja");
    }

    // =====================================================
    // Grafo con densidad MEDIA: E ≈ V log V
    // =====================================================
    void generate_medium_density(int vertices) {
        n = vertices;
        edges.clear();

        int E = static_cast<int>(n * std::log2(n));
        generate_connected_base();

        add_random_edges(E - (n - 1));
        print_info("media");
    }

    // =====================================================
    // Grafo con densidad ALTA: E ≈ V² / 2
    // =====================================================
    void generate_high_density(int vertices) {
        n = vertices;
        edges.clear();

        int E = (n * n) / 2;
        std::set<std::pair<int,int>> used;

        for (int u = 0; u < n; ++u) {
            for (int v = 0; v < n; ++v) {
                if (u != v && used.size() < (size_t)E) {
                    used.insert({u, v});
                    edges.push_back({u, v, random_weight()});
                }
            }
        }
        print_info("alta");
    }

    bool save_to_file(const std::string& filename,
                  const std::string& density,
                  unsigned seed) const {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: no se pudo crear " << filename << "\n";
            return false;
        }

        // Comentarios (metadatos experimentales)
        file << "c Grafo generado para benchmark BMSSP vs Dijkstra\n";
        file << "c Vertices: " << n << "\n";
        file << "c Aristas: " << edges.size() << "\n";
        file << "c Densidad: " << density << "\n";
        file << "c Seed: " << seed << "\n";

        // DIMACS
        file << "p sp " << n << " " << edges.size() << "\n";
        for (const auto& e : edges) {
            file << "a " << e.u << " " << e.v << " " << e.weight << "\n";
        }

        file.close();
        return true;
    }
    // =====================================================
    // Getters
    // =====================================================
    int get_vertices() const { return n; }
    const std::vector<Edge>& get_edges() const { return edges; }

private:
    // Árbol base para asegurar conectividad
    void generate_connected_base() {
        std::uniform_int_distribution<int> parent_dist;

        for (int i = 1; i < n; ++i) {
            parent_dist = std::uniform_int_distribution<int>(0, i - 1);
            edges.push_back({parent_dist(rng), i, random_weight()});
        }
    }

    // Agregar aristas aleatorias sin duplicar
    void add_random_edges(int extra) {
        std::set<std::pair<int,int>> used;
        for (auto& e : edges)
            used.insert({e.u, e.v});

        std::uniform_int_distribution<int> vdist(0, n - 1);

        while (extra > 0) {
            int u = vdist(rng);
            int v = vdist(rng);
            if (u != v && !used.count({u, v})) {
                used.insert({u, v});
                edges.push_back({u, v, random_weight()});
                extra--;
            }
        }
    }

    void print_info(const std::string& type) {
        std::cout << "Grafo " << type << ": V=" << n
                  << ", E=" << edges.size() << "\n";
    }

    
};

#endif
