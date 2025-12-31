# ===========================
# Configuración
# ===========================
CXX      = g++
CXXFLAGS = -std=c++17 -Iinclude
LDFLAGS  =

# ===========================
# Ejecutables
# ===========================
GENERATOR = generate_graphs
BENCHMARK = benchmark

# ===========================
# Fuentes
# ===========================
GEN_SRC = src/main_generate_graphs.cpp
BEN_SRC = src/main_benchmark.cpp

# ===========================
# Targets por defecto
# ===========================
all: $(GENERATOR) $(BENCHMARK)

# ===========================
# Generador de grafos
# ===========================
$(GENERATOR): $(GEN_SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@

# ===========================
# Benchmark
# ===========================
$(BENCHMARK): $(BEN_SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@

# ===========================
# Ejecutar flujo completo
# ===========================
run: all
	@echo "▶ Generando grafos..."
	./$(GENERATOR)
	@echo "▶ Ejecutando benchmark..."
	./$(BENCHMARK)
	@echo "▶ Analizando resultados..."
	python3 analysis/analyze_results.py
	python3 analysis/find_breakpoints.py
# ===========================
# Limpieza
# ===========================
clean:
	rm -f $(GENERATOR) $(BENCHMARK)

clean-all: clean
	rm -rf data/* results/*.csv results/plots/*

.PHONY: all run clean clean-all
