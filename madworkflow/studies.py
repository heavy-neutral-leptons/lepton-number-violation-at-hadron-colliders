# Copyright 2023-2026 Bruno M. S. Oliveira

import os
from collections.abc import Callable
from dataclasses import dataclass
from pathlib import Path

from .common import analysis_bin, sv
from .simulation import Simulation, SimulationGroup


################################################################################
# Experiment                                                                   #
################################################################################
@dataclass
class Experiment:
    name: str
    energy: float
    luminosity: float
    delphes_card: Path
    mass_splitting_threshold: float


################################################################################
# Study                                                                        #
################################################################################
@dataclass
class Study:
    process: str
    experiment: Experiment

    bkgrnd_factor: float

    detector_bin: Path
    detector_cut_columns: list[str]
    generator_bin: Path
    generator_cut_columns: list[str]

    scaling_factor_xsect: Callable[[float, float], float]

    def name(self) -> str:
        return f"{self.process}@{self.experiment.name}"

    def history_template_path(self) -> Path:
        return Path(f"Cards/MG5_aMC/{self.name()}.history.template")

    def simulation_dir(self) -> Path:
        return Path(self.name())

    def reconstruction_dir(self) -> Path:
        return Path(f"/tmp/Heavy-Neutrino-Antineutrino-Oscillations/{self.name()}")

    def plots_dir(self) -> Path:
        return Path(f"Plots/{self.name()}")

    def simulations(
        self, name: str, existing=False, fail_smoothly=False
    ) -> SimulationGroup:
        if existing:
            ext = "run"
        else:
            ext = "history"

        if (self.simulation_dir() / f"{name}.{ext}").exists():
            return SimulationGroup(name, [Simulation(self, name)])

        if (self.simulation_dir() / f"{name}-1.{ext}").exists():
            names = self.simulation_dir().glob(f"{name}-[1-9].{ext}")
            names = list(names)
            names = [os.path.basename(f) for f in names]
            names = [os.path.splitext(f)[0] for f in names]
            names.sort()

            return SimulationGroup(name, [Simulation(self, s) for s in names])

        if (self.simulation_dir() / f"{name}-01.{ext}").exists():
            names = self.simulation_dir().glob(f"{name}-[0-9][0-9].{ext}")
            names = list(names)
            names = [os.path.basename(f) for f in names]
            names = [os.path.splitext(f)[0] for f in names]
            names.sort()

            return SimulationGroup(name, [Simulation(self, s) for s in names])

        if fail_smoothly:
            return SimulationGroup(name, [])
        else:
            raise Exception(f"Simulation '{name}' not found in {self.name()}")

    def reconstruction_binary(self, generator: bool):
        if generator:
            return self.generator_bin
        else:
            return self.detector_bin

    def reconstruction_cut_columns(self, generator: bool) -> list[str]:
        if generator:
            return self.generator_cut_columns
        else:
            return self.detector_cut_columns

    def luminosity(self) -> float:
        return self.experiment.luminosity

    def delphes_card(self) -> Path:
        return self.experiment.delphes_card


################################################################################
# Studies                                                                      #
################################################################################
delphes_card: dict[str, Path] = {}
experiment: dict[str, Experiment] = {}
study: dict[str, Study] = {}

# ----- Dummy -----
delphes_card["empty"] = Path("./Cards/Delphes/delphes_card_empty.tcl")
experiment["Dummy"] = Experiment("Dummy", 0, 0, delphes_card["empty"], 0)

# ----- LHC  -----
# 3 ab^{-1} = 3.336 x 10^6 pb^{-1}
delphes_card["CMSII"] = Path("Cards/Delphes/CMS/CMS_PhaseII_0PU.tcl")
experiment["LHC"] = Experiment("LHC", 0, 3e6, delphes_card["CMSII"], 0)

study["DimuonSS@LHC"] = Study(
    "DimuonSS",
    experiment["LHC"],
    1,
    analysis_bin("DimuonSS-LHC-Detector"),
    [],
    analysis_bin("DimuonSS-LHC-Generator"),
    [],
    lambda theta_squared_0, theta_squared: 0,
)

