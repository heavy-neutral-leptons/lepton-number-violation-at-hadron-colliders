# Copyright 2023-2026 Bruno M. S. Oliveira

from pathlib import Path

import matplotlib.axes as axes
import matplotlib.pyplot as plt
import pandas as pd

from ..studies import Study
from ..theta_squared_scaling import theta_squared_scaling
from .common import contour_asymmetry, contour_xsection, dpi


def compare_summaries(
    axs: list[axes.Axes],
    lhs: tuple[str, Path],
    rhs: tuple[str, Path],
):
    df1 = pd.read_csv(lhs[1])
    df1 = df1.drop_duplicates(subset=["meanMass", "thetaSquared"], ignore_index=True)
    df1 = df1.sort_values(by=["meanMass", "thetaSquared"], ascending=[False, True])

    df2 = pd.read_csv(rhs[1])
    df2 = df2.drop_duplicates(subset=["meanMass", "thetaSquared"], ignore_index=True)
    df2 = df2.sort_values(by=["meanMass", "thetaSquared"], ascending=[False, True])

    [_, xc] = contour_xsection(axs[0], df1, "crossSection")
    plt.colorbar(xc, ax=axs[0], location="top", label=f"σ₁ / pb [{lhs[0]}]")

    [_, xc] = contour_xsection(axs[1], df2, "crossSection")
    plt.colorbar(xc, ax=axs[1], location="top", label=f"σ₂ / pb [{rhs[0]}]")

    [_, rc] = contour_asymmetry(axs[2], df1, df2, "crossSection")
    plt.colorbar(
        rc,
        ax=axs[2],
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


def is_valid_combination(lhs: tuple[str, Path], rhs: tuple[str, Path]):
    # return lhs[1].exists() and rhs[1].exists()
    return lhs[1].with_suffix(".png").exists() and rhs[1].with_suffix(".png").exists()


def compare_summaries_combinations(
    title: str,
    png_path: Path,
    combinations: list[tuple[tuple[str, Path], tuple[str, Path]]],
):
    # Determine number of rows
    nrows = 0
    for lhs, rhs in combinations:
        if is_valid_combination(lhs, rhs):
            nrows += 1

    if nrows == 0:
        return

    # Plots
    fig, axs = plt.subplots(nrows, 3, figsize=(3 * 6, nrows * 6), layout="constrained")
    fig.suptitle(title)

    if nrows == 1:
        for lhs, rhs in combinations:
            if is_valid_combination(lhs, rhs):
                compare_summaries(axs, lhs, rhs)

    else:
        row_idx = 0

        for lhs, rhs in combinations:
            if is_valid_combination(lhs, rhs):
                compare_summaries(axs[row_idx], lhs, rhs)
                row_idx += 1

    print(f"Saving figure to {png_path}")
    fig.savefig(png_path, dpi=dpi)


def compare_summaries_combinations_auto(study: Study, simulation_name: str):
    study_dir: Path = study.simulation_dir()

    if "mid" in simulation_name:
        sub_simulations: dict[str, Path] = {
            "On-shell + No cuts": study_dir / f"{simulation_name}-nocuts-Summary.csv",
            "On-shell + Cuts": study_dir / f"{simulation_name}-Summary.csv",
            "NP + No cuts": study_dir / f"{simulation_name}-nocuts-np-Summary.csv",
            "NP + Cuts": study_dir / f"{simulation_name}-np-Summary.csv",
        }
    else:
        sub_simulations: dict[str, Path] = {
            "On-shell + No cuts": study_dir
            / f"{simulation_name}-nocuts.run/Events/Summary.csv",
            "On-shell + Cuts": study_dir / f"{simulation_name}.run/Events/Summary.csv",
            "NP + No cuts": study_dir
            / f"{simulation_name}-nocuts-np.run/Events/Summary.csv",
            "NP + Cuts": study_dir / f"{simulation_name}-np.run/Events/Summary.csv",
        }

    # ----- Cuts -----
    combinations: list[tuple[tuple[str, Path], tuple[str, Path]]] = [
        (
            ("On-shell + Cuts", sub_simulations["On-shell + Cuts"]),
            ("On-shell + No cuts", sub_simulations["On-shell + No cuts"]),
        ),
        (
            ("NP + Cuts", sub_simulations["NP + Cuts"]),
            ("NP + No cuts", sub_simulations["NP + No cuts"]),
        ),
    ]

    png_path = study_dir / f"{simulation_name}-Compare-Cuts.png"
    compare_summaries_combinations(
        str(study_dir / simulation_name), png_path, combinations
    )

    # ----- MG5_aMC Schemes -----
    combinations: list[tuple[tuple[str, Path], tuple[str, Path]]] = [
        (
            ("On-shell + Cuts", sub_simulations["On-shell + Cuts"]),
            ("NP + Cuts", sub_simulations["NP + Cuts"]),
        ),
        (
            ("On-shell + No cuts", sub_simulations["On-shell + No cuts"]),
            ("NP + No cuts", sub_simulations["NP + No cuts"]),
        ),
    ]

    png_path = study_dir / f"{simulation_name}-Compare-Scheme.png"
    compare_summaries_combinations(
        str(study_dir / simulation_name), png_path, combinations
    )

    # ----- Scaling -----
    combinations: list[tuple[tuple[str, Path], tuple[str, Path]]] = []

    for title, path in sub_simulations.items():
        if path.with_suffix(".png").exists():
            scaled_path = theta_squared_scaling(path, study.scaling_factor_xsect)
            combinations.append(((title, path), (title + " (Scaled)", scaled_path)))

    png_path = study_dir / f"{simulation_name}-Compare-Scaling.png"
    compare_summaries_combinations(
        str(study_dir / simulation_name), png_path, combinations
    )
