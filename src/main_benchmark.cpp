#include "graph_loader.hpp"
#include "dijkstra.hpp"
#include "bmssp.hpp"
#include "bellman_ford.hpp"
#include "metrics.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <numeric>

using T = long long;
namespace fs = std::filesystem;

// =====================================================
// Medición de tiempo
// =====================================================
template<typename Func>
double measure_time_ms(Func&& f) {
    auto start = std::chrono::high_resolution_clock::now();
    f();
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}

// =====================================================
// Media
// =====================================================
double mean(const std::vector<double>& v) {
    return std::accumulate(v.begin(), v.end(), 0.0) / v.size();
}

// =====================================================
// Desviación estándar
// =====================================================
double stddev(const std::vector<double>& v) {
    double m = mean(v);
    double acc = 0.0;
    for (double x : v)
        acc += (x - m) * (x - m);
    return std::sqrt(acc / v.size());
}

// =====================================================
// MAIN
// =====================================================
int main() {
    fs::create_directories("results");

    std::ofstream csv("results/benchmark_results.csv");
    csv << "graph,algorithm,mean_ms,stddev_ms,instructions\n";

    const int REPS = 5;
    const int SOURCE = 0;

    for (const auto& entry : fs::directory_iterator("data")) {
        if (entry.path().extension() != ".gr")
            continue;

        std::string graph_file = entry.path().string();
        std::cout << "\nProcesando: " << graph_file << "\n";

        // =================================================
        // Cargar grafo
        // =================================================
        GraphLoader<T> loader;
        if (!loader.load_from_file(graph_file)) {
            std::cerr << "Error cargando " << graph_file << "\n";
            continue;
        }

        int n = loader.get_vertices();
        auto edges = loader.get_edges();

        // =================================================
        // === DIJKSTRA ===
        // =================================================
        std::vector<double> times_dij;

        for (int i = 0; i < REPS; ++i) {
            Dijkstra<T> dij(n);
            for (auto& e : edges)
                dij.add_edge(e.u, e.v, e.weight);

            times_dij.push_back(
                measure_time_ms([&]() {
                    dij.execute(SOURCE);
                })
            );
        }

        double mean_dij = mean(times_dij);
        double sd_dij   = stddev(times_dij);

        // Instrucciones (una sola vez)
        Metrics md;
        Dijkstra<T> dij_m(n);
        for (auto& e : edges)
            dij_m.add_edge(e.u, e.v, e.weight);
        dij_m.execute(SOURCE, &md);

        csv << graph_file << ",Dijkstra,"
            << mean_dij << "," << sd_dij << ","
            << md.count << "\n";

        // =================================================
        // === BMSSP ===
        // =================================================
        std::vector<double> times_bms;

        for (int i = 0; i < REPS; ++i) {
            spp::bmssp<T> bms(n);
            for (auto& e : edges)
                bms.addEdge(e.u, e.v, e.weight);

            times_bms.push_back(
                measure_time_ms([&]() {
                    bms.prepare_graph(false);
                    bms.execute(SOURCE);
                })
            );
        }

        double mean_bms = mean(times_bms);
        double sd_bms   = stddev(times_bms);

        Metrics mb;
        spp::bmssp<T> bms_m(n, &mb);
        for (auto& e : edges)
            bms_m.addEdge(e.u, e.v, e.weight);
        bms_m.prepare_graph(false);
        bms_m.execute(SOURCE);

        csv << graph_file << ",BMSSP,"
            << mean_bms << "," << sd_bms << ","
            << mb.count << "\n";

        // =================================================
        // === BELLMAN-FORD ===
        // =================================================
        std::vector<double> times_bf;

        for (int i = 0; i < REPS; ++i) {
            BellmanFord<T> bf(n);
            for (auto& e : edges)
                bf.add_edge(e.u, e.v, e.weight);

            times_bf.push_back(
                measure_time_ms([&]() {
                    bf.execute(SOURCE);
                })
            );
        }

        double mean_bf = mean(times_bf);
        double sd_bf   = stddev(times_bf);

        // Instrucciones (una sola vez)
        Metrics mbf;
        BellmanFord<T> bf_m(n);
        for (auto& e : edges)
            bf_m.add_edge(e.u, e.v, e.weight);
        bf_m.execute(SOURCE, &mbf);

        csv << graph_file << ",BellmanFord,"
            << mean_bf << "," << sd_bf << ","
            << mbf.count << "\n";
    }

    csv.close();
    std::cout << "\nBenchmark finalizado. Resultados en results/benchmark_results.csv\n";
    return 0;
}