study["Displaced@LHC"] = Study(
    "Displaced",
    experiment["LHC"],
    1,
    analysis_bin("Displaced-LHC-Detector"),
    [],
    analysis_bin("Displaced-LHC-Generator"),
    [],
    lambda theta_squared_0, theta_squared: 0,
)

# ----- FCC-ee -----
delphes_card["IDEA"] = Path("Cards/Delphes/FCCee/delphes_card_IDEA.tcl")

# Z-Pole (203.5 ab^{-1} = 203.5 x 10^6 pb^{-1})
experiment["Z-Pole"] = Experiment("Z-Pole", 0, 2.03542e8, delphes_card["IDEA"], 0)
experiment["Z-Decay"] = Experiment(
    "Z-Decay", 91.1880, 2.03542e8, delphes_card["IDEA"], 0
)

study["Displaced@Z-Pole"] = Study(
    "Displaced",
    experiment["Z-Pole"],
    1,
    analysis_bin("Displaced-Z-Pole-Detector"),
    [],
    analysis_bin("Displaced-Z-Pole-Generator"),
    [],
    lambda theta_squared_0, theta_squared: 0,
)


# WW-Threshold (203.5 ab^{-1} = 203.5 x 10^6 pb^{-1})
experiment["WW-Threshold"] = Experiment(
    "WW-Threshold", 0, 2.03542e8, delphes_card["IDEA"], 0
)

# ----- FCC-hh -----
# 30 ab^{-1} = 30 x 10^6 pb^{-1}
delphes_card["FCChh"] = Path("Cards/Delphes/FCChh/FCChh.tcl")
experiment["FCChh"] = Experiment("FCChh", 100, 30e6, delphes_card["FCChh"], 19.94e-9)

study["Production@FCChh"] = Study(
    "Production",
    experiment["FCChh"],
    1,
    analysis_bin("Production-FCChh-Detector"),
    [],
    analysis_bin("Production-FCChh-Generator"),
    [],
    lambda theta_squared_0, theta_squared: 0,
)


def scaling_factor_xsect_trimuon_fcchh(
    theta_squared_0: float, theta_squared: float
) -> float:
    if theta_squared == theta_squared_0:
        return 1

    theta_squared_ratio = theta_squared / theta_squared_0
    sv_ratio = sv(theta_squared) / sv(theta_squared_0)
    return theta_squared_ratio / (sv_ratio**3.1)


study["Trimuon@FCChh"] = Study(
    "Trimuon",
    experiment["FCChh"],
    1,
    analysis_bin("Trimuon-FCChh-Detector"),
    [
        "countElectronTopology",
        "countMuonTopologyDelphes",
        "countMuonTopology",
        "countReconstructed",
    ],
    analysis_bin("Trimuon-FCChh-Generator"),
    [],
    scaling_factor_xsect_trimuon_fcchh,
)

study["LeptonNumber@FCChh"] = Study(
    "LeptonNumber",
    experiment["FCChh"],
    1,
    analysis_bin("LeptonNumber-FCChh-Detector"),
    [],
    analysis_bin("LeptonNumber-FCChh-Generator"),
    [],
    lambda theta_squared_0, theta_squared: 0,
)


def scaling_factor_xsect_dimuonss_fcchh(
    theta_squared_0: float, theta_squared: float
) -> float:
    if theta_squared == theta_squared_0:
        return 1

    theta_squared_ratio = theta_squared / theta_squared_0
    sv_ratio = sv(theta_squared) / sv(theta_squared_0)
    return theta_squared_ratio / (sv_ratio**1.5)


study["DimuonSS@FCChh"] = Study(
    "DimuonSS",
    experiment["FCChh"],
    1,
    analysis_bin("DimuonSS-FCChh-Detector"),
    [
        "countElectronTopology",
        "countMuonTopologyDelphes",
        "countMuonTopology",
        "countMuonCharge",
        "countJetTopology",
        "countReconstructed",
    ],
    analysis_bin("DimuonSS-FCChh-Generator"),
    [],
    scaling_factor_xsect_dimuonss_fcchh,
)


# Functions
def fetch_study(key: str) -> Study:
    if study.__contains__(key):
        return study[key]

    raise Exception(f"No valid study was found in '{key}'")
