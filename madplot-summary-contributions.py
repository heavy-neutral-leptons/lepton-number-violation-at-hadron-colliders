# Copyright 2023-2026 Bruno M. S. Oliveira

from itertools import cycle
from pathlib import Path

import matplotlib.pyplot as plt
import pandas as pd

# -----------------------------
# Load data
# -----------------------------
base_dir = Path("Trimuon@FCChh")
summary_path = Path("Events/Summary.csv")

df_prod = {
    "DY CC": pd.read_csv(base_dir / "SPSS_251209a-dy-prod.run" / summary_path),
    "DY CCj": pd.read_csv(base_dir / "SPSS_251209a-gq-prod.run" / summary_path),
    "DY NC": pd.read_csv(base_dir / "SPSS_251209a-dy_nc-prod.run" / summary_path),
    "VBF": pd.read_csv(base_dir / "SPSS_251209a-vbf-prod.run" / summary_path),
    "GF": pd.read_csv(base_dir / "SPSS_260121a-gf-prod.run" / summary_path),
    # "DY+VBF": pd.read_csv(base_dir / "SPSS_251209a-dy+vbf-prod.run" / summary_path),
    # "DY+VBF+GF": pd.read_csv(base_dir / "SPSS_260121a-dy+vbf+gf-prod.run" / summary_path),
}

df_full = {
    "DY CC": pd.read_csv(base_dir / "SPSS_251209a-dy-small.run" / summary_path),
    "VBF": pd.read_csv(base_dir / "SPSS_251209a-vbf-small.run" / summary_path),
    "DY+VBF": pd.read_csv(base_dir / "SPSS_251209a-dy+vbf-small.run" / summary_path),
}

# -----------------------------
# Styling
# -----------------------------
process_colors = {
    "DY CC": "tab:blue",
    "DY CCj": "tab:orange",
    "DY NC": "tab:green",
    "VBF": "tab:red",
    "GF": "tab:purple",
    "DY+VBF": "tab:brown",
}

process_markers = {
    "DY CC": "^",
    "DY CCj": ">",
    "DY NC": "v",
    "VBF": "o",
    "GF": "*",
    "DY+VBF": "s",
    "DY+VBF+GF": "D",
}

linestyles = cycle(["-", "--"])


# -----------------------------
# Reusable plotting function
# -----------------------------
def plot_summary(ax, dfs, title):
    for process, df in dfs.items():
        styles = cycle(linestyles)
        for theta in sorted(df["thetaSquared"].unique()):
            subset = df[df["thetaSquared"] == theta]
            ax.plot(
                subset["meanMass"],
                subset["nExpected"] / subset["thetaSquared"],
                color=process_colors[process],
                marker=process_markers[process],
                linestyle=next(styles),
                label=f"{process}, θ²={theta}",
            )

    ax.set_title(title)
    ax.grid(True, which="both", linestyle="--", alpha=0.6)

    ax.set_xlabel("Mean Mass")
    ax.set_xscale("log")

    ax.set_ylabel("Cross section / pb / |θ|²")
    ax.set_yscale("log")


# -----------------------------
# Side-by-side plots
# -----------------------------
fig, axes = plt.subplots(1, 2, figsize=(14, 6), layout="constrained", sharey=True)

plot_summary(axes[0], df_prod, "Production")
plot_summary(axes[1], df_full, "Production & Decay")

# Single legend (cleaner)
handles, labels = axes[0].get_legend_handles_labels()
fig.legend(handles, labels, loc="upper center", ncol=5)

plt.tight_layout()
plt.show()
