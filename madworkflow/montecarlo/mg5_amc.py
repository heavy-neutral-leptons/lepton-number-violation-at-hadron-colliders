# Copyright 2023-2026 Bruno M. S. Oliveira

import subprocess
import sys

import numpy as np

from ..common import mg5_amc
from ..simulation import Simulation


def scan_array(argmin: float, argmax: float, n: int):
    return np.logspace(start=np.log10(argmin), stop=np.log10(argmax), num=n)


def __gen_scan():
    id = 0

    def gen_scan(
        val: float | None, n: int, min: float, max: float, sort_decreasing: bool = False
    ) -> tuple[int, str]:
        nonlocal id

        if val is not None:
            return (1, "{}".format(val))

        if min == max:
            return (1, "{}".format(min))

        if n < 2:
            raise Exception(f"Attempting to generate scan with {n} point(s)")

        id += 1

        scan = scan_array(min, max, n).tolist()
        if sort_decreasing:
            scan.sort(reverse=True)
        else:
            scan.sort()

        scan_str = ",".join(str(entry) for entry in scan)

        return (n, f"scan{id}:[{scan_str}] # min={min}, max={max}, n={n}")

    return gen_scan


scan = __gen_scan()


def history(simulation: Simulation, replacements: dict[str, str]) -> None:
    history_template_path = simulation.study.history_template_path()
    if not history_template_path.is_file():
        raise FileNotFoundError(f"{history_template_path} does not exist")

    history_path = simulation.history_path()
    if history_path.is_file():
        raise FileExistsError(
            f"{history_path} will not be generated since it already exists"
        )

    simulation_dir = simulation.simulation_dir()
    replacements["dir"] = str(simulation.simulation_dir())
    if simulation_dir.is_dir():
        raise FileExistsError(
            f"{history_path} will not be generated since {simulation_dir} already exists"
        )

    print(f"Generating {history_path} ({history_template_path}) with:")
    for replacement in replacements.items():
        print(f" {replacement[0]}: {replacement[1]}")

    history_template: str = open(history_template_path).read()
    with open(history_path, "w") as history:
        history.write("# python " + " ".join(sys.argv) + "\n")
        history.write(history_template.format(**replacements))


def run_mg5_amc(simulation: Simulation) -> None:
    history_path = simulation.history_path()
    simulation_dir = simulation.simulation_dir()
    log_path = simulation.log_path()

    if simulation_dir.is_dir():
        print(
            f"{history_path} will not be simulated since {simulation_dir} already exists"
        )
        return

    print(f"Running {mg5_amc} {history_path} and piping output to {log_path}")
    with open(log_path, "w") as log:
        subprocess.run([mg5_amc, history_path], stdout=log, stderr=log, check=True)
