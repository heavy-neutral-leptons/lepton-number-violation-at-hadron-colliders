# Copyright 2023-2026 Bruno M. S. Oliveira

import argparse

import madworkflow

if __name__ == "__main__":
    ################################################################################
    # Setup                                                                        #
    ################################################################################
    madworkflow.analysis.compile()

    ########################################################################################################################
    # Command Line Arguments                                                                                               #
    ########################################################################################################################
    parser = argparse.ArgumentParser()

    parser.add_argument("--study", type=str, required=True)
    parser.add_argument("--name", type=str, required=True)

    parser.add_argument("--tag", nargs="?", type=int, default=1)

    args = parser.parse_args()

    # ----- Derived Variables -----
    study = madworkflow.fetch_study(args.study)
    simulations = study.simulations(args.name)

    # ----- Analysis and Plot -----
    csv_path = madworkflow.analysis.summary(simulations, args.tag)

    if "bg" not in args.name:
        madworkflow.plot.summary(csv_path)
