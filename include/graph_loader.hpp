#ifndef GRAPH_LOADER_HPP
#define GRAPH_LOADER_HPP

#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

template<typename T>
class GraphLoader {
public:
    struct Edge {
        int u, v;
        T weight;
        Edge(int u, int v, T w) : u(u), v(v), weight(w) {}
    };

private:
    int n;  // número de vértices
    int m;  // número de aristas
    std::vector<Edge> edges;

public:
    GraphLoader() : n(0), m(0) {}

    // Cargar desde archivo formato DIMACS .gr
    bool load_from_file(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: no se pudo abrir " << filename << "\n";
            return false;
        }

        edges.clear();
        n = 0;
        m = 0;
        
        std::string line;
        bool header_found = false;
        
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            
            std::istringstream iss(line);
            char type;
            iss >> type;
            
            if (type == 'c') {
                // Comentario, ignorar
                continue;
            }
            else if (type == 'p') {
                // Header: p sp n m
                std::string format;
                iss >> format >> n >> m;
                
                if (format != "sp") {
                    std::cerr << "Advertencia: formato esperado 'sp', encontrado '" 
                              << format << "'\n";
                }
                
                edges.reserve(m);
                header_found = true;
                std::cout << "Cargando grafo: " << n << " vértices, " 
                          << m << " aristas\n";
            }
            else if (type == 'a') {
                // Arista: a u v weight
                if (!header_found) {
                    std::cerr << "Error: arista antes del header 'p'\n";
                    return false;
                }
                
                int u, v;
                T weight;
                iss >> u >> v >> weight;
                
                if (u < 0 || u >= n || v < 0 || v >= n) {
                    std::cerr << "Advertencia: arista fuera de rango (" 
                              << u << ", " << v << "), ignorada\n";
                    continue;
                }
                
                edges.emplace_back(u, v, weight);
            }
            else {
                std::cerr << "Advertencia: línea desconocida '" << line << "'\n";
            }
        }
        
        file.close();
        
        if (edges.size() != m) {
            std::cerr << "Advertencia: se esperaban " << m << " aristas, "
                      << "se cargaron " << edges.size() << "\n";
        }
        
        std::cout << "Grafo cargado exitosamente: " << n << " vértices, " 
                  << edges.size() << " aristas\n";
        return true;
    }
    
    // Cargar desde formato simple (cada línea: u v weight)
    bool load_from_simple_format(const std::string& filename, int vertices) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: no se pudo abrir " << filename << "\n";
            return false;
        }

        n = vertices;
        edges.clear();
        
        std::string line;
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;
            
            std::istringstream iss(line);
            int u, v;
            T weight;
            
            if (iss >> u >> v >> weight) {
                if (u >= 0 && u < n && v >= 0 && v < n) {
                    edges.emplace_back(u, v, weight);
                }
            }
        }
        
        file.close();
        m = edges.size();
        
        std::cout << "Cargado formato simple: " << n << " vértices, " 
                  << m << " aristas\n";
        return true;
    }
    
    // Validar que el grafo sea válido
    bool validate() const {
        if (n <= 0) {
            std::cerr << "Error: número de vértices inválido\n";
            return false;
        }
        
        for (const auto& e : edges) {
            if (e.u < 0 || e.u >= n || e.v < 0 || e.v >= n) {
                std::cerr << "Error: arista inválida (" << e.u << ", " << e.v << ")\n";
                return false;
            }
            if (e.weight < 0) {
                std::cerr << "Error: peso negativo " << e.weight << "\n";
                return false;
            }
        }
        
        return true;
    }
    
    // Mostrar estadísticas del grafo
    void print_stats() const {
        std::cout << "\n=== Estadísticas del Grafo ===\n";
        std::cout << "Vértices: " << n << "\n";
        std::cout << "Aristas: " << edges.size() << "\n";
        
        if (n > 0) {
            double density = (double)edges.size() / (n * (n - 1));
            std::cout << "Densidad: " << density << "\n";
            std::cout << "Grado promedio: " << (2.0 * edges.size() / n) << "\n";
        }
        
        if (!edges.empty()) {
            T min_w = edges[0].weight;
            T max_w = edges[0].weight;
            T sum_w = 0;
            
            for (const auto& e : edges) {
                min_w = std::min(min_w, e.weight);
                max_w = std::max(max_w, e.weight);
                sum_w += e.weight;
            }
            
            std::cout << "Peso mínimo: " << min_w << "\n";
            std::cout << "Peso máximo: " << max_w << "\n";
            std::cout << "Peso promedio: " << (sum_w / edges.size()) << "\n";
        }
        std::cout << "==============================\n\n";
    }
    
    // Getters
    int get_vertices() const { return n; }
    int get_edges_count() const { return edges.size(); }
    const std::vector<Edge>& get_edges() const { return edges; }
};

#endif // GRAPH_LOADER_HPP