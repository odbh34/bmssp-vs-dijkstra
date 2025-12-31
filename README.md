# BMSSP vs Dijkstra vs Bellman-Ford – Análisis Experimental

Proyecto experimental para el curso **Algoritmos Avanzados**, donde se comparan empíricamente tres algoritmos de caminos más cortos desde un único origen (SSSP - Single Source Shortest Path):

- **BMSSP** (Bounded Multi-Source Shortest Path)
- **Dijkstra** (algoritmo clásico con cola de prioridad)
- **Bellman-Ford** (algoritmo de relajación de aristas)

---

## 📋 Tabla de Contenidos

- [Objetivo](#objetivo)
- [Algoritmos Implementados](#algoritmos-implementados)
- [Estructura del Proyecto](#estructura-del-proyecto)
- [Requisitos](#requisitos)
- [Compilación y Ejecución](#compilación-y-ejecución)
- [Metodología Experimental](#metodología-experimental)
- [Resultados](#resultados)
- [Uso Individual](#uso-individual)

---

## 🎯 Objetivo

Evaluar empíricamente el rendimiento de los tres algoritmos en términos de:

- ⏱️ **Tiempo de ejecución** (promedio y desviación estándar)
- 🔢 **Número de instrucciones** ejecutadas
- 📊 **Comportamiento** según densidad del grafo (baja, media, alta)
- 📈 **Escalabilidad** con respecto al tamaño del grafo

---

## 🔬 Algoritmos Implementados

### Dijkstra
- **Complejidad**: O((V + E) log V) con cola de prioridad binaria
- **Características**: 
  - Requiere pesos no negativos
  - Algoritmo voraz que procesa vértices en orden de distancia
  - Eficiente para grafos densos y dispersos
- **Implementación**: Cola de prioridad (heap) con `std::priority_queue`

### Bellman-Ford
- **Complejidad**: O(V × E)
- **Características**:
  - Funciona con pesos negativos (pero no ciclos negativos)
  - Más simple pero generalmente más lento que Dijkstra
  - Relaja todas las aristas V-1 veces
  - Incluye optimización: termina antes si no hay cambios
- **Implementación**: Lista de aristas con relajación iterativa

### BMSSP
- **Complejidad**: Variable (depende de la estructura del grafo)
- **Características**:
  - Algoritmo recursivo con selección heurística de pivotes
  - Divide el problema en subproblemas más pequeños
  - Usa Dijkstra acotado como caso base
  - Corrección final con Dijkstra completo para garantizar optimalidad
- **Implementación**: Algoritmo híbrido recursivo con profundidad máxima log₂(V)

---

## 📁 Estructura del Proyecto

```
bmssp-vs-dijkstra/
├── include/                    # Headers con implementaciones
│   ├── bellman_ford.hpp       # Algoritmo Bellman-Ford
│   ├── bmssp.hpp              # Algoritmo BMSSP
│   ├── dijkstra.hpp           # Algoritmo Dijkstra
│   ├── graph_generator.hpp    # Generador de grafos sintéticos
│   ├── graph_loader.hpp       # Cargador de grafos (formato DIMACS)
│   └── metrics.hpp            # Sistema de conteo de instrucciones
│
├── src/                        # Código fuente principal
│   ├── main_benchmark.cpp     # Programa de benchmarking
│   └── main_generate_graphs.cpp # Generador de grafos
│
├── analysis/                   # Scripts de análisis
│   └── analyze_results.py      # Generación de gráficos comparativos
│
├── data/                       # Grafos generados (formato .gr)
├── results/                    # Resultados del benchmark
│   ├── benchmark_results.csv   # Datos en formato CSV
│   └── plots/                 # Gráficos generados (.png)
│
├── Makefile                    # Sistema de compilación
└── README.md                   # Este archivo
```

---

## 📦 Requisitos

### Compilador y Herramientas
- **g++** con soporte C++17 o superior
- **make** (para usar el Makefile)
- **Python 3** (versión 3.6 o superior)

### Librerías Python
```bash
pip3 install pandas matplotlib
# o alternativamente:
sudo apt install python3-pandas python3-matplotlib
```

---

## 🛠️ Compilación y Ejecución

### Compilación

Compilar todos los ejecutables:
```bash
make
```

Esto genera:
- `generate_graphs`: Generador de grafos sintéticos
- `benchmark`: Programa de benchmarking

### Ejecución Completa

Ejecutar el flujo completo (generar grafos → benchmark → análisis):
```bash
make run
```

Este comando:
1. Genera grafos con diferentes tamaños y densidades
2. Ejecuta los benchmarks para cada algoritmo
3. Genera gráficos comparativos en `results/plots/`

### Ejecución Individual

#### 1. Generar grafos
```bash
./generate_graphs
```
Genera grafos en `data/` con formato DIMACS (.gr)

#### 2. Ejecutar benchmark
```bash
./benchmark
```
Ejecuta los algoritmos sobre todos los grafos en `data/` y genera `results/benchmark_results.csv`

#### 3. Analizar resultados
```bash
python3 analysis/analyze_results.py
```
Genera gráficos comparativos en `results/plots/`

### Limpieza

Limpiar ejecutables:
```bash
make clean
```

Limpiar todo (ejecutables + datos + resultados):
```bash
make clean-all
```

---

## 📊 Metodología Experimental

### Tamaños de Grafos
- **V = 100, 500, 1000, 5000** vértices

### Densidades

| Densidad | Fórmula | Descripción |
|----------|---------|-------------|
| **Baja** | E ≈ V | Grafo disperso, casi árbol |
| **Media** | E ≈ V log V | Grafo moderadamente conectado |
| **Alta** | E ≈ V²/2 | Grafo denso, casi completo |

### Parámetros Experimentales

- **Pesos**: Enteros en el rango [1, 100] (siempre positivos)
- **Repeticiones**: 5 ejecuciones por algoritmo y grafo
- **Nodo fuente**: Siempre el vértice 0
- **Formato de grafos**: DIMACS (.gr)
- **Seed**: 42 (para reproducibilidad)

### Métricas Reportadas

1. **Tiempo promedio** (ms): Media aritmética de las 5 ejecuciones
2. **Desviación estándar** (ms): Variabilidad en el tiempo de ejecución
3. **Número de instrucciones**: Conteo aproximado de operaciones básicas

---

## 📈 Resultados

### Archivos Generados

- **`results/benchmark_results.csv`**: Datos tabulares con todas las métricas
  - Columnas: `graph`, `algorithm`, `mean_ms`, `stddev_ms`, `instructions`

- **`results/plots/`**: Gráficos comparativos
  - `time_{density}.png`: Tiempo vs tamaño del grafo
  - `stddev_{density}.png`: Variabilidad vs tamaño del grafo
  - `instructions_{density}.png`: Instrucciones vs tamaño del grafo
  - Donde `{density}` ∈ {`low`, `medium`, `high`}

### Interpretación de Resultados

Los gráficos permiten comparar los algoritmos según:

- **Escalabilidad**: Cómo crece el tiempo con el tamaño del grafo
- **Eficiencia**: Qué algoritmo es más rápido para cada densidad
- **Estabilidad**: Qué algoritmo tiene menor variabilidad
- **Costo computacional**: Número de operaciones realizadas

### Ejemplo de Análisis

Para grafos de **baja densidad**:
- Dijkstra suele ser el más eficiente
- Bellman-Ford puede ser competitivo en grafos muy pequeños
- BMSSP puede tener overhead por su estructura recursiva

Para grafos de **alta densidad**:
- Las diferencias se acentúan
- Dijkstra mantiene ventaja por su eficiencia
- Bellman-Ford puede volverse prohibitivo (O(V × E))

---

## 💻 Uso Individual

### Usar un Algoritmo en tu Código

```cpp
#include "dijkstra.hpp"  // o bellman_ford.hpp, bmssp.hpp

using T = long long;

// Crear grafo con n vértices
Dijkstra<T> dij(100);

// Agregar aristas
dij.add_edge(0, 1, 5);
dij.add_edge(1, 2, 3);
// ...

// Ejecutar desde el vértice 0
auto [distances, predecessors] = dij.execute(0);

// Obtener distancia a un vértice
T dist_to_5 = dij.get_distance(5);

// Reconstruir camino
std::vector<int> path = dij.get_shortest_path(5);
```

### Cargar un Grafo desde Archivo

```cpp
#include "graph_loader.hpp"

GraphLoader<T> loader;
if (loader.load_from_file("data/graph_100_low.gr")) {
    int n = loader.get_vertices();
    auto edges = loader.get_edges();
    
    // Usar edges para construir tu grafo
    for (const auto& e : edges) {
        // e.u, e.v, e.weight
    }
}
```

### Medir Instrucciones

```cpp
#include "metrics.hpp"

Metrics m;
Dijkstra<T> dij(n, &m);

// ... agregar aristas y ejecutar ...

std::cout << "Instrucciones: " << m.count << std::endl;
```

---

## 🔍 Notas Técnicas

### Formato DIMACS

Los grafos se almacenan en formato DIMACS estándar:
```
c Comentarios (opcionales)
p sp <vértices> <aristas>
a <origen> <destino> <peso>
```

### Optimizaciones Implementadas

- **Bellman-Ford**: Termina antes si no hay relajaciones en una iteración
- **BMSSP**: Profundidad máxima log₂(V) para evitar recursión excesiva
- **Dijkstra**: Usa cola de prioridad eficiente de la STL

### Limitaciones

- Los algoritmos asumen **pesos no negativos** (excepto Bellman-Ford que puede detectar ciclos negativos)
- El conteo de instrucciones es **aproximado** y no incluye overhead del sistema
- Los tiempos pueden variar según la carga del sistema

---

## 📝 Licencia y Créditos

Proyecto académico para el curso de **Algoritmos Avanzados**.

---

## 🐛 Solución de Problemas

### Error: "No se encuentra g++"
```bash
sudo apt install build-essential
```

### Error: "ModuleNotFoundError: No module named 'pandas'"
```bash
pip3 install pandas matplotlib --user
# o
sudo apt install python3-pip
pip3 install pandas matplotlib
```

### Los gráficos no se generan
- Verifica que `results/benchmark_results.csv` existe
- Asegúrate de ejecutar `./benchmark` antes del análisis
- Revisa que las rutas en `analyze_results.py` sean correctas

---

**¡Listo para experimentar! 🚀**
