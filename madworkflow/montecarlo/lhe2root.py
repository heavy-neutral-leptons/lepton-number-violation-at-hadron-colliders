# Copyright 2023-2026 Bruno M. S. Oliveira

import gzip
import shutil
import subprocess
from pathlib import Path

from ..common import mg5_home
from ..runners import run_function
from ..simulation import SimulationGroup
from ..studies import delphes_card

delphes_bin = mg5_home / "Delphes" / "DelphesLHEF"
delphes_card = delphes_card["empty"]


def run_delphes_single(run_dir: Path) -> None:
    run_str = run_dir.name
    tag_str = "tag_0"

    # --- Paths ---
    old_banner_path = run_dir / f"{run_str}_tag_1_banner.txt"
    if not old_banner_path.exists():
        raise FileNotFoundError(f"{old_banner_path} do not exist")

    new_banner_path = run_dir / f"{run_str}_{tag_str}_banner.txt"
    if not new_banner_path.exists():
        new_banner_path.symlink_to(old_banner_path.name)

    madgraph_data = run_dir / "unweighted_events.lhe"
    compressed_madgraph_data = madgraph_data.with_suffix(".lhe.gz")
    if not madgraph_data.exists() and not compressed_madgraph_data.exists():
        raise FileNotFoundError(
            f"{madgraph_data} and {compressed_madgraph_data} do not exist"
        )

    delphes_data = run_dir / f"{tag_str}_delphes_events.root"
    delphes_log = run_dir / f"{tag_str}_delphes.log"
    if delphes_data.exists():
        raise FileExistsError(f"{delphes_data} already exists")

    # --- Uncompress Madgraph Data ---
    keep_madgraph_data: bool = True
    if not madgraph_data.exists():
        keep_madgraph_data = False
        with gzip.open(compressed_madgraph_data, "rb") as f_in:
            with open(madgraph_data, "wb") as f_out:
                shutil.copyfileobj(f_in, f_out)

    # --- Run DelphesLHEF ---
    with open(delphes_log, "w") as log:
        subprocess.run(
            [delphes_bin, str(delphes_card), str(delphes_data), str(madgraph_data)],
            stdout=log,
            stderr=subprocess.STDOUT,
            check=True,
        )

    # --- Cleanup ---
    if not keep_madgraph_data:
        madgraph_data.unlink()


def run_lhe2root(simulations: SimulationGroup) -> None:
    run_dirs: list[Path] = simulations.run_dirs(0)
    run_function(run_delphes_single, run_dirs, max_workers=40)
