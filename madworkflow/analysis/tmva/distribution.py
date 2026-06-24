# Copyright 2023-2026 Bruno M. S. Oliveira

from pathlib import Path

from ...common import analysis_bin
from ...runners import run_cmds_table, save_cmds
from ...simulation import Simulation, SimulationGroup


def cmd(
    dataset: Path,
    signal_reconstructed: list[Path],
    bkgrnd_args: list[str | Path],
    luminosity: float,
    signal_factor: float,
    bkgrnd_factor: float,
    output: Path,
):
    if not dataset.is_dir():
        return []

    return [
        analysis_bin("TMVA-Distribution"),
        "--dataset",
        dataset,
        "--signal",
        *signal_reconstructed,
        *bkgrnd_args,
        "--luminosity",
        str(luminosity),
        "--signal-factor",
        str(signal_factor),
        "--background-factor",
        str(bkgrnd_factor),
        "--output",
        str(output),
    ]


def output_path(
    simulation: Simulation, method: str, training: str, signal_factor: float
) -> Path:
    return (
        simulation.study.plots_dir()
        / method
        / f"{simulation.name}-{signal_factor:.0e}-{training}"
    )


def distribution(
    method: str,
    training: str,
    signal: SimulationGroup,
    bkgrnd_list: list[SimulationGroup],
):

    luminosity = signal.study().experiment.luminosity

    theta_squared_list = [1e-5, 1e-4, 1e-3]

    bkgrnd_factor = signal.study().bkgrnd_factor

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

    cmds: list[list[str | Path]]
    if training == "Global":
        dataset = signal.tmva_dataset_global(method)

        cmds = [
            cmd(
                dataset,
                [
                    reconstruction_outputs[1]
                    for reconstruction_outputs in signal_simulation.reconstruction_outputs()
                ],
                bkgrnd_args,
                luminosity,
                signal.study().scaling_factor_xsect(1e-6, theta_squared),
                bkgrnd_factor,
                output_path(signal_simulation, method, training, theta_squared),
            )
            for signal_simulation in signal
            for theta_squared in theta_squared_list
        ]

    elif training == "Semilocal":
        cmds = [
            cmd(
                signal_simulation.tmva_dataset_semilocal(method),
                [
                    reconstruction_outputs[1]
                    for reconstruction_outputs in signal_simulation.reconstruction_outputs()
                ],
                bkgrnd_args,
                luminosity,
                signal.study().scaling_factor_xsect(1e-6, theta_squared),
                bkgrnd_factor,
                output_path(signal_simulation, method, training, theta_squared),
            )
            for signal_simulation in signal
            for theta_squared in theta_squared_list
        ]

    elif training == "Local":
        cmds = [
            cmd(
                signal_simulation.tmva_dataset_local(method, run_id=1),
                [signal_simulation.reconstruction_output(run_id=1)],
                bkgrnd_args,
                luminosity,
                signal.study().scaling_factor_xsect(1e-6, theta_squared),
                bkgrnd_factor,
                output_path(signal_simulation, method, training, theta_squared),
            )
            for signal_simulation in signal
            for theta_squared in theta_squared_list
        ]

    else:
        raise NameError(f"Invalid training {training}")

    cmds = [cmd for cmd in cmds if cmd]
    if not cmds:
        return

    save_cmds(
        cmds,
        signal.study().simulation_dir()
        / f"{signal.name}-Distribution-{method}-{training}.sh",
    )
    run_cmds_table(cmds)
