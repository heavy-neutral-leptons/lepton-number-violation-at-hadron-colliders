# Copyright 2023-2026 Bruno M. S. Oliveira

import numpy as np

import madworkflow

if __name__ == "__main__":
    ################################################################################
    # Setup                                                                        #
    ################################################################################
    madworkflow.analysis.compile()

    ################################################################################
    # Trimuon@FCChh                                                               #
    ################################################################################
    study = madworkflow.fetch_study("Trimuon@FCChh")
    signal_simulation_name = "SPSS_251209a-dy+vbf"
    n_bkgrnd_events = 900000

    simulations = study.simulations(signal_simulation_name)

    for min_delta_r in np.linspace(0, 1, 11):
        print(f"Reconstructing {study.name()}/{signal_simulation_name} ({min_delta_r})")

        csv_path = madworkflow.analysis.generator(
            simulations, 0, min_delta_r=min_delta_r, n_bkgrnd_events=n_bkgrnd_events
        )
        madworkflow.plot.detector(csv_path)

    ################################################################################
    # DimuonSS@FCChh                                                              #
    ################################################################################
    study = madworkflow.fetch_study("DimuonSS@FCChh")
    signal_simulation_name = "SPSS_251209a-dy+vbf"
    n_bkgrnd_events = 900000

    simulations = study.simulations(signal_simulation_name)

    for min_delta_r in np.linspace(0, 1, 11):
        print(f"Reconstructing {study.name()}/{signal_simulation_name} ({min_delta_r})")

        csv_path = madworkflow.analysis.generator(
            simulations, 0, min_delta_r=min_delta_r, n_bkgrnd_events=n_bkgrnd_events
        )
        madworkflow.plot.detector(csv_path)
