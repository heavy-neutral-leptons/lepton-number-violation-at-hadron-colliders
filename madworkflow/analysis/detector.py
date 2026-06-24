# Copyright 2023-2026 Bruno M. S. Oliveira

from pathlib import Path

import pandas as pd

from ..common import backup, significance
from ..models import columns_pspss
from ..runners import run_cmds_row, save_cmds
from ..simulation import Simulation, SimulationGroup


def cmd_detector(
    simulation: Simulation,
    run_id: int,
    tag_id: int,
    cs_factor: float,
) -> list[str | Path]:
    return [
        simulation.study.detector_bin,
        "--name",
        simulation.name,
        "--run",
        str(run_id),
        "--tag",
        str(tag_id),
        "--cs-factor",
        str(cs_factor),
    ]


def cmd_detector_single_run(
    simulation: Simulation,
    run_id: int,
    tag_id: int,
    cs_factor: float,
) -> list[str | Path]:
    return [*cmd_detector(simulation, run_id, tag_id, cs_factor), "--singlerun"]


def prepare_detector(
    simulation: Simulation, tag_id: int, cs_factor: float
) -> list[list[str | Path]]:
    cmds: list[list[str | Path]] = []
    if simulation.single_run():
        if not simulation.reconstruction_output_single_run().exists():
            run_id = 1
            cmds = [cmd_detector_single_run(simulation, run_id, tag_id, cs_factor)]
    else:
        for run_id, reconstructed_run in simulation.reconstruction_outputs():
            if not reconstructed_run.exists():
                cmds.append(cmd_detector(simulation, run_id, tag_id, cs_factor))

    return cmds


def run_detector(
    cmds: list[list[str | Path]],
    luminosity: float,
    csv_path: Path,
    n_bkgrnd_events: float | None,
    cut_columns: list[str],
):
    if not cmds:
        return

    save_cmds(cmds, csv_path.with_suffix(".sh"))
    result = run_cmds_row(cmds)

    columns_signal: list[str] = [
        "run",
        *columns_pspss,
        "crossSection",
        "nSimulated",
        *cut_columns,
        "nReconstructed",
        "effectiveCrossSection",
    ]

    columns_bkgrnd: list[str] = [
        "run",
        "crossSection",
        "nSimulated",
        *cut_columns,
        "nReconstructed",
        "effectiveCrossSection",
    ]

    df: pd.DataFrame = pd.DataFrame(result)
    if len(df.columns) == len(columns_signal):
        df = df.set_axis(columns_signal, axis=1)

        df["meanMass"] = pd.to_numeric(df["meanMass"], errors="coerce")
        df["thetaSquared"] = pd.to_numeric(df["thetaSquared"], errors="coerce")
        df = df.sort_values(by=["meanMass", "thetaSquared"], ascending=[True, False])

    elif len(df.columns) == len(columns_bkgrnd):
        df = df.set_axis(columns_bkgrnd, axis=1)
    else:
        raise Exception(f"Invalid output\n{df}")

    df["expectedEvents"] = luminosity * df.effectiveCrossSection.astype(float)
    df["simulatedFraction"] = (
        luminosity * df.crossSection.astype(float) / df.nSimulated.astype(float)
    )
    if n_bkgrnd_events:
        df["significance"] = significance(df["expectedEvents"], n_bkgrnd_events)

    backup(csv_path)

    print(f"Writing result to {csv_path}")
    df.to_csv(csv_path, index=False, na_rep="nan")


def detector(
    simulations: SimulationGroup,
    tag_id: int,
    cs_factor: float = 1,
    n_bkgrnd_events: float | None = None,
) -> Path:
    cmds = [
        cmds for s in simulations for cmds in prepare_detector(s, tag_id, cs_factor)
    ]

    luminosity = simulations.study().experiment.luminosity

    csv_path: Path = simulations.analysis_output("Detector", "csv")

    run_detector(
        cmds,
        luminosity,
        csv_path,
        n_bkgrnd_events,
        simulations.study().detector_cut_columns,
    )

    return csv_path
