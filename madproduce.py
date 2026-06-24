#!/usr/bin/env python3
# Copyright 2023-2026 Bruno M. S. Oliveira

import argparse
import multiprocessing

import madworkflow
import madworkflow.montecarlo as montecarlo


def make_history_files(
    study: madworkflow.Study,
    name: str,
    splitscan: bool,
    maxjetflavor: int,
    nevents: int,
    mn,
    mnn: int,
    mnmin: float,
    mnmax: float,
    v1e,
    v1en: int,
    v1emin: float,
    v1emax: float,
    v1m,
    v1mn: int,
    v1mmin: float,
    v1mmax: float,
    v1t,
    v1tn: int,
    v1tmin: float,
    v1tmax: float,
):
    # ----- Variables -----
    jet_flavors: str = ""
    if maxjetflavor < 2 or maxjetflavor > 5:
        raise ValueError("--maxjetflavor must be between 2 and 5")
    elif maxjetflavor == 2:
        jet_flavors = "d d~ u u~"
    elif maxjetflavor == 3:
        jet_flavors = "d d~ u u~ s s~"
    elif maxjetflavor == 4:
        jet_flavors = "d d~ u u~ s s~ c c~"
    elif maxjetflavor == 5:
        jet_flavors = "d d~ u u~ s s~ c c~ b b~"

    if nevents < 1:
        raise ValueError("--nevents must be a positive integer")

    [n_mn, mn] = montecarlo.scan(mn, mnn, mnmin, mnmax)
    [_, v1e] = montecarlo.scan(v1e, v1en, v1emin, v1emax, True)
    [_, v1m] = montecarlo.scan(v1m, v1mn, v1mmin, v1mmax, True)
    [_, v1t] = montecarlo.scan(v1t, v1tn, v1tmin, v1tmax, True)

    if v1e == "0":
        if v1m == "0":
            restriction: str = "-t"
        elif v1t == "0":
            restriction: str = "-m"
        else:
            restriction: str = "-mt"
    elif v1m == "0":
        if v1t == "0":
            restriction: str = "-e"
        else:
            restriction: str = "-et"
    elif v1m == "0":
        restriction: str = "-em"
    else:
        restriction: str = ""

    parameter_replacements: dict[str, str] = {
        "version": "_251209a",
        "restriction": restriction,
        "maxjetflavor": str(maxjetflavor),
        "jet_flavors": jet_flavors,
        "nevents": str(nevents),
        "mn": mn,
        "v1e": v1e,
        "v1m": v1m,
        "v1t": v1t,
    }

    simulations = madworkflow.SimulationGroup(name, [])

    # ----- History files -----
    if splitscan:
        if n_mn <= 1:
            raise ValueError("Only scans over mn may be split")

        mn_array = montecarlo.scan_array(mnmin, mnmax, mnn)

        for idx in range(mn_array.size):
            simulation_name: str = f"{name}-{idx + 1:>02d}"

            simulation = madworkflow.Simulation(study, simulation_name)
            simulations.append(simulation)

            replacements: dict[str, str] = {
                **parameter_replacements,
                "mn": mn_array[idx],
                "iseed": f"{idx + 1}",
            }

            montecarlo.history(simulation, replacements)

    else:
        simulation_name: str = name

        simulation = madworkflow.Simulation(study, simulation_name)
        simulations.append(simulation)

        replacements: dict[str, str] = {
            **parameter_replacements,
            "iseed": "0",
        }

        montecarlo.history(simulation, replacements)


if __name__ == "__main__":
    # ----- Command line arguments -----
    parser = argparse.ArgumentParser()

    parser.add_argument("--study", nargs="?", type=str, required=True)

    parser.add_argument("--name", nargs="?", type=str, required=True)

    parser.add_argument("--splitscan", action="store_true")

    parser.add_argument("--maxjetflavor", nargs="?", type=int, default=5)

    parser.add_argument("--nevents", type=int, default=10000)

    parser.add_argument("--mn", nargs="?", type=float)
    parser.add_argument("--mnn", nargs="?", type=int, default=1)
    parser.add_argument("--mnmin", nargs="?", type=float, default=1)
    parser.add_argument("--mnmax", nargs="?", type=float, default=1)

    parser.add_argument("--v1e", nargs="?", type=float)
    parser.add_argument("--v1en", nargs="?", type=int, default=1)
    parser.add_argument("--v1emin", nargs="?", type=float, default=0)
    parser.add_argument("--v1emax", nargs="?", type=float, default=0)

    parser.add_argument("--v1m", nargs="?", type=float)
    parser.add_argument("--v1mn", nargs="?", type=int, default=1)
    parser.add_argument("--v1mmin", nargs="?", type=float, default=1)
    parser.add_argument("--v1mmax", nargs="?", type=float, default=1)

    parser.add_argument("--v1t", nargs="?", type=float)
    parser.add_argument("--v1tn", nargs="?", type=int, default=1)
    parser.add_argument("--v1tmin", nargs="?", type=float, default=0)
    parser.add_argument("--v1tmax", nargs="?", type=float, default=0)

    parser.add_argument("--runparton", action="store_true")
    parser.add_argument("--runlhe2root", action="store_true")
    parser.add_argument("--rundetector", action="store_true")

    args = parser.parse_args()

    # ----- Identify Simulations -----
    study: madworkflow.Study = madworkflow.fetch_study(args.study)
    simulations: madworkflow.SimulationGroup = study.simulations(
        args.name, fail_smoothly=True
    )

    # ----- Main -----
    if len(simulations) != 0:
        print(f"History files for {study.name()}/{args.name} already exist")

    else:
        make_history_files(
            study,
            args.name,
            args.splitscan,
            args.maxjetflavor,
            args.nevents,
            args.mn,
            args.mnn,
            args.mnmin,
            args.mnmax,
            args.v1e,
            args.v1en,
            args.v1emin,
            args.v1emax,
            args.v1m,
            args.v1mn,
            args.v1mmin,
            args.v1mmax,
            args.v1t,
            args.v1tn,
            args.v1tmin,
            args.v1tmax,
        )

    if args.runparton:
        pool = multiprocessing.Pool(processes=24)
        pool.map(montecarlo.run_mg5_amc, simulations)

    if args.runlhe2root:
        montecarlo.run_lhe2root(simulations)

    if args.rundetector:
        montecarlo.run_delphes(simulations, study.delphes_card(), tag_id=1)
        montecarlo.compress_pythia_data(simulations, tag_id=1)

    print(f"Finishing processing {args.study}/{args.name}")
