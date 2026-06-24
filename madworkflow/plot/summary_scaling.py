# Copyright 2023-2026 Bruno M. S. Oliveira

from pathlib import Path

import matplotlib.pyplot as plt
import pandas as pd

from ..common import asymmetry
from ..simulation import SimulationGroup
from ..theta_squared_scaling import theta_squared_scaling_df
from .common import (
    colormap,
    contour_width,
    contour_xsection,
    dpi,
    levels,
    seesawi_contour,
)


def summary_scaling(simulation: SimulationGroup):
    data_path: Path = simulation.analysis_output("Summary")
    if not data_path.with_suffix(".png").exists():
        raise FileNotFoundError(data_path.with_suffix(".png"))

    fig, axs = plt.subplots(2, 3, figsize=(18, 12), layout="constrained")
    fig.suptitle(simulation.name)

    # --------------------------------------------------------------------------------

    df1 = pd.read_csv(data_path)
    df1 = df1.sort_values(by=["meanMass", "thetaSquared"], ascending=[False, True])

    [_, xc] = contour_xsection(axs[0, 0], df1, "crossSection")
    plt.colorbar(xc, ax=axs[0, 0], location="top", label="σ₁ / pb")

    [_, xc] = contour_width(axs[1, 0], df1)
    plt.colorbar(xc, ax=axs[1, 0], location="top", label="Γ₁ / pb")

    # --------------------------------------------------------------------------------

    df2 = theta_squared_scaling_df(
        df1,
        theta_squared_0=df1.thetaSquared.min(),
        theta_squared_list=list(df1["thetaSquared"].unique()),
        scaling_factor_xsect=simulation.study().scaling_factor_xsect,
    )
    df2 = df2.sort_values(by=["meanMass", "thetaSquared"], ascending=[False, True])

    [_, xc] = contour_xsection(axs[0, 1], df2, "crossSection")
    plt.colorbar(xc, ax=axs[0, 1], location="top", label="σ₂ / pb [Scaled]")

    [_, xc] = contour_width(axs[1, 1], df2)
    plt.colorbar(xc, ax=axs[1, 1], location="top", label="Γ₂ / pb")

    # --------------------------------------------------------------------------------

    df = pd.merge(
        df1,
        df2,
        on=["meanMass", "thetaSquared"],
        suffixes=("_mg5", "_scl"),
        how="inner",
    )

    df["meanWidth"] = df["meanWidth_mg5"]

    df["xsection_asymmetry"] = asymmetry(df["crossSection_scl"], df["crossSection_mg5"])
    df["width_asymmetry"] = asymmetry(df["meanWidth_scl"], df["meanWidth_mg5"])

    df.to_csv(data_path.with_suffix(".scaled.csv"))

    [_, rc] = seesawi_contour(
        axs[0, 2],
        df,
        "xsection_asymmetry",
        levels["asymmetry"],
        cmap=colormap["asymmetry"],
        log=False,
    )

    plt.colorbar(
        rc,
        ax=axs[0, 2],
        location="top",
        label="(σ₂ - σ₁) / (σ₁ + σ₂)",
        ticks=[
            -1.05,
            -0.85,
            -0.65,
            -0.45,
            -0.25,
            0.0,
            +0.25,
            +0.45,
            +0.65,
            +0.85,
            +1.05,
        ],
    )

    [_, rc] = seesawi_contour(
        axs[1, 2],
        df,
        "width_asymmetry",
        levels["asymmetry"],
        cmap=colormap["asymmetry"],
        log=False,
    )

    plt.colorbar(
        rc,
        ax=axs[1, 2],
        location="top",
        label="(Γ₂ - Γ₁) / (Γ₁ + Γ₂)",
        ticks=[
            -1.05,
            -0.85,
            -0.65,
            -0.45,
            -0.25,
            0.0,
            +0.25,
            +0.45,
            +0.65,
            +0.85,
            +1.05,
        ],
    )

    # --------------------------------------------------------------------------------

    png_path = simulation.analysis_output("Compare-Scaling", "png")
    fig.savefig(png_path, dpi=dpi)
