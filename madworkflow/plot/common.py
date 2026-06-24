# Copyright 2023-2026 Bruno M. S. Oliveira

import matplotlib.axes as axes
import matplotlib.colors as colors
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
from matplotlib.collections import PathCollection
from matplotlib.contour import QuadContourSet

from ..common import asymmetry, seesawi_theta_squared

dpi = 100

colormap: dict[str, colors.Colormap] = {
    "mass": plt.get_cmap("summer"),
    "thetaSquared": plt.get_cmap("winter"),
    "simulated_events": plt.get_cmap("copper"),
    "width": plt.get_cmap("spring"),
    "xsection": plt.get_cmap("viridis"),
    "asymmetry": plt.get_cmap("cool"),
    "nevents": plt.get_cmap("viridis"),
    "efficiency": plt.get_cmap("cividis"),
    "significance": plt.get_cmap("plasma"),
    "deltam": plt.get_cmap("inferno"),
    "mv": plt.get_cmap("magma"),
}

levels: dict[str, list[float]] = {
    "nevents": list(np.logspace(0, 10, 11)),
    "width": list(np.logspace(-12, 8, 16)),
    "xsection": list(np.logspace(-15, 6, 22)),
    "asymmetry": list(np.linspace(-1.05, +1.05, 22)),
    "efficiency": list(np.linspace(0, 1, 11)),
    "significance": [2],
}


def __prepare(ax: axes.Axes, df: pd.DataFrame) -> PathCollection:
    xlim = (df.meanMass.min(), df.meanMass.max())
    ylim = (df.thetaSquared.min(), df.thetaSquared.max())
    __prepare_axis(ax, xlim, ylim)

    __yukawa(
        ax,
        [
            (8 * np.pi, ("g", 0.8)),
            (4 * np.pi, ("r", 0.8)),
            (np.sqrt(4 * np.pi), ("r", 0.8)),
            (1, ("r", 0.8)),
        ],
    )

    __contour_width_mass(ax, df)

    return __points(ax, df)


def __prepare_axis(ax: axes.Axes, xlim: tuple, ylim: tuple):
    ax.set_xlabel("log₁₀(HNL Mass / GeV)")
    ax.set_xlim(tuple(np.log10(xlim)))

    ax.set_ylabel("log₁₀(|θ|²)")
    ax.set_ylim(tuple(np.log10(ylim)))


def __yukawa(ax: axes.Axes, yukawas):
    xlist = np.logspace(ax.get_xlim()[0], ax.get_xlim()[1], 100)

    for yukawa, color in yukawas:
        ylist = seesawi_theta_squared(yukawa, xlist)

        xlist_log = [np.log10(x) for x, y in zip(xlist, ylist) if y > 0]
        ylist_log = [np.log10(y) for y in ylist if y > 0]

        ax.plot(xlist_log, ylist_log, color=color, zorder=1.4)


def __contour_width_mass(ax: axes.Axes, df: pd.DataFrame):
    df["mwR"] = df["meanMass"] / df["meanWidth"]

    x = np.log10(df.meanMass.unique())
    y = np.log10(df.thetaSquared.unique())
    z = df.loc[:, "mwR"].values.reshape(len(x), len(y)).T

    return ax.contour(x, y, z, levels=[1], colors="black", zorder=1.41)


def __points(ax: axes.Axes, df: pd.DataFrame) -> PathCollection:
    x = np.log10(df.meanMass)
    y = np.log10(df.thetaSquared)

    if "nSimulated" in df.columns:
        z = np.log10(df.nSimulated)
        color = None
        cmap = colormap["simulated_events"]
    else:
        z = None
        color = "black"
        cmap = None

    return ax.scatter(x, y, c=z, s=1, color=color, cmap=cmap, zorder=1.5)


def seesawi_contour(
    ax: axes.Axes,
    df: pd.DataFrame,
    z_column: str,
    levels: int | list[float] | None = None,
    colors: (
        str
        | list[str]
        | tuple[float, float, float, float]
        | list[tuple[float, float, float, float]]
        | None
    ) = None,
    cmap: str | colors.Colormap | None = None,
    linestyles: None | str = None,
    zorder: float = 1.31,
    filled: bool = True,
    log: bool = True,
    integer_z: bool = False,
) -> tuple[PathCollection, QuadContourSet]:
    points = __prepare(ax, df)

    df = df.sort_values(by=["meanMass", "thetaSquared"], ascending=[False, True])
    x = np.log10(df.meanMass.unique())
    y = np.log10(df.thetaSquared.unique())
    z = df.loc[:, z_column].values.reshape(len(x), len(y)).T

    if log:
        if integer_z:
            z = np.maximum(z, 1)

        z = np.log10(z)

        if isinstance(levels, list):
            levels = list(np.log10(levels))

    if filled:
        contour = ax.contourf(
            x, y, z, levels=levels, colors=colors, cmap=cmap, zorder=zorder
        )
    else:
        contour = ax.contour(
            x,
            y,
            z,
            levels=levels,
            colors=colors,
            cmap=cmap,
            linestyles=linestyles,
            zorder=zorder,
        )

    return (points, contour)


def contour_width(
    ax: axes.Axes, df: pd.DataFrame
) -> tuple[PathCollection, QuadContourSet]:
    return seesawi_contour(ax, df, "meanWidth", levels["width"], cmap=colormap["width"])


def contour_xsection(
    ax: axes.Axes, df: pd.DataFrame, z: str
) -> tuple[PathCollection, QuadContourSet]:
    return seesawi_contour(ax, df, z, levels["xsection"], cmap=colormap["xsection"])


def contour_asymmetry(
    ax: axes.Axes, df1: pd.DataFrame, df2: pd.DataFrame, z: str
) -> tuple[PathCollection, QuadContourSet]:
    df = pd.merge(
        df1, df2, on=["meanMass", "thetaSquared"], suffixes=("_1", "_2"), how="inner"
    )

    df = df.rename(columns={"meanWidth_1": "meanWidth"})

    df["asymmetry"] = asymmetry(df[f"{z}_2"], df[f"{z}_1"])

    return seesawi_contour(
        ax, df, "asymmetry", levels["asymmetry"], cmap=colormap["asymmetry"], log=False
    )


def contour_nevents(
    ax: axes.Axes, df: pd.DataFrame, z: str
) -> tuple[PathCollection, QuadContourSet]:
    return seesawi_contour(
        ax, df, z, levels["nevents"], cmap=colormap["nevents"], integer_z=True
    )


def contour_efficiency(
    ax: axes.Axes, df: pd.DataFrame, z: str
) -> tuple[PathCollection, QuadContourSet]:
    return seesawi_contour(
        ax, df, z, levels["efficiency"], cmap=colormap["efficiency"], log=False
    )


def contour_discoverability(
    ax: axes.Axes,
    df: pd.DataFrame,
    colors: (
        str
        | list[str]
        | tuple[float, float, float, float]
        | list[tuple[float, float, float, float]]
        | None
    ) = "black",
    column="significance",
    linestyles: None | str = None,
) -> tuple[PathCollection, QuadContourSet]:
    return seesawi_contour(
        ax,
        df,
        column,
        levels["significance"],
        colors=colors,
        linestyles=linestyles,
        filled=False,
    )
