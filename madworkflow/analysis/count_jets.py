# Copyright 2023-2026 Bruno M. S. Oliveira

from pathlib import Path

import pandas as pd

from ..common import analysis_bin, backup
from ..models import columns_heavyn, columns_pspss
from ..runners import run_cmds_row, save_cmds
from ..simulation import Simulation, SimulationGroup


def jet_columns(idx: int) -> list[str]:
    return [
        f"frequency{idx}",
        f"filteredFrequency{idx}",
        f"pt{idx}",
        f"eta{idx}",
        f"mass{idx}",
        f"dr{idx}Q1",
        f"bestJetFrequency{idx}Q1",
        f"dr{idx}Q2",
        f"bestJetFrequency{idx}Q2",
        f"dr{idx}WW",
        f"bestJetFrequency{idx}WW",
    ]


analysis_columns: list[str] = [
    "efficiency",
    "efficiency2",
    "nMeanJets",
    *jet_columns(1),
    *jet_columns(2),
    *jet_columns(3),
    *jet_columns(4),
    *jet_columns(5),
    *jet_columns(12),
    *jet_columns(23),
    "drBestJetQ1",
    "massBestJetQ1",
    "drBestJetQ2",
    "massBestJetQ2",
    "drBestJetWW",
    "massBestJetWW",
    "sharedBestJetFrequency",
]
count_jets_columns: dict[str, list[str]] = {
    "Background": ["run", "crossSection", *analysis_columns],
    "HeavyN": ["run", *columns_heavyn, "crossSection", *analysis_columns],
    "SPSS": ["run", *columns_pspss, "crossSection", *analysis_columns],
}


def cmd_count_jets(
    simulation: Simulation,
    run_id: int,
    tag_id: int,
) -> list[str | Path]:
    return [
        str(analysis_bin("CountJets")),
        "--study",
        simulation.study.name(),
        "--name",
        simulation.name,
        "--run",
        str(run_id),
        "--tag",
        str(tag_id),
    ]


def prepare_count_jets(simulation: Simulation, tag_id: int) -> list[list[str | Path]]:
    return [
        cmd_count_jets(simulation, run_id, tag_id) for run_id in simulation.run_range()
    ]


def run_count_jets(cmds: list[list[str | Path]], luminosity: float, csv_path: Path):
    if not cmds:
        return

    save_cmds(cmds, csv_path.with_suffix(".sh"))
    result = run_cmds_row(cmds)

    df: pd.DataFrame = pd.DataFrame(result)

    model: str | None = None
    columns: list[str] | None = None
    for mc in count_jets_columns.items():
        if len(mc[1]) == len(df.columns):
            model, columns = mc
            break

    if model and columns:
        print(f"Identified model as {model}")
        df = df.set_axis(columns, axis=1)
    else:
        raise Exception(
            f"Failed to identify model with {len(df.columns)} columns:\n{df}"
        )

    if model == "HeavyN":
        df["meanMass"] = df["mn1"].astype(float)
        df["thetaSquared"] = (
            df["ven1"].astype(float) ** 2
            + df["vmun1"].astype(float) ** 2
            + df["vtan1"].astype(float) ** 2
        )
        df["meanWidth"] = df["wn1"]

    df["crossSectionNorm"] = df.crossSection.astype(float) / df.thetaSquared.astype(
        float
    )
    df["nExpected"] = luminosity * df.crossSection.astype(float)

    backup(csv_path)

    print(f"Writing result to {csv_path}")
    df.to_csv(csv_path, index=False, na_rep="nan")


def count_jets(simulations: SimulationGroup, tag_id: int) -> Path:
    cmds = [cmds for s in simulations for cmds in prepare_count_jets(s, tag_id)]

    luminosity = simulations.study().experiment.luminosity

    csv_path = simulations.analysis_output("CountJets", "csv")

    run_count_jets(cmds, luminosity, csv_path)

    return csv_path
