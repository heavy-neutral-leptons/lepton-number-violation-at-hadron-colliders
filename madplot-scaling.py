# Copyright 2023-2026 Bruno M. S. Oliveira

import madworkflow

simulation_name = "SPSS_251209a-qq+gq+aq-os2"

if __name__ == "__main__":
    ################################################################################
    # Setup                                                                        #
    ################################################################################
    madworkflow.analysis.compile()

    ################################################################################
    # Trimuon                                                                     #
    ################################################################################
    study = madworkflow.fetch_study("Trimuon@FCChh")
    simulations = study.simulations(simulation_name)

    csv_path = madworkflow.analysis.summary(simulations, tag_id=0)
    madworkflow.plot.summary(csv_path, study.scaling_factor_xsect)

    madworkflow.plot.summary_scaling(simulations)

    ################################################################################
    # DimuonSS                                                                    #
    ################################################################################
    study = madworkflow.fetch_study("DimuonSS@FCChh")
    simulations = study.simulations(simulation_name)

    csv_path = madworkflow.analysis.summary(simulations, tag_id=0)
    madworkflow.plot.summary(csv_path, study.scaling_factor_xsect)

    madworkflow.plot.summary_scaling(simulations)
