# Copyright 2023-2026 Bruno M. S. Oliveira

from collections.abc import Callable
from pathlib import Path

import matplotlib.pyplot as plt
import pandas as pd

from ..theta_squared_scaling import theta_squared_scaling_df
from .common import contour_discoverability, contour_efficiency, contour_nevents, dpi


def plot_reconstruction_rate(ax, df: pd.DataFrame):
    df["reconstructionRate"] = df.nReconstructed / df.nSimulated
    return contour_efficiency(ax, df, "reconstructionRate")


def plot_expected_events(ax, df: pd.DataFrame):
    return contour_nevents(ax, df, "expectedEvents")


def plot_discoverability(ax, df: pd.DataFrame):
    return contour_discoverability(ax, df)


def detector(
    csv_path: Path, scaling_factor_xsect: Callable[[float, float], float] | None = None
):
    # Read Data
    df = pd.read_csv(csv_path)
    df = df.drop_duplicates(subset=["meanMass", "thetaSquared"], ignore_index=True)

    theta_squared_list = df.thetaSquared.astype(float).unique()
    if "SPSS" and len(theta_squared_list) == 1 and scaling_factor_xsect:
        theta_squared_0 = theta_squared_list[0]
        theta_squared_list = [
            scaling * theta_squared_0 for scaling in [1e1, 1e2, 1e3, 1e4, 1e5, 1e6]
        ]
        df = theta_squared_scaling_df(
            df,
            theta_squared_0,
            theta_squared_list,
            scaling_factor_xsect=scaling_factor_xsect,
        )

    # Prepare Figure
    fig, axs = plt.subplots(1, 2, figsize=(10, 5), layout="constrained")
    fig.suptitle(csv_path.with_suffix("")._str, fontsize=16, fontweight="bold")

    # Plots
    bms, reconstruction_rate = plot_reconstruction_rate(axs[0], df)
    if "significance" in df.columns:
        plot_discoverability(axs[0], df)

    _, expected_events = plot_expected_events(axs[1], df)
    if "significance" in df.columns:
        plot_discoverability(axs[1], df)

    # Colorbars
    fig.colorbar(bms, ax=axs, location="top", aspect=30).set_label("log₁₀(Simulated)")

    fig.colorbar(reconstruction_rate).set_label("Reconstruction Rate")
    fig.colorbar(expected_events).set_label("log₁₀(Expected)")

    # Save Figure
    png_path = csv_path.with_suffix(".png")
    fig.savefig(png_path, dpi=dpi, bbox_inches="tight")
