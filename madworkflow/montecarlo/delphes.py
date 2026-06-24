# Copyright 2023-2026 Bruno M. S. Oliveira

import subprocess
from functools import partial
from pathlib import Path

from ..common import mg5_home
from ..runners import run_function
from ..simulation import SimulationGroup
from .compress import decompress_data

delphes_pythia_bin = mg5_home / "Delphes" / "DelphesPythia8"
delphes_hepmc2_bin = mg5_home / "Delphes" / "DelphesHepMC2"

default_pythia_card = Path("./Cards/Pythia8/configLHE.cmnd")


def run_delphes_pythia(
    banner_path: Path,
    madgraph_data: Path,
    madgraph_data_compressed: Path,
    pythia_card: Path,
    delphes_card: Path,
    delphes_data: Path,
    delphes_log: Path,
) -> None:
    if not madgraph_data.exists() and not madgraph_data_compressed.exists():
        raise FileNotFoundError(
            f"{madgraph_data} and {madgraph_data_compressed} do not exist"
        )

    # --- Get Number of Events ---
    nevents = None
    with open(banner_path) as f:
        for line in f:
            if "nevents" in line:
                nevents = line.strip().split("=")[0].strip()
                break

    if nevents is None:
        raise RuntimeError("Could not determine number of events (nevents)")

    # --- Generate Pythia card ---
    with open(default_pythia_card) as f:
        pythia_card_contents = f.read()

    pythia_card_contents = pythia_card_contents.replace(
        "Main:numberOfEvents =", f"Main:numberOfEvents = {nevents}"
    ).replace("Beams:LHEF =", f"Beams:LHEF = {madgraph_data}")

    with open(pythia_card, "w") as f:
        f.write(pythia_card_contents)

    # --- Uncompress Madgraph Data ---
    keep_madgraph_data: bool = True
    if not madgraph_data.exists():
        keep_madgraph_data = False
        decompress_data(madgraph_data_compressed, madgraph_data)

    # --- Run DelphesPythia8 ---
    with open(delphes_log, "w") as log:
        subprocess.run(
            [delphes_pythia_bin, delphes_card, pythia_card, delphes_data],
            stdout=log,
            stderr=subprocess.STDOUT,
            check=True,
        )

    # --- Cleanup ---
    if not keep_madgraph_data:
        madgraph_data.unlink()


def run_delphes_hepmc2(
    pythia_data: Path,
    pythia_data_compressed: Path,
    delphes_card: Path,
    delphes_data: Path,
    delphes_log: Path,
) -> None:
    if not pythia_data.exists() and not pythia_data_compressed.exists():
        raise FileNotFoundError(
            f"{pythia_data} and {pythia_data_compressed} do not exist"
        )

    # --- Uncompress Pythia Data ---
    keep_pythia_data: bool = True
    if not pythia_data.exists():
        keep_pythia_data = False
        decompress_data(pythia_data_compressed, pythia_data)

    # --- Run DelphesHepMC2 ---
    with open(delphes_log, "w") as log:
        subprocess.run(
            [delphes_hepmc2_bin, delphes_card, delphes_data, pythia_data],
            stdout=log,
            stderr=subprocess.STDOUT,
            check=True,
        )

    # --- Cleanup ---
    if not keep_pythia_data:
        pythia_data.unlink()


def run_delphes_single(run_dir: Path, delphes_card: Path, tag_id: int) -> None:
    run_str = run_dir.name
    tag_str = f"tag_{tag_id}"

    # --- Paths ---
    old_banner_path = run_dir / f"{run_str}_tag_1_banner.txt"
    if not old_banner_path.exists():
        raise FileNotFoundError(f"{old_banner_path} do not exist")

    banner_path = run_dir / f"{run_str}_{tag_str}_banner.txt"
    if not banner_path.exists():
        banner_path.symlink_to(old_banner_path.name)

    madgraph_data = run_dir / "unweighted_events.lhe"
    madgraph_data_compressed = madgraph_data.with_suffix(".lhe.gz")

    pythia_card = run_dir / f"{tag_str}_pythia8_card.dat"
    pythia_data = run_dir / f"{tag_str}_pythia8_events.hepmc"
    pythia_data_compressed = pythia_data.with_suffix(".hepmc.gz")

    delphes_data = run_dir / f"{tag_str}_delphes_events.root"
    delphes_log = run_dir / f"{tag_str}_delphes.log"
    if delphes_data.exists():
        raise FileExistsError(f"{delphes_data} already exists")

    # --- Run ---
    if pythia_data.exists() or pythia_data_compressed.exists():
        run_delphes_hepmc2(
            pythia_data, pythia_data_compressed, delphes_card, delphes_data, delphes_log
        )

    elif madgraph_data.exists() or madgraph_data_compressed.exists():
        run_delphes_pythia(
            banner_path,
            madgraph_data,
            madgraph_data_compressed,
            pythia_card,
            delphes_card,
            delphes_data,
            delphes_log,
        )

    else:
        raise FileNotFoundError(f"No available data in {run_dir}")


def run_delphes(simulations: SimulationGroup, delphes_card: Path, tag_id: int) -> None:
    run_delphes_single_partial = partial(
        run_delphes_single, delphes_card=delphes_card, tag_id=tag_id
    )

    run_dirs: list[Path] = simulations.run_dirs(tag_id)
    run_function(run_delphes_single_partial, run_dirs, max_workers=40)
