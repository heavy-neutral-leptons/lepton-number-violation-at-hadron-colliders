# Copyright 2023-2026 Bruno M. S. Oliveira

from pathlib import Path

import matplotlib.pyplot as plt
import pandas as pd

from .common import contour_width, dpi


def width(csv_path: Path, png_path: Path):
    # Read Data
    df = pd.read_csv(csv_path)
    df = df.drop_duplicates(subset=["meanMass", "thetaSquared"], ignore_index=True)

    # Prepare Figure
    fig, ax = plt.subplots(figsize=(5, 5), layout="constrained")
    fig.suptitle(csv_path.with_suffix("")._str, fontsize=16, fontweight="bold")

    # Plots
    (bms, width) = contour_width(ax, df)

    # Colorbars
    fig.colorbar(bms).set_label("log₁₀(Simulated)")

    width_colorbar = fig.colorbar(width, location="top")
    width_colorbar.set_label("log₁₀(Decay Width / GeV)")

    # Save Figure
    print(png_path)
    fig.savefig(png_path, dpi=3 * dpi, bbox_inches="tight")
