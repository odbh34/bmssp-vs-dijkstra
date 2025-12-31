import pandas as pd
import re
from pathlib import Path

CSV_FILE = "results/benchmark.csv"
OUT_FILE = "results/breakpoints.csv"

# -----------------------------------------
# Extraer V y densidad desde el nombre del grafo
# -----------------------------------------
def parse_graph_info(graph_name):
    """
    Ejemplo:
    data/graph_1000_medium.gr -> (1000, 'medium')
    """
    m = re.search(r"graph_(\d+)_(low|medium|high)", graph_name)
    if not m:
        return None, None
    return int(m.group(1)), m.group(2)

# -----------------------------------------
# Cargar datos
# -----------------------------------------
df = pd.read_csv(CSV_FILE)

df["V"], df["density"] = zip(
    *df["graph"].apply(parse_graph_info)
)

df = df.dropna()

# -----------------------------------------
# Algoritmos y comparaciones por pares
# -----------------------------------------
algorithms = ["Dijkstra", "BMSSP", "BellmanFord"]
pairs = [
    ("Dijkstra", "BMSSP"),
    ("Dijkstra", "BellmanFord"),
    ("BMSSP", "BellmanFord")
]

results = []

# -----------------------------------------
# Calcular puntos de quiebre
# -----------------------------------------
for density in df["density"].unique():
    df_d = df[df["density"] == density]

    for a, b in pairs:
        da = df_d[df_d["algorithm"] == a].set_index("V")["mean_ms"]
        db = df_d[df_d["algorithm"] == b].set_index("V")["mean_ms"]

        common_V = sorted(set(da.index) & set(db.index))
        breakpoint_V = None

        for V in common_V:
            if da[V] >= db[V]:
                breakpoint_V = V
                break

        results.append({
            "density": density,
            "algorithm_A": a,
            "algorithm_B": b,
            "breakpoint_V": breakpoint_V
        })

# -----------------------------------------
# Guardar resultados
# -----------------------------------------
bp_df = pd.DataFrame(results)
bp_df.to_csv(OUT_FILE, index=False)

print("\n✔ Puntos de quiebre calculados\n")
print(bp_df)
print(f"\nGuardado en {OUT_FILE}")
