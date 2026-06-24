# Copyright 2023-2026 Bruno M. S. Oliveira

from pathlib import Path

import pandas as pd

from ...common import analysis_bin, backup
from ...models import columns_pspss
from ...runners import run_cmds_table, save_cmds
from ...simulation import Simulation, SimulationGroup

columns: list[str] = [
    *columns_pspss,
    "separation1TrainA",
    "separation2TrainA",
    "separation1ValidA",
    "separation2ValidA",
    "separation1TrainB",
    "separation2TrainB",
    "separation1ValidB",
    "separation2ValidB",
    "separation1ApplyB",
    "separation2ApplyB",
    "separation1ApplyC",
    "separation2ApplyC",
]


def cmd(
    dataset: Path,
    signal_reconstructed: Path,
    bkgrnd_args: list[str | Path],
) -> list[str | Path]:
    if (dataset.parent / f"{dataset.stem}-hello.csv").is_file():
        print(f"{dataset} already exists")
        return []

    return [
        analysis_bin("TMVA-Separation"),
        "--dataset",
        dataset,
        "--signal",
        signal_reconstructed,
        *bkgrnd_args,
    ]


def prepare_global(
    dataset: Path,
    signal_simulation: Simulation,
    bkgrnd_args: list[str | Path],
) -> list[list[str | Path]]:
    return [
        cmd(
            dataset,
            signal_reconstructed_run,
            bkgrnd_args,
        )
        for _, signal_reconstructed_run in signal_simulation.reconstruction_outputs()
    ]


def prepare_semilocal(
    method: str,
    signal_simulation: Simulation,
    bkgrnd_args: list[str | Path],
) -> list[list[str | Path]]:
    return [
        cmd(
            signal_simulation.tmva_dataset_semilocal(method),
            signal_reconstructed_run,
            bkgrnd_args,
        )
        for _, signal_reconstructed_run in signal_simulation.reconstruction_outputs()
    ]


def prepare_local(
    method: str,
    signal_simulation: Simulation,
    bkgrnd_args: list[str | Path],
) -> list[list[str | Path]]:
    return [
        cmd(
            signal_simulation.tmva_dataset_local(method, run_id),
            signal_reconstructed_run,
            bkgrnd_args,
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

    df["meanMass"] = pd.to_numeric(df["meanMass"], errors="coerce")
    df["thetaSquared"] = pd.to_numeric(df["thetaSquared"], errors="coerce")
    df = df.sort_values(by=["meanMass", "thetaSquared"], ascending=[True, False])

    backup(csv_path)

    print(f"Writing result to {csv_path}")
    df.to_csv(csv_path, index=False, na_rep="nan")


def separation(
    method: str,
    training: str,
    signal: SimulationGroup,
    bkgrnd_list: list[SimulationGroup],
) -> Path:
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

    cmds: list[list[str | Path]] = []
    if training == "Global":
        dataset = signal.tmva_dataset_global(method)
        cmds = [
            simulation_cmds
            for signal_simulation in signal
            for simulation_cmds in prepare_global(
                dataset, signal_simulation, bkgrnd_args
            )
        ]

    elif training == "Semilocal":
        cmds = [
            simulation_cmds
            for signal_simulation in signal
            for simulation_cmds in prepare_semilocal(
                method, signal_simulation, bkgrnd_args
            )
        ]

    elif training == "Local":
        cmds = [
            simulation_cmds
            for signal_simulation in signal
            for simulation_cmds in prepare_local(method, signal_simulation, bkgrnd_args)
        ]

    else:
        raise NameError(f"Invalid training {training}")

    csv_path = signal.analysis_output(f"{method}-{training}-Separation", "csv")

    run(cmds, csv_path)

    return csv_path
