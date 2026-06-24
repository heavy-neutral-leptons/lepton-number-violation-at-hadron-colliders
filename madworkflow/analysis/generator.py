# Copyright 2023-2026 Bruno M. S. Oliveira

from pathlib import Path

import pandas as pd

from ..common import backup, significance
from ..models import columns_pspss
from ..runners import run_cmds_row, save_cmds
from ..simulation import Simulation, SimulationGroup

columns_signal: list[str] = [
    "run",
    *columns_pspss,
    "crossSection",
    "nSimulated",
    "nReconstructed",
    "effectiveCrossSection",
    "ptL1",
    "etaL1",
    "ptL2",
    "etaL2",
    "ptQ1",
    "etaQ1",
    "ptQ2",
    "etaQ2",
    "ptW",
    "etaW",
    "massW",
    "drQuarks",
    "drL1L2",
    "drL1Q1",
    "drL1Q2",
    "drL1W",
    "drL2Q1",
    "drL2Q2",
    "drL2W",
]


def cmd_generator(
    simulation: Simulation,
    run_id: int,
    tag_id: int,
    cs_factor: float,
) -> list[str | Path]:
    return [
        simulation.study.generator_bin,
        "--name",
        simulation.name,
        "--run",
        str(run_id),
        "--tag",
        str(tag_id),
        "--cs-factor",
        str(cs_factor),
    ]


def cmd_generator_single_run(
    simulation: Simulation,
    run_id: int,
    tag_id: int,
    cs_factor: float,
) -> list[str | Path]:
    return [
        *cmd_generator(simulation, run_id, tag_id, cs_factor),
        "--singlerun",
    ]


def prepare_generator(
    simulation: Simulation, tag_id: int, cs_factor: float
) -> list[list[str | Path]]:
    cmds: list[list[str | Path]] = []
    if simulation.single_run():
        if not simulation.reconstruction_output_single_run(True).exists():
            run_id = 1
            cmds = [cmd_generator_single_run(simulation, run_id, tag_id, cs_factor)]
    else:
        for run_id, reconstructed_run in simulation.reconstruction_outputs(True):
            if not reconstructed_run.exists():
                cmds.append(cmd_generator(simulation, run_id, tag_id, cs_factor))

    return cmds


def run_generator(
    cmds: list[list[str | Path]],
    luminosity: float,
    csv_path: Path,
    n_bkgrnd_events: float | None,
):
    if not cmds:
        return

    save_cmds(cmds, csv_path.with_suffix(".sh"))
    result = run_cmds_row(cmds)

    df: pd.DataFrame = pd.DataFrame(result)
    if len(df.columns) == len(columns_signal):
        df = df.set_axis(columns_signal, axis=1)
    else:
        raise Exception(f"Invalid output\n{df}")

    df["expectedEvents"] = luminosity * df.effectiveCrossSection.astype(float)
    if n_bkgrnd_events:
        df["significance"] = significance(df["expectedEvents"], n_bkgrnd_events)

    backup(csv_path)

    print(f"Writing result to {csv_path}")
    df.to_csv(csv_path, index=False, na_rep="nan")


def generator(
    simulations: SimulationGroup,
    tag_id: int,
    cs_factor: float = 1,
    n_bkgrnd_events: float | None = None,
) -> Path:
    cmds = [
        cmds for s in simulations for cmds in prepare_generator(s, tag_id, cs_factor)
    ]

    luminosity = simulations.study().experiment.luminosity

    csv_path: Path = simulations.analysis_output("Generator", "csv")

    run_generator(cmds, luminosity, csv_path, n_bkgrnd_events)

    return csv_path
