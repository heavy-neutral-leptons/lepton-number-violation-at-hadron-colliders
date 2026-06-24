# Copyright 2023-2026 Bruno M. S. Oliveira

from pathlib import Path

import numpy as np
import pandas as pd

from ...common import analysis_bin, backup
from ...models import columns_pspss
from ...runners import run_cmds_table, save_cmds
from ...simulation import Simulation, SimulationGroup

columns: list[str] = [
    *columns_pspss,
    "signalFactor",
    "nTotalSignalEventsU",
    "nTotalSignalEventsS",
    "optimalCut",
    "optimalCutSignal",
    "optimalCutBkgrnd",
    "optimalCutSignificance",
    "nSignalEventsU",
    "nSignalEventsS",
    "nBackgroundEvents",
    "zExcludeSignal",
    "zExcludeSuppression",
]


def cmd(
    dataset: Path,
    signal_reconstructed: Path,
    bkgrnd_args: list[str | Path],
    luminosity: float,
    bkgrnd_factor: float,
    theta_squared_args: list[str],
    deltam: list[str] | None,
    mv: list[str] | None,
    mass_splitting_threshold: float,
) -> list[str | Path]:
    mass_splitting_scan: list[str] = []
    if deltam and len(deltam) != 0:
        mass_splitting_scan += ["--mass-splitting", *deltam]

    if mv and len(mv) != 0:
        mass_splitting_scan += ["--inverse-seesaw-mass", *mv]

    return [
        analysis_bin("TMVA-Apply"),
        "--dataset",
        dataset,
        "--signal",
        signal_reconstructed,
        *bkgrnd_args,
        "--luminosity",
        str(luminosity),
        "--background-factor",
        str(bkgrnd_factor),
        *theta_squared_args,
        *mass_splitting_scan,
        "--mass-splitting-threshold",
        str(mass_splitting_threshold),
    ]


def prepare_global(
    dataset: Path,
    signal_simulation: Simulation,
    bkgrnd_args: list[str | Path],
    bkgrnd_factor: float,
    theta_squared_args: list[str],
    deltam: list[str] | None,
    mv: list[str] | None,
) -> list[list[str | Path]]:
    return [
        cmd(
            dataset,
            signal_reconstructed_run,
            bkgrnd_args,
            signal_simulation.study.experiment.luminosity,
            bkgrnd_factor,
            theta_squared_args,
            deltam,
            mv,
            signal_simulation.study.experiment.mass_splitting_threshold,
        )
        for _, signal_reconstructed_run in signal_simulation.reconstruction_outputs()
    ]


def prepare_semilocal(
    method: str,
    signal_simulation: Simulation,
    bkgrnd_args: list[str | Path],
    bkgrnd_factor: float,
    theta_squared_args: list[str],
    deltam: list[str] | None,
    mv: list[str] | None,
) -> list[list[str | Path]]:
    return [
        cmd(
            signal_simulation.tmva_dataset_semilocal(method),
            signal_reconstructed_run,
            bkgrnd_args,
            signal_simulation.study.experiment.luminosity,
            bkgrnd_factor,
            theta_squared_args,
            deltam,
            mv,
            signal_simulation.study.experiment.mass_splitting_threshold,
        )
        for _, signal_reconstructed_run in signal_simulation.reconstruction_outputs()
    ]


def prepare_local(
    method: str,
    signal_simulation: Simulation,
    bkgrnd_args: list[str | Path],
    bkgrnd_factor: float,
    theta_squared_args: list[str],
    deltam: list[str] | None,
    mv: list[str] | None,
) -> list[list[str | Path]]:
    return [
        cmd(
            signal_simulation.tmva_dataset_local(method, run_id),
            signal_reconstructed_run,
            bkgrnd_args,
            signal_simulation.study.experiment.luminosity,
            bkgrnd_factor,
            theta_squared_args,
            deltam,
            mv,
            signal_simulation.study.experiment.mass_splitting_threshold,
        )
        for run_id, signal_reconstructed_run in signal_simulation.reconstruction_outputs()
    ]


