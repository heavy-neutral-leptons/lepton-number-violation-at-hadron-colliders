# Copyright 2023-2026 Bruno M. S. Oliveira

from functools import partial
from pathlib import Path

from ...common import analysis_bin
from ...runners import run_cmd, run_function, save_cmds
from ...simulation import Simulation, SimulationGroup


def run(
    dataset: Path,
    luminosity: float,
    signal_args: list[str | Path],
    bkgrnd_args: list[str | Path],
):
    if dataset.is_dir():
        print(f"{dataset} already exists")
        return

    print(f"Training {dataset} and sending CLI output to {dataset}.log")

    cmd: list[str | Path] = [
        analysis_bin("TMVA-Train"),
        "--dataset",
        dataset,
        "--luminosity",
        str(luminosity),
        *signal_args,
        *bkgrnd_args,
    ]

    save_cmds([cmd], Path(f"{dataset}.sh"))
    with open(f"{dataset}.log", "w") as out:
        run_cmd(cmd, out, out)


def train_global(signal: SimulationGroup, bkgrnd_args: list[str | Path], method: str):
    dataset = signal.tmva_dataset_global(method)
    if dataset.is_dir():
        return

    luminosity = signal.study().experiment.luminosity

    signal_reconstructed: list[Path] = [
        reconstruction_outputs
        for signal_simulation in signal
        for _, reconstruction_outputs in signal_simulation.reconstruction_outputs()
    ]
    signal_args: list[str | Path] = ["--signal", *signal_reconstructed]

    run(dataset, luminosity, signal_args, bkgrnd_args)


def train_semilocal(
    signal_simulation: Simulation,
    bkgrnd_args: list[str | Path],
    method: str,
):
    dataset = signal_simulation.tmva_dataset_semilocal(method)
    if dataset.is_dir():
        return

    luminosity = signal_simulation.study.experiment.luminosity

    signal_reconstructed: list[Path] = [
        signal_reconstructed_run
        for _, signal_reconstructed_run in signal_simulation.reconstruction_outputs()
    ]
    signal_args: list[str | Path] = ["--signal", *signal_reconstructed]

    run(dataset, luminosity, signal_args, bkgrnd_args)


def train_local(
    signal_simulation_run: tuple[Simulation, int],
    bkgrnd_args: list[str | Path],
    method: str,
):
    signal_simulation, run_id = signal_simulation_run

    dataset = signal_simulation.tmva_dataset_local(method, run_id)
    if dataset.is_dir():
        return

    luminosity = signal_simulation.study.experiment.luminosity

    signal_reconstructed: list[Path] = [signal_simulation.reconstruction_output(run_id)]
    signal_args: list[str | Path] = ["--signal", *signal_reconstructed]

    run(dataset, luminosity, signal_args, bkgrnd_args)


def train(
    method: str,
    training: str,
    signal: SimulationGroup,
    bkgrnd_list: list[SimulationGroup],
):
    bkgrnd_reconstructed_list: list[list[Path]] = [
        [
            bkgrnd_reconstructed_run
            for bkgrnd_simulation in bkgrnd
            for _, bkgrnd_reconstructed_run in bkgrnd_simulation.reconstruction_outputs()
        ]
        for bkgrnd in bkgrnd_list
    ]
    bkgrnd_args_list: list[list[str | Path]] = [
        ["--background", *bkgrnd_reconstructed]
        for bkgrnd_reconstructed in bkgrnd_reconstructed_list
    ]
    bkgrnd_args: list[str | Path] = [
        arg for bkgrnd_args_sublist in bkgrnd_args_list for arg in bkgrnd_args_sublist
    ]

    if training == "Global":
        train_global(signal, bkgrnd_args, method)

    elif training == "Semilocal":
        train_semilocal_cuts = partial(
            train_semilocal,
            method=method,
            bkgrnd_args=bkgrnd_args,
        )
        run_function(train_semilocal_cuts, signal)

    elif training == "Local":
        train_local_bdt = partial(
            train_local,
            method=method,
            bkgrnd_args=bkgrnd_args,
        )
        run_function(
            train_local_bdt,
            [
                (signal_simulation, run_id)
                for signal_simulation in signal
                for run_id in signal_simulation.run_range()
            ],
        )

    else:
        raise NameError(f"Invalid training {training}")
