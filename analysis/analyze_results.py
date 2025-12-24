import pandas as pd
import matplotlib.pyplot as plt
import os
import re

# =====================================================
# Configuración
# =====================================================
CSV_FILE = "../results/benchmark.csv"
PLOTS_DIR = "../results/plots"

os.makedirs(PLOTS_DIR, exist_ok=True)

# =====================================================
# Cargar datos
# =====================================================
df = pd.read_csv(CSV_FILE)

# =====================================================
# Extraer V y densidad desde el nombre del grafo
# Ejemplo: data/graph_1000_high.gr
# =====================================================
def parse_graph_name(name):
    match = re.search(r"graph_(\d+)_(low|medium|high)", name)
    if match:
        return int(match.group(1)), match.group(2)
    return None, None

df[["V", "density"]] = df["graph"].apply(
    lambda x: pd.Series(parse_graph_name(x))
)

# Limpiar por seguridad
df = df.dropna(subset=["V", "density"])

# =====================================================
# 1️⃣ TIEMPO PROMEDIO vs |V| (por densidad)
# =====================================================
for density in ["low", "medium", "high"]:
    plt.figure(figsize=(7, 5))

    for algo in ["Dijkstra", "BMSSP"]:
        data = df[
            (df["algorithm"] == algo) &
            (df["density"] == density)
        ].sort_values("V")

        plt.plot(
            data["V"],
            data["mean_ms"],
            marker="o",
            label=algo
        )

    plt.xscale("log")
    plt.yscale("log")
    plt.xlabel("Número de vértices |V|")
    plt.ylabel("Tiempo promedio (ms)")
    plt.title(f"Tiempo vs |V| — Densidad {density}")
    plt.legend()
    plt.grid(True, which="both", linestyle="--", alpha=0.5)

    out = f"{PLOTS_DIR}/time_{density}.png"
    plt.savefig(out)
    plt.close()
    print(f"✔ Guardado {out}")

# =====================================================
# 2️⃣ DESVIACIÓN ESTÁNDAR vs |V| (por densidad)
# =====================================================
for density in ["low", "medium", "high"]:
    plt.figure(figsize=(7, 5))

    for algo in ["Dijkstra", "BMSSP"]:
        data = df[
            (df["algorithm"] == algo) &
            (df["density"] == density)
        ].sort_values("V")

        plt.plot(
            data["V"],
            data["stddev_ms"],
            marker="o",
            label=algo
        )

    plt.xscale("log")
    plt.yscale("log")
    plt.xlabel("Número de vértices |V|")
    plt.ylabel("Desviación estándar (ms)")
    plt.title(f"Variabilidad del tiempo — Densidad {density}")
    plt.legend()
    plt.grid(True, which="both", linestyle="--", alpha=0.5)

    out = f"{PLOTS_DIR}/stddev_{density}.png"
    plt.savefig(out)
    plt.close()
    print(f"✔ Guardado {out}")

# =====================================================
# 3️⃣ INSTRUCCIONES vs |V| (por densidad)
# =====================================================
for density in ["low", "medium", "high"]:
    plt.figure(figsize=(7, 5))

    for algo in ["Dijkstra", "BMSSP"]:
        data = df[
            (df["algorithm"] == algo) &
            (df["density"] == density)
        ].sort_values("V")

        plt.plot(
            data["V"],
            data["instructions"],
            marker="o",
            label=algo
        )

    plt.xscale("log")
    plt.yscale("log")
    plt.xlabel("Número de vértices |V|")
    plt.ylabel("Número de instrucciones")
    plt.title(f"Instrucciones vs |V| — Densidad {density}")
    plt.legend()
    plt.grid(True, which="both", linestyle="--", alpha=0.5)

    out = f"{PLOTS_DIR}/instructions_{density}.png"
    plt.savefig(out)
    plt.close()
    print(f"✔ Guardado {out}")

print("\n🎉 Análisis completado. Revisa results/plots/")
