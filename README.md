# BMSSP vs Dijkstra – Experimental Analysis

Proyecto experimental para el curso **Algoritmos Avanzados**, donde se compara el
algoritmo **BMSSP** contra **Dijkstra clásico** sobre grafos con distintas densidades.

---

## Objetivo

Evaluar empíricamente el rendimiento de BMSSP frente a Dijkstra en términos de:

- Tiempo de ejecución
- Número de instrucciones ejecutadas
- Comportamiento según densidad del grafo

---

## Estructura del proyecto

```text
proyectov2/
├── include/ # Implementaciones de grafos y algoritmos
├── src/ # Programas principales
├── analysis/ # Análisis en Python
├── data/ # Grafos generados 
├── results/ # CSV y gráficos 
├── Makefile
└── README.md
```

---

## Compilación

Requisitos:
- g++ con soporte C++17
- Python 3
- Librerías Python: `pandas`, `matplotlib`

Compilar todo:
```bash
make
```

---
## Ejecución

Generar grafos, ejecutar benchmarks y analizar resultados:
```bash
make run
```

## Metodología Experimental
### Tamaños de grafos
- V = 100, 500, 1000, 5000

### Densidades:
- Baja: 𝐸≈𝑉

- Media: E≈VlogV

- Alta: E≈V²/2

### Otros parámetros
Pesos enteros en el rango [1, 100]
Cada experimento se repite 5 veces

### Métricas reportadas

- Tiempo promedio

- Desviación estándar

- Número de instrucciones

---
## Resultados
Los resultados se almacenan en:
- results/benchmark_results.csv

- results/plots/

Los gráficos permiten comparar BMSSP y Dijkstra según:

- Tamaño del grafo

- Densidad

- Costo computacional
