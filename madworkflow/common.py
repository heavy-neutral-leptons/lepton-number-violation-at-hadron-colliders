# Copyright 2023-2026 Bruno M. S. Oliveira

import os
from pathlib import Path

import numpy as np

mg5_home_var = os.environ.get("MG5_HOME")
if mg5_home_var is None:
    raise RuntimeError("MG5_HOME environment variable is not set")

mg5_home: Path = Path(mg5_home_var)
mg5_amc: Path = mg5_home / "bin" / "mg5_aMC"

delphes_lhef: Path = mg5_home / "Delphes" / "DelphesLHEF"
delphes_hepmc2: Path = mg5_home / "Delphes" / "DelphesHepMC2"
delphes_pythia8: Path = mg5_home / "Delphes" / "DelphesPythia8"

analysis_dir: Path = Path("Analysis/build")

plots_dir: Path = Path("Plots")


def backup(path: Path):
    if path.exists(follow_symlinks=False):
        print(f"Backing up {path} to {path}.bak")
        path.replace(f"{path}.bak")


def significance(n_signal_events, n_bkgrnd_events):
    return n_signal_events / np.sqrt(n_signal_events + n_bkgrnd_events)


def analysis_bin(name: str) -> Path:
    return analysis_dir / name / name


def asymmetry(lhs, rhs):
    asym = (lhs - rhs) / (lhs + rhs)
    return asym


higgs_vev = 174


def seesawi_yukawa(mass, theta_squared) -> float:
    return mass * theta_squared / higgs_vev


def seesawi_mass(hnl_yukawa, theta_squared):
    hnl_dirac_mass = higgs_vev * hnl_yukawa
    return np.sqrt(hnl_dirac_mass**2 / theta_squared * (1 + theta_squared))


def seesawi_theta_squared(hnl_yukawa: float, hnl_mass):
    hnl_dirac_mass = higgs_vev * hnl_yukawa
    return hnl_dirac_mass**2 / (hnl_mass**2 - hnl_dirac_mass**2)


def sv(theta_squared: float):
    return np.sqrt(1 + theta_squared)
