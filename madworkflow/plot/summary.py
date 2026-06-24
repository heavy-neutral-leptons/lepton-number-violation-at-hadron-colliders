# Copyright 2023-2026 Bruno M. S. Oliveira
from collections.abc import Callable
from pathlib import Path

import matplotlib.colors as mcolors
import matplotlib.pyplot as plt
import pandas as pd

from ..theta_squared_scaling import theta_squared_scaling_df
from .common import colormap, contour_xsection, dpi


def summary(csv_path: Path, scaling_factor_xsect: Callable[[float, float], float]):
    # Read Data
    df = pd.read_csv(csv_path)
    df = df.drop_duplicates(subset=["meanMass", "thetaSquared"], ignore_index=True)

    theta_squared_list = df.thetaSquared.astype(float).unique()
    if len(theta_squared_list) == 1:
        theta_squared_0 = theta_squared_list[0]
        theta_squared_list = [
            scaling * theta_squared_0 for scaling in [1e1, 1e2, 1e3, 1e4, 1e5, 1e6]
        ]
        df = theta_squared_scaling_df(
            df, theta_squared_0, theta_squared_list, scaling_factor_xsect
        )

    # Prepare Figure
    fig, axs = plt.subplots(1, 3, figsize=(30, 10), layout="constrained")
    fig.suptitle(csv_path.with_suffix("")._str, fontsize=16, fontweight="bold")

    # ----- Mass x Coupling Scan -----
    [bms, xsection] = contour_xsection(axs[0], df, "crossSection")

    # Colorbars
    fig.colorbar(bms).set_label("log₁₀(Simulated)")
    xsection_colorbar = fig.colorbar(xsection, location="top")
    xsection_colorbar.set_label("log₁₀(Cross Section / pb)")
    xsection_colorbar.ax.invert_xaxis()

    # ----- Mass scan -----
    axs[1].grid(True, which="both", linestyle="--", alpha=0.6)

    axs[1].set_xlabel("HNL Mass / GeV")
    axs[1].set_xscale("log")

    axs[1].set_ylabel("(Cross section / pb) / |θ|²")
    axs[1].set_yscale("log")

    axs[1].axvline(80.3692, color="lightgray", zorder=0)
    axs[1].axvline(91.1880, color="lightgray", zorder=0)
    axs[1].axvline(125.11, color="lightgray", zorder=0)

    cmap = colormap["thetaSquared"]
    norm = mcolors.LogNorm(vmin=df.thetaSquared.min(), vmax=df.thetaSquared.max())
    sm = plt.cm.ScalarMappable(norm=norm, cmap=cmap)

    for theta_squared, group in df.groupby("thetaSquared"):
        assert isinstance(theta_squared, (int, float))

        axs[1].plot(
            group["meanMass"],
            group["crossSection"] / group["thetaSquared"],
            "-o",
            color=cmap(norm(theta_squared)),
        )

    plt.colorbar(sm, ax=axs[1], location="top", label="|θ|²")

    # ----- Coupling scan -----
    axs[2].grid(True, which="both", linestyle="--", alpha=0.6)

    axs[2].set_xlabel("|θ|²")
    axs[2].set_xscale("log")

    axs[2].set_ylabel("Cross section / pb")
    axs[2].set_yscale("log")

    cmap = colormap["mass"]
    norm = mcolors.LogNorm(vmin=df.meanMass.min(), vmax=df.meanMass.max())
    sm = plt.cm.ScalarMappable(norm=norm, cmap=cmap)

    for mean_mass, group in df.groupby("meanMass"):
        assert isinstance(mean_mass, (int, float))

        axs[2].plot(
            group["thetaSquared"],
            group["crossSection"],
            "-o",
            color=cmap(norm(mean_mass)),
        )

    plt.colorbar(sm, ax=axs[2], location="top", label="HNL Mass / GeV")

    # Save Figure
    png_path: Path = csv_path.with_suffix(".png")
    fig.savefig(png_path, dpi=dpi, bbox_inches="tight")