def run(cmds: list[list[str | Path]], csv_path: Path):
    if not cmds:
        return

    save_cmds(cmds, csv_path.with_suffix(".sh"))
    result = run_cmds_table(cmds)
    result = [bm_list for run_list in result for bm_list in run_list]

    df: pd.DataFrame = pd.DataFrame(result)
    df = df.set_axis(columns, axis=1)

    for column in df.columns:
        df[column] = df[column].apply(pd.to_numeric, errors="coerce")

    df = df.sort_values(
        by=["meanMass", "thetaSquared", "deltam"], ascending=[True, False, True]
    )

    backup(csv_path)

    print(f"Writing result to {csv_path}")
    df.to_csv(csv_path, index=False, na_rep="nan")


def apply(
    method: str,
    training: str,
    signal: SimulationGroup,
    bkgrnd_list: list[SimulationGroup],
    deltam: list[str] | None,
    mv: list[str] | None,
) -> Path:
    cmds: list[list[str | Path]] = []

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

    theta_squared_args: list[str] = []

    if training == "Global":
        dataset = signal.tmva_dataset_global(method)
        cmds += [
            sim_cmds
            for signal_simulation in signal
            for sim_cmds in prepare_global(
                dataset,
                signal_simulation,
                bkgrnd_args,
                bkgrnd_factor,
                theta_squared_args,
                deltam,
                mv,
            )
        ]

    elif training == "Semilocal":
        cmds += [
            sim_cmds
            for signal_simulation in signal
            for sim_cmds in prepare_semilocal(
                method,
                signal_simulation,
                bkgrnd_args,
                bkgrnd_factor,
                theta_squared_args,
                deltam,
                mv,
            )
        ]

    elif training == "Local":
        cmds += [
            sim_cmds
            for signal_simulation in signal
            for sim_cmds in prepare_local(
                method,
                signal_simulation,
                bkgrnd_args,
                bkgrnd_factor,
                theta_squared_args,
                deltam,
                mv,
            )
        ]

    else:
        raise NameError(f"Invalid training {training}")

    csv_path = signal.analysis_output(f"{method}-{training}", "csv")

    run(cmds, csv_path)

    return csv_path


def apply_scan_theta_squared(
    method: str,
    training: str,
    signal: SimulationGroup,
    bkgrnd_list: list[SimulationGroup],
    deltam: list[str] | None,
    mv: list[str] | None,
) -> Path:
    cmds: list[list[str | Path]] = []

    theta_squared_0: float = 1e-6
    theta_squared_list: list[float] = list(np.logspace(-7, 1, 57))

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

    theta_squared_args_list: list[list[str]] = [
        [
            "--theta-squared",
            f"{theta_squared}:{signal.study().scaling_factor_xsect(theta_squared_0, theta_squared)}",
        ]
        for theta_squared in theta_squared_list
    ]

    theta_squared_args: list[str] = [
        arg
        for theta_squared_args_sublist in theta_squared_args_list
        for arg in theta_squared_args_sublist
    ]

    if training == "Global":
        dataset = signal.tmva_dataset_global(method)
        cmds += [
            sim_cmds
            for signal_simulation in signal
            for sim_cmds in prepare_global(
                dataset,
                signal_simulation,
                bkgrnd_args,
                bkgrnd_factor,
                theta_squared_args,
                deltam,
                mv,
            )
        ]

    elif training == "Semilocal":
        cmds += [
            sim_cmds
            for signal_simulation in signal
            for sim_cmds in prepare_semilocal(
                method,
                signal_simulation,
                bkgrnd_args,
                bkgrnd_factor,
                theta_squared_args,
                deltam,
                mv,
            )
        ]

    elif training == "Local":
        cmds += [
            sim_cmds
            for signal_simulation in signal
            for sim_cmds in prepare_local(
                method,
                signal_simulation,
                bkgrnd_args,
                bkgrnd_factor,
                theta_squared_args,
                deltam,
                mv,
            )
        ]

    else:
        raise NameError(f"Invalid training {training}")

    csv_path = signal.analysis_output(f"{method}-{training}", "csv")

    run(cmds, csv_path)

    return csv_path
