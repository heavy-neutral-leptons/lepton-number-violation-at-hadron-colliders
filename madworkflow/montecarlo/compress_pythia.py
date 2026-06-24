# Copyright 2023-2026 Bruno M. S. Oliveira

from functools import partial
from pathlib import Path

from ..runners import run_function
from ..simulation import SimulationGroup
from .compress import compress_data


def compress_pythia_data_single(run_dir: Path, tag_id: int):
    tag_str = f"tag_{tag_id}"
    pythia_data = run_dir / f"{tag_str}_pythia8_events.hepmc"
    pythia_data_compressed = pythia_data.with_suffix(".hepmc.gz")

    if pythia_data.exists() and not pythia_data_compressed.exists():
        compress_data(pythia_data, pythia_data_compressed, False)

    if pythia_data.exists() and pythia_data_compressed.exists():
        pythia_data.unlink()


def compress_pythia_data(simulations: SimulationGroup, tag_id: int) -> None:
    run_delphes_single_partial = partial(compress_pythia_data_single, tag_id=tag_id)

    run_dirs: list[Path] = simulations.run_dirs()
    run_function(run_delphes_single_partial, run_dirs, max_workers=40)
