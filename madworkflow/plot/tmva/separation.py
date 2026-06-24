# Copyright 2023-2026 Bruno M. S. Oliveira

import colorsys
from pathlib import Path

import matplotlib.colors
import matplotlib.pyplot as plt
import pandas as pd

from ...studies import Study
from ..common import dpi


def scale_lightness(hex, scale_l):
    rgb = matplotlib.colors.ColorConverter.to_rgb(hex)
    hue, luminance, saturation = colorsys.rgb_to_hls(*rgb)
    return colorsys.hls_to_rgb(hue, min(1, luminance * scale_l), s=saturation)


def row_major_legend_items(handles, labels, ncol: int):
    nrows = (len(handles) + ncol - 1) // ncol
    order = []

    for col in range(ncol):
        for row in range(nrows):
            idx = row * ncol + col
            if idx < len(handles):
                order.append(idx)

    return [handles[idx] for idx in order], [labels[idx] for idx in order]


def plot_separation(ax, csv_path: Path, id: int, prefix: str | None = None):
    df = pd.read_csv(csv_path)
    df = df.sort_values(by=["meanMass", "thetaSquared"], ascending=[False, True])

    # Prepare Plot
    ax.set_title(f"1 - S{id}")

    ax.set_xlabel("HNL Mass / GeV")
    ax.set_xscale("log")

    ax.set_ylabel(f"1 - S{id}")
    ax.set_yscale("log")
    ax.set_ylim([1e-3, 2])

    ax.axvline(80.3692, color="lightgray", zorder=0)
    ax.axvline(91.1880, color="lightgray", zorder=0)
    ax.axvline(125.11, color="lightgray", zorder=0)

    # Plot names
    if prefix:
        label_train_a = f"{prefix} Training (ROOT)"
        label_train_b = f"{prefix} Training (TMVASurvivalRate)"

        label_valid_a = f"{prefix} Validation (ROOT)"
        label_valid_b = f"{prefix} Validation (TMVASurvivalRate)"

        label_apply_b = f"{prefix} Application (TMVASurvivalRate)"
        label_apply_c = f"{prefix} Application (TMVAApply)"

    else:
        label_train_a = "Training (ROOT)"
        label_train_b = "Training (TMVASurvivalRate)"

        label_valid_a = "Validation (ROOT)"
        label_valid_b = "Validation (TMVASurvivalRate)"

        label_apply_b = "Application (TMVASurvivalRate)"
        label_apply_c = "Application (TMVAApply)"

    # Training
    x = "meanMass"
    y = f"separation{id}TrainA"
    (line,) = ax.plot(df[x], 1 - df[y], linestyle=":", label=label_train_a)

    color = line.get_color()

    y = f"separation{id}TrainB"
    ax.plot(
        df[x], 1 - df[y], linestyle="-", color=color, alpha=0.6, label=label_train_b
    )

    # Validation
    y = f"separation{id}ValidA"
    (line,) = ax.plot(df[x], 1 - df[y], linestyle=":", label=label_valid_a)

    color = line.get_color()

    y = f"separation{id}ValidB"
    ax.plot(
        df[x], 1 - df[y], linestyle="-", color=color, alpha=0.6, label=label_valid_b
    )

    # Application
    y = f"separation{id}ApplyB"
    (line,) = ax.plot(df[x], 1 - df[y], linestyle=":", label=label_apply_b)

    color = line.get_color()

    y = f"separation{id}ApplyC"
    ax.plot(
        df[x], 1 - df[y], linestyle="-", color=color, alpha=0.6, label=label_apply_c
    )


