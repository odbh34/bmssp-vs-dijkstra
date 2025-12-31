#include "graph_generator.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <filesystem>

using T = long long;
namespace fs = std::filesystem;

int main() {
    // Crear carpetas si no existen
    fs::create_directories("data");

    std::vector<int> sizes = {100, 500, 1000, 5000};
    std::vector<std::string> densities = {"low", "medium", "high"};

    const unsigned SEED = 42;  // seed fijo para reproducibilidad
    GraphGenerator<T> gen(SEED);

    for (int V : sizes) {
        gen.generate_low_density(V);
        gen.save_to_file("data/graph_" + std::to_string(V) + "_low.gr",
                        "low", SEED);

        gen.generate_medium_density(V);
        gen.save_to_file("data/graph_" + std::to_string(V) + "_medium.gr",
                        "medium", SEED);

        gen.generate_high_density(V);
        gen.save_to_file("data/graph_" + std::to_string(V) + "_high.gr",
                        "high", SEED);
    }


    std::cout << "\nTodos los grafos fueron generados en /data\n";
    return 0;
}
