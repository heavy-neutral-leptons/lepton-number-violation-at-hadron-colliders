# Copyright 2023-2026 Bruno M. S. Oliveira

from pathlib import Path

from . import analysis, plot
from .simulation import SimulationGroup


def workflow(
    method: str,
    training: str,
    signal: SimulationGroup,
    bkgrnd_list: list[SimulationGroup],
    deltam: list[str] | None,
    mv: list[str] | None,
):
    print(f"Training {method} ({training})")
    analysis.tmva.train(method, training, signal, bkgrnd_list)

    if method == "BDT":
        print(f"Computing separation of {method} ({training})")
        csv_path = analysis.tmva.separation(method, training, signal, bkgrnd_list)
        plot.tmva.separation(csv_path)

    print(f"Applying {method} ({training})")
    csv_path = analysis.tmva.apply_scan_theta_squared(
        method, training, signal, bkgrnd_list, deltam, mv
    )
    plot.tmva.apply(csv_path)

    print(f"Plotting distribution of {method} ({training})")
    analysis.tmva.distribution(method, training, signal, bkgrnd_list)


def signal_background(
    todo: dict[str, bool],
    signal: SimulationGroup,
    bkgrnd_list: list[SimulationGroup],
    deltam: list[str] | None = None,
    mv: list[str] | None = None,
):
    ################################################################################
    # Reconstructed data                                                           #
    ################################################################################
    signal_reconstructed: list[Path] = [
        signal_reconstructed_run
        for signal_simulation in signal
        for _, signal_reconstructed_run in signal_simulation.reconstruction_outputs()
    ]

    bkgrnd_reconstructed: list[Path] = [
        bkgrnd_reconstructed_run
        for bkgrnd in bkgrnd_list
        for bkgrnd_simulation in bkgrnd
        for _, bkgrnd_reconstructed_run in bkgrnd_simulation.reconstruction_outputs()
    ]

    ################################################################################
    # Kinematics                                                                   #
    ################################################################################
    if todo["Kinematics"]:
        print("Performing kinematic analysis")
        kinematics_output_path: Path = (
            signal[0].study.simulation_dir() / f"{signal[0].main_name}-Kinematics.csv"
        )

        analysis.kinematics(
            signal_reconstructed, bkgrnd_reconstructed, kinematics_output_path
        )

    ################################################################################
    # TMVA                                                                         #
    ################################################################################
    # ----- Global -----
    if todo["Cuts-Global"]:
        workflow("Cuts", "Global", signal, bkgrnd_list, deltam, mv)

    if todo["BDT-Global"]:
        workflow("BDT", "Global", signal, bkgrnd_list, deltam, mv)

    # ----- Semilocal -----
    if todo["Cuts-Semilocal"]:
        workflow("Cuts", "Semilocal", signal, bkgrnd_list, deltam, mv)

    if todo["BDT-Semilocal"]:
        workflow("BDT", "Semilocal", signal, bkgrnd_list, deltam, mv)

    # ----- Local -----
    if todo["Cuts-Local"]:
        workflow("Cuts", "Local", signal, bkgrnd_list, deltam, mv)

    if todo["BDT-Local"]:
        workflow("BDT", "Local", signal, bkgrnd_list, deltam, mv)
