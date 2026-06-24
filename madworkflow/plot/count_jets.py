# Copyright 2023-2026 Bruno M. S. Oliveira
from pathlib import Path

import matplotlib.pyplot as plt
import pandas as pd

from .common import dpi


def count_jets(csv_path: Path):
    # Read Data
    df = pd.read_csv(csv_path)
    df = df.drop_duplicates(subset=["meanMass", "thetaSquared"], ignore_index=True)
    df = df.sort_values(by=["meanMass", "thetaSquared"], ascending=[False, True])

    df_gen = pd.read_csv("DimuonSS@FCChh/SPSS_251209a-qq+gq+aq-Generator.csv")
    df_gen = df_gen.sort_values(
        by=["meanMass", "thetaSquared"], ascending=[False, True]
    )

    fig, axs = plt.subplots(4, 4, figsize=(20, 15), layout="constrained")
    fig.suptitle(csv_path.with_suffix("")._str, fontsize=16, fontweight="bold")

    ################################################################################
    # Number of Jets                                                               #
    ################################################################################
    ax = axs[0, 0]
    ax.set_title("Events Reconstruction Efficiency")

    ax.set_xlabel("HNL Mass / GeV")
    ax.set_xscale("log")

    ax.set_ylabel("Frequency")
    ax.set_yscale("linear")

    ax.plot(df["meanMass"], df["efficiency"], label="PT(Quarks) > 25 GeV")
    ax.plot(df["meanMass"], df["efficiency2"], label="Aligned Jets")

    ax.legend()

    ax.axvline(80.3692, color="lightgray", zorder=0)
    ax.axvline(91.1880, color="lightgray", zorder=0)
    ax.axvline(125.11, color="lightgray", zorder=0)

    ################################################################################
    # Number of Jets                                                               #
    ################################################################################
    ax = axs[0, 1]
    ax.set_title("Share of Events with n Jets")

    ax.set_xlabel("HNL Mass / GeV")
    ax.set_xscale("log")

    ax.set_ylabel("Frequency")
    ax.set_yscale("linear")

    for idx in [1, 2, 3, 4, 5]:
        ax.plot(df["meanMass"], df[f"frequency{idx}"], label=f"#Jets = {idx}")

    ax.legend(ncol=2)

    ax.axvline(80.3692, color="lightgray", zorder=0)
    ax.axvline(91.1880, color="lightgray", zorder=0)
    ax.axvline(125.11, color="lightgray", zorder=0)

    ################################################################################
    # Number of Jets                                                               #
    ################################################################################
    ax = axs[0, 2]
    ax.set_title("Share of Events with n Jets (Filtered)")

    ax.set_xlabel("HNL Mass / GeV")
    ax.set_xscale("log")

    ax.set_ylabel("Frequency")
    ax.set_yscale("linear")

    for idx in [1, 2, 3, 4, 5]:
        ax.plot(df["meanMass"], df[f"filteredFrequency{idx}"], label=f"#Jets = {idx}")

    ax.legend(ncol=2)

    ax.axvline(80.3692, color="lightgray", zorder=0)
    ax.axvline(91.1880, color="lightgray", zorder=0)
    ax.axvline(125.11, color="lightgray", zorder=0)

    ################################################################################
    # Shared best jet                                                              #
    ################################################################################
    ax = axs[0, 3]
    ax.set_title("Shared Best Jet Frequency")

    ax.set_xlabel("HNL Mass / GeV")
    ax.set_xscale("log")

    ax.set_ylabel("Frequency")
    ax.set_yscale("linear")

    ax.plot(df["meanMass"], df["sharedBestJetFrequency"])

    ax.axvline(80.3692, color="lightgray", zorder=0)
    ax.axvline(91.1880, color="lightgray", zorder=0)
    ax.axvline(125.11, color="lightgray", zorder=0)

    ################################################################################
    # Jet PT                                                                       #
    ################################################################################
    ax = axs[1, 0]
    ax.set_title("PT(Jet)")

    ax.set_xlabel("HNL Mass / GeV")
    ax.set_xscale("log")

    ax.set_ylabel("PT(Jet) / Gev")
    ax.set_yscale("log")

    ax.plot(df_gen["meanMass"], df_gen["ptW"], c="black", label="Quarks")
    for idx in [1, 2, 3, 4, 5, 12, 23]:
        ax.plot(df["meanMass"], df[f"pt{idx}"], label=f"Jet {idx}")

    ax.legend(ncol=2)

    ax.axvline(80.3692, color="lightgray", zorder=0)
    ax.axvline(91.1880, color="lightgray", zorder=0)
    ax.axvline(125.11, color="lightgray", zorder=0)

    ################################################################################
    # Jet Eta                                                                      #
    ################################################################################
    ax = axs[2, 0]
    ax.set_title("η(Jet)")

    ax.set_xlabel("HNL Mass / GeV")
    ax.set_xscale("log")

    ax.set_ylabel("|η(Jet)|")
    ax.set_yscale("linear")

    ax.plot(df_gen["meanMass"], df_gen["etaW"], c="black", label="Quarks")
    for idx in [1, 2, 3, 4, 5, 12, 23]:
        ax.plot(df["meanMass"], df[f"eta{idx}"], label=f"Jet {idx}")

    ax.legend(ncol=2)

    ax.axvline(80.3692, color="lightgray", zorder=0)
    ax.axvline(91.1880, color="lightgray", zorder=0)
    ax.axvline(125.11, color="lightgray", zorder=0)

    ################################################################################
    # Jet Mass                                                                     #
    ################################################################################
    ax = axs[3, 0]
    ax.set_title("Mass(Jet)")

    ax.set_xlabel("HNL Mass / GeV")
    ax.set_xscale("log")

    ax.set_ylabel("Mass(Jet) / GeV")
    ax.set_yscale("log")

    ax.plot(df_gen["meanMass"], df_gen["massW"], c="black", label="Quarks")
    for idx in [1, 2, 3, 4, 5, 12, 23]:
        ax.plot(df["meanMass"], df[f"mass{idx}"], label=f"Jet {idx}")

    ax.legend(ncol=2)

    ax.axvline(80.3692, color="lightgray", zorder=0)
    ax.axvline(91.1880, color="lightgray", zorder=0)
    ax.axvline(125.11, color="lightgray", zorder=0)

    ax.axhline(80.3692, color="lightgray", zorder=0)

    ################################################################################
    # ΔR(Jet, W)                                                                   #
    ################################################################################
    ax = axs[1, 1]
    ax.set_title("ΔR(Jet, W)")

    ax.set_xlabel("HNL Mass / GeV")
    ax.set_xscale("log")

    ax.set_ylabel("ΔR(Jet, W)")
    ax.set_yscale("linear")

    for idx in [1, 2, 3, 4, 5, 12, 23]:
        ax.plot(df["meanMass"], df[f"dr{idx}WW"], label=f"Jet {idx}")

    ax.legend(ncol=2)

    ax.axvline(80.3692, color="lightgray", zorder=0)
    ax.axvline(91.1880, color="lightgray", zorder=0)
    ax.axvline(125.11, color="lightgray", zorder=0)

    # Save
    png_path: Path = csv_path.with_suffix(".png")
    fig.savefig(png_path, dpi=dpi, bbox_inches="tight")

    ################################################################################
    # ΔR(Jet, Quark 1)                                                             #
    ################################################################################
    ax = axs[2, 1]
    ax.set_title("ΔR(Jet, Quark 1)")

    ax.set_xlabel("HNL Mass / GeV")
    ax.set_xscale("log")

    ax.set_ylabel("ΔR(Jet, Quark 1)")
    ax.set_yscale("linear")

    for idx in [1, 2, 3, 4, 5, 12, 23]:
        ax.plot(df["meanMass"], df[f"dr{idx}Q1"], label=f"Jet {idx}")

    ax.legend(ncol=2)

    ax.axvline(80.3692, color="lightgray", zorder=0)
    ax.axvline(91.1880, color="lightgray", zorder=0)
    ax.axvline(125.11, color="lightgray", zorder=0)

    ################################################################################
    # ΔR(Jet, Quark 2)                                                             #
    ################################################################################
    ax = axs[3, 1]
    ax.set_title("ΔR(Jet, Quark 2)")

    ax.set_xlabel("HNL Mass / GeV")
    ax.set_xscale("log")

    ax.set_ylabel("ΔR(Jet, Quark 2)")
    ax.set_yscale("linear")

    for idx in [1, 2, 3, 4, 5, 12, 23]:
        ax.plot(df["meanMass"], df[f"dr{idx}Q2"], label=f"Jet {idx}")

    ax.legend(ncol=2)

    ax.axvline(80.3692, color="lightgray", zorder=0)
    ax.axvline(91.1880, color="lightgray", zorder=0)
    ax.axvline(125.11, color="lightgray", zorder=0)

    ################################################################################
    # Best Jet (W)                                                                 #
    ################################################################################
    ax = axs[1, 2]
    ax.set_title("Best Jet (W)")

    ax.set_xlabel("HNL Mass / GeV")
    ax.set_xscale("log")

    ax.set_ylabel("Frequency")
    ax.set_yscale("linear")

    for idx in [1, 2, 3, 4, 5, 12, 23]:
        ax.plot(df["meanMass"], df[f"bestJetFrequency{idx}WW"], label=f"Jet {idx}")

    ax.legend(ncol=2)

    ax.axvline(80.3692, color="lightgray", zorder=0)
    ax.axvline(91.1880, color="lightgray", zorder=0)
    ax.axvline(125.11, color="lightgray", zorder=0)

    ################################################################################
    # Best Jet (Quark 1)                                                           #
    ################################################################################
    ax = axs[2, 2]
    ax.set_title("Best Jet (Quark 1)")

    ax.set_xlabel("HNL Mass / GeV")
    ax.set_xscale("log")

    ax.set_ylabel("Frequency")
    ax.set_yscale("linear")

    for idx in [1, 2, 3, 4, 5, 12, 23]:
        ax.plot(df["meanMass"], df[f"bestJetFrequency{idx}Q1"], label=f"Jet {idx}")

    ax.legend(ncol=2)

    ax.axvline(80.3692, color="lightgray", zorder=0)
    ax.axvline(91.1880, color="lightgray", zorder=0)
    ax.axvline(125.11, color="lightgray", zorder=0)

    ################################################################################
    # Best Jet (Quark 2)                                                           #
    ################################################################################
    ax = axs[3, 2]
    ax.set_title("Best Jet (Quark 2)")

    ax.set_xlabel("HNL Mass / GeV")
    ax.set_xscale("log")

    ax.set_ylabel("Frequency")
    ax.set_yscale("linear")

    for idx in [1, 2, 3, 4, 5, 12, 23]:
        ax.plot(df["meanMass"], df[f"bestJetFrequency{idx}Q2"], label=f"Jet {idx}")

    ax.legend(ncol=2)
    ax.axvline(80.3692, color="lightgray", zorder=0)
    ax.axvline(91.1880, color="lightgray", zorder=0)
    ax.axvline(125.11, color="lightgray", zorder=0)

    ################################################################################
    # Best Jet ΔR                                                                  #
    ################################################################################
    ax = axs[1, 3]
    ax.set_title("ΔR(Best Jet)")

    ax.set_xlabel("HNL Mass / GeV")
    ax.set_xscale("log")

    ax.set_ylabel("ΔR")
    ax.set_yscale("linear")

    ax.plot(df["meanMass"], df["drBestJetWW"], label="WW")
    ax.plot(df["meanMass"], df["drBestJetQ1"], label="Q1")
    ax.plot(df["meanMass"], df["drBestJetQ2"], label="Q2")

    ax.legend()

    ax.axvline(80.3692, color="lightgray", zorder=0)
    ax.axvline(91.1880, color="lightgray", zorder=0)
    ax.axvline(125.11, color="lightgray", zorder=0)

    ################################################################################
    # Best Jet Mass                                                                #
    ################################################################################
    ax = axs[2, 3]
    ax.set_title("Mass(Best Jet)")

    ax.set_xlabel("HNL Mass / GeV")
    ax.set_xscale("log")

    ax.set_ylabel("Mass(Jet) / GeV")
    ax.set_yscale("log")

    ax.plot(df["meanMass"], df["massBestJetWW"], label="WW")
    ax.plot(df["meanMass"], df["massBestJetQ1"], label="Q1")
    ax.plot(df["meanMass"], df["massBestJetQ2"], label="Q2")

    ax.legend()

    ax.axvline(80.3692, color="lightgray", zorder=0)
    ax.axvline(91.1880, color="lightgray", zorder=0)
    ax.axvline(125.11, color="lightgray", zorder=0)

    ax.axhline(80.3692, color="lightgray", zorder=0)

    ################################################################################
    # Save                                                                         #
    ################################################################################
    png_path: Path = csv_path.with_suffix(".png")
    fig.savefig(png_path, dpi=dpi, bbox_inches="tight")