def plot_separation_difference(ax, csv_path: Path, id: int, prefix: str | None = None):
    df = pd.read_csv(csv_path)
    df = df.sort_values(by=["meanMass", "thetaSquared"], ascending=[False, True])

    # Prepare Plot
    ax.set_title(f"ΔS{id}")

    ax.set_xlabel("HNL Mass / GeV")
    ax.set_xscale("log")

    ax.set_ylabel(f"S{id} A-B")
    ax.axhline(0, color="lightgray", zorder=0)

    ax.axvline(80.3692, color="lightgray", zorder=0)
    ax.axvline(91.1880, color="lightgray", zorder=0)
    ax.axvline(125.11, color="lightgray", zorder=0)

    if prefix:
        label_train = f"{prefix} Training (ROOT - TMVASurvivalRate)"
        label_valid = f"{prefix} Validation (ROOT - TMVASurvivalRate)"
        label_apply = f"{prefix} Application (TMVASurvivalRate - TMVAApply)"

    else:
        label_train = "Training (ROOT - TMVASurvivalRate)"
        label_valid = "Validation (ROOT - TMVASurvivalRate)"
        label_apply = "Application (TMVASurvivalRate - TMVAApply)"

    x = "meanMass"
    ya = f"separation{id}TrainA"
    yb = f"separation{id}TrainB"
    ax.plot(df[x], df[ya] - df[yb], label=label_train)

    ya = f"separation{id}ValidA"
    yb = f"separation{id}ValidB"
    ax.plot(df[x], df[ya] - df[yb], label=label_valid)

    if "Global" not in csv_path.name:
        ya = f"separation{id}ApplyB"
        yb = f"separation{id}ApplyC"
        ax.plot(df[x], df[ya] - df[yb], label=label_apply)


def separation(csv_path: Path):
    # Prepare Figure
    fig, ax = plt.subplots(2, 2, figsize=(20, 12))
    fig.suptitle(csv_path.with_suffix("")._str, fontsize=16, fontweight="bold")

    # Make Plots
    plot_separation(ax[0, 0], csv_path, 2)
    plot_separation(ax[0, 1], csv_path, 1)
    plot_separation_difference(ax[1, 0], csv_path, 2)
    plot_separation_difference(ax[1, 1], csv_path, 1)

    # Legends
    handles, labels = ax[0, 0].get_legend_handles_labels()
    handles, labels = row_major_legend_items(handles, labels, ncol=2)
    fig.legend(handles, labels, ncol=2, loc="upper right")

    diff_handles, diff_labels = ax[1, 0].get_legend_handles_labels()
    diff_handles, diff_labels = row_major_legend_items(
        diff_handles, diff_labels, ncol=1
    )
    fig.legend(diff_handles, diff_labels, ncol=1, loc="lower right")

    fig.subplots_adjust(top=0.84)

    # Save Figure
    png_path = csv_path.with_suffix(".png")
    fig.savefig(png_path, dpi=dpi, bbox_inches="tight")
    plt.close()


def separation_all(study: Study, name: str, method: str):
    # Prepare Figure
    fig, ax = plt.subplots(2, 2, figsize=(30, 16))
    fig.suptitle(f"{study.name()}/{name}-{method}", fontsize=16, fontweight="bold")

    # Make Plots
    csv_path = study.simulation_dir() / f"{name}-{method}-Global-Separation.csv"
    if csv_path.is_file():
        plot_separation(ax[0, 0], csv_path, 2, "Global")
        plot_separation(ax[0, 1], csv_path, 1, "Global")
        plot_separation_difference(ax[1, 0], csv_path, 2, "Global")
        plot_separation_difference(ax[1, 1], csv_path, 1, "Global")

    csv_path = study.simulation_dir() / f"{name}-{method}-Semilocal-Separation.csv"
    if csv_path.is_file():
        plot_separation(ax[0, 0], csv_path, 2, "Semilocal")
        plot_separation(ax[0, 1], csv_path, 1, "Semilocal")
        plot_separation_difference(ax[1, 0], csv_path, 2, "Semilocal")
        plot_separation_difference(ax[1, 1], csv_path, 1, "Semilocal")

    csv_path = study.simulation_dir() / f"{name}-{method}-Local-Separation.csv"
    if csv_path.is_file():
        plot_separation(ax[0, 0], csv_path, 2, "Local")
        plot_separation(ax[0, 1], csv_path, 1, "Local")
        plot_separation_difference(ax[1, 0], csv_path, 2, "Local")
        plot_separation_difference(ax[1, 1], csv_path, 1, "Local")

    # Legends
    handles, labels = ax[0, 0].get_legend_handles_labels()
    handles, labels = row_major_legend_items(handles, labels, ncol=2)
    fig.legend(handles, labels, ncol=2, loc="upper right")

    diff_handles, diff_labels = ax[1, 0].get_legend_handles_labels()
    diff_handles, diff_labels = row_major_legend_items(
        diff_handles, diff_labels, ncol=1
    )
    fig.legend(diff_handles, diff_labels, ncol=1, loc="lower right")

    fig.subplots_adjust(top=0.84)

    # Save Figure
    png_path = study.simulation_dir() / f"{name}-{method}-Separation.png"
    fig.savefig(png_path, dpi=dpi, bbox_inches="tight")
    plt.clf()
