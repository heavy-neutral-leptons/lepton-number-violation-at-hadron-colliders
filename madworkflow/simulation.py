# Copyright 2023-2026 Bruno M. S. Oliveira

from dataclasses import dataclass
from pathlib import Path
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from .studies import Study


@dataclass
class Simulation:
    study: "Study"
    name: str

    def history_path(self) -> Path:
        return self.study.simulation_dir() / f"{self.name}.history"

    def log_path(self) -> Path:
        return self.study.simulation_dir() / f"{self.name}.log"

    def simulation_dir(self) -> Path:
        return self.study.simulation_dir() / f"{self.name}.run"

    def events_dir(self) -> Path:
        return self.simulation_dir() / "Events"

    def run_dirs(self, delphes_tag_id: int = -1) -> list[Path]:
        if False and (self.events_dir() / "run_01_decayed_1").exists():
            run_dirs = self.events_dir().glob("run_[0-9][0-9]_decayed_1")
        else:
            run_dirs = [
                *self.events_dir().glob("run_[0-9][0-9]"),
                *self.events_dir().glob("run_[0-9][0-9][0-9]"),
            ]

        run_dirs = list(run_dirs)
        run_dirs.sort()

        if delphes_tag_id >= 0:
            run_dirs = [
                r
                for r in run_dirs
                if not (r / f"tag_{delphes_tag_id}_delphes_events.root").exists()
            ]

        return run_dirs

    def n_runs(self) -> int:
        return len(self.run_dirs())

    def single_run(self) -> bool:
        return self.n_runs() == 1

    def run_range(self) -> range:
        return range(1, self.n_runs() + 1)

    def reconstruction_dir(self) -> Path:
        return self.study.reconstruction_dir() / self.name

    def reconstruction_output(self, run_id, generator: bool = False) -> Path:
        if generator:
            return self.reconstruction_dir() / f"run_{run_id:>02}-generator.root"
        else:
            return self.reconstruction_dir() / f"run_{run_id:>02}.root"

    def reconstruction_output_single_run(self, generator: bool = False) -> Path:
        if generator:
            return self.study.reconstruction_dir() / f"{self.name}-generator.root"
        else:
            return self.study.reconstruction_dir() / f"{self.name}.root"

    def analysis_output(
        self,
        analysis_name: str,
        extension: str = "csv",
    ) -> Path:
        return self.events_dir() / f"{analysis_name}.{extension}"

    def reconstruction_outputs(self, generator: bool = False) -> list[tuple[int, Path]]:
        if self.single_run():
            return [(1, self.reconstruction_output_single_run(generator))]
        else:
            return [
                (i, self.reconstruction_output(i, generator)) for i in self.run_range()
            ]

    def tmva_dataset_semilocal(self, method: str):
        return self.study.reconstruction_dir() / f"{self.name}-{method}"

    def tmva_dataset_local(self, method: str, run_id: int):
        return self.reconstruction_dir() / f"run_{run_id:>02}-{method}"


class SimulationGroup:
    def __init__(self, name: str, simulations: list[Simulation]) -> None:
        if len(simulations) > 1:
            for idx in range(1, len(simulations)):
                if simulations[idx].study != simulations[idx - 1].study:
                    raise Exception(
                        "Attempted to group simulations from different studies"
                    )

        self.name = name
        self.simulations = simulations

    def __iter__(self):
        return iter(self.simulations)

    def __len__(self):
        return len(self.simulations)

    def __getitem__(self, idx):
        return self.simulations[idx]

    def append(self, simulation: Simulation):
        if len(self.simulations) == 0:
            self.simulations = [simulation]
            return

        if simulation.study != self.simulations[-1].study:
            raise Exception("Attempted to group simulations from different studies")

        self.simulations.append(simulation)

    def analysis_output(
        self,
        analysis_name: str,
        extension: str = "csv",
    ) -> Path:
        if self.single_simulation():
            return self.simulations[0].analysis_output(analysis_name, extension)
        else:
            return (
                self.study().simulation_dir()
                / f"{self.name}-{analysis_name}.{extension}"
            )

    def study(self) -> "Study":
        return self.simulations[0].study

    def single_simulation(self) -> bool:
        return len(self.simulations) == 1

    def run_dirs(self, delphes_tag_id: int = -1) -> list[Path]:
        run_dirs = [r for s in self.simulations for r in s.run_dirs(delphes_tag_id)]
        run_dirs.sort()

        return run_dirs

    def reconstruction_outputs(self) -> list[Path]:
        return [r for s in self.simulations for _, r in s.reconstruction_outputs()]

    def tmva_dataset_global(self, method: str):
        return self.study().reconstruction_dir() / f"{self.name}-{method}-Global"


def merge_simulation_groups(name: str, simulation_groups: list[SimulationGroup]):
    return SimulationGroup(
        name,
        [
            simulation
            for simulation_group in simulation_groups
            for simulation in simulation_group.simulations
        ],
    )
