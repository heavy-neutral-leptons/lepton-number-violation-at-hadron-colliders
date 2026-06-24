# Copyright 2023-2026 Bruno M. S. Oliveira

import madworkflow

if __name__ == "__main__":
    ################################################################################
    # Setup                                                                        #
    ################################################################################
    madworkflow.analysis.compile()

    simulation_names = [
        "SPSS_251209a-nc-qq",
        "SPSS_251209a-nc-gq",
        "SPSS_251209a-nc-aq",
        "SPSS_260121a-nc-gg",
        "SPSS_251209a-cc-qq",
        "SPSS_251209a-cc-gq",
        "SPSS_251209a-cc-aq",
    ]

    study = madworkflow.fetch_study("Production@FCChh")
    for simulation_name in simulation_names:
        simulation_list = [
            simulation
            for simulation in [
                *study.simulations(simulation_name),
                *study.simulations(f"{simulation_name}-ew"),
                *study.simulations(f"{simulation_name}-higgs"),
            ]
        ]

        simulations = madworkflow.SimulationGroup(simulation_name, simulation_list)
        csv_path = madworkflow.analysis.summary(simulations, tag_id=0)
        madworkflow.plot.summary(csv_path)
