# Copyright 2023-2026 Bruno M. S. Oliveira

from pathlib import Path

import matplotlib.cm as cm
import matplotlib.colors as colors
import matplotlib.pyplot as plt
import pandas as pd

from ...studies import Study
from ..common import colormap, contour_discoverability, dpi

cmap_deltam = colormap["deltam"]
cmap_mv = colormap["mv"]


def repeated_elements(df: pd.DataFrame, col: str, n: int):
    return list(pd.Series(df[col].value_counts()[lambda x: x >= n]).index.sort_values())


def plot_discoverability(ax, df: pd.DataFrame):
    dff: pd.DataFrame = df.loc[df.deltam == 0]

    dff = dff.drop_duplicates(
        subset=["meanMass", "thetaSquared", "deltam"],
        ignore_index=True,
    )

    contour_discoverability(ax, dff, column="zExcludeSignal")


def plot_discoverability_deltam(
    ax, df: pd.DataFrame, norm: colors.LogNorm, deltam: float
):
    dff: pd.DataFrame = df.loc[df.deltam == deltam]

    dff = dff.drop_duplicates(
        subset=["meanMass", "thetaSquared", "deltam"],
        ignore_index=True,
    )

    colors = [cmap_deltam(norm(deltam))]
    contour_discoverability(ax, dff, colors, column="zExcludeSignal")
    contour_discoverability(
        ax, dff, colors, column="zExcludeSuppression", linestyles="--"
    )


def plot_discoverability_mv(ax, df: pd.DataFrame, norm: colors.LogNorm, mv: float):
    dff: pd.DataFrame = df.loc[df.inverseSeesawMass == mv]
    dff = dff.drop_duplicates(
        subset=["meanMass", "thetaSquared", "inverseSeesawMass"],
        ignore_index=True,
    )

    colors = [cmap_mv(norm(mv))]
    contour_discoverability(ax, dff, colors, column="zExcludeSignal")
    contour_discoverability(
        ax, dff, colors, column="zExcludeSuppression", linestyles="--"
    )


def plot_discoverability_only(csv_path: Path, ax):
    # Read Data
    df = pd.read_csv(csv_path)

    scan_deltam = repeated_elements(df, "deltam", 1000)
    scan_deltam = [deltam for deltam in scan_deltam if deltam > 0]

    scan_mv = repeated_elements(df, "inverseSeesawMass", 1000)
    scan_mv = [mv for mv in scan_mv if mv > 0]

    # Make Plot
    plot_discoverability(ax, df)


def plot_column(csv_path: Path, fig, ax_deltam, ax_mv):
    # Read Data
    df = pd.read_csv(csv_path)

    scan_deltam = repeated_elements(df, "deltam", 1000)
    scan_deltam = [deltam for deltam in scan_deltam if deltam > 0]
    scan_deltam.sort(reverse=True)

    scan_mv = repeated_elements(df, "inverseSeesawMass", 1000)
    scan_mv = [mv for mv in scan_mv if mv > 0]
    scan_mv.sort(reverse=True)

    # Plot Mass Splitting Scan
    if scan_deltam:
        norm_deltam = colors.LogNorm(vmin=min(scan_deltam), vmax=max(scan_deltam))
    else:
        norm_deltam = colors.LogNorm(vmin=1, vmax=2)

    plot_discoverability(ax_deltam, df)
    for deltam in scan_deltam:
        plot_discoverability_deltam(ax_deltam, df, norm_deltam, deltam)

    sm = cm.ScalarMappable(norm=norm_deltam, cmap=cmap_deltam)
    sm.set_array([])

    cbar_deltam = fig.colorbar(sm, ax=ax_deltam)
    cbar_deltam.ax.invert_yaxis()
    cbar_deltam.set_label(r"$\Delta m$ / GeV")

    #  Plot Inverse Seesaw Mass Scan
    if scan_mv:
        norm_mv = colors.LogNorm(vmin=min(scan_mv), vmax=max(scan_mv))
    else:
        norm_mv = colors.LogNorm(vmin=1, vmax=2)

    plot_discoverability(ax_mv, df)
    for mv in scan_mv:
        plot_discoverability_mv(ax_mv, df, norm_mv, mv)

    sm = cm.ScalarMappable(norm=norm_mv, cmap=cmap_mv)
    sm.set_array([])

    cbar_mv = fig.colorbar(sm, ax=ax_mv)
    cbar_mv.ax.invert_yaxis()
    cbar_mv.set_label(r"Inverse Seesaw $m_\nu$ / GeV")


def apply(csv_path: Path):
    if "DimuonSS" in str(csv_path):
        # Prepare Figure
        fig, axs = plt.subplots(2, figsize=(10, 10))
        fig.suptitle(csv_path.with_suffix("")._str, fontsize=16, fontweight="bold")

        # Make Plots
        plot_column(csv_path, fig, axs[0], axs[1])

    else:
        # Prepare Figure
        fig, ax = plt.subplots(figsize=(10, 5))
        fig.suptitle(csv_path.with_suffix("")._str, fontsize=16, fontweight="bold")

        # Make Plots
        plot_discoverability_only(csv_path, ax)

    # Save Figure
    png_path = csv_path.with_suffix(".png")
    fig.savefig(png_path, dpi=dpi, bbox_inches="tight")


def apply_all(study: Study, signal_simulation_name: str, method: str):
    # CSV Paths
    csv_path_global = (
        study.simulation_dir() / f"{signal_simulation_name}-{method}-Global.csv"
    )

    csv_path_semilocal = (
        study.simulation_dir() / f"{signal_simulation_name}-{method}-Semilocal.csv"
    )

    csv_path_local = (
        study.simulation_dir() / f"{signal_simulation_name}-{method}-Local.csv"
    )

    ncols: int = (
        csv_path_global.is_file()
        + csv_path_semilocal.is_file()
        + csv_path_local.is_file()
    )

    # Prepare Figure
    if study.process == "DimuonSS":
        fig, axs = plt.subplots(2, ncols, figsize=(ncols * 10, 10))
        fig.suptitle(
            f"{study.name()}/{signal_simulation_name}-{method}",
            fontsize=16,
            fontweight="bold",
        )

        # Make Plots
        col = 0

        if csv_path_global.is_file():
            plot_column(csv_path_global, fig, axs[0, col], axs[1, col])
            col += 1

        if csv_path_semilocal.is_file():
            plot_column(csv_path_semilocal, fig, axs[0, col], axs[1, col])
            col += 1

        if csv_path_local.is_file():
            plot_column(csv_path_local, fig, axs[0, col], axs[1, col])
            col += 1

    else:
        fig, axs = plt.subplots(1, ncols, figsize=(ncols * 10, 5))
        fig.suptitle(
            f"{study.name()}/{signal_simulation_name}-{method}",
            fontsize=16,
            fontweight="bold",
        )

        # Make Plots
        col = 0

        if csv_path_global.is_file():
            plot_discoverability_only(csv_path_global, axs[col])
            col += 1

        if csv_path_semilocal.is_file():
            plot_discoverability_only(csv_path_semilocal, axs[col])
            col += 1

        if csv_path_local.is_file():
            plot_discoverability_only(csv_path_local, axs[col])
            col += 1

    # Save Figure
    png_path = study.simulation_dir() / f"{signal_simulation_name}-{method}.png"
    fig.savefig(png_path, dpi=dpi, bbox_inches="tight")
