# Copyright 2023-2026 Bruno M. S. Oliveira

import numpy as np
import pandas as pd

import madworkflow

################################################################################
# Configuration                                                                #
################################################################################
study = madworkflow.fetch_study("Trimuon@FCChh")

signal_simulation_name = "SPSS_251209a-qq+gq+aq"

n_bkgrnd_events = 900000

todo: dict[str, bool] = {
    "Kinematics": False,
    "Cuts-Global": False,
    "Cuts-Semilocal": False,
    "Cuts-Local": False,
    "BDT-Global": False,
    "BDT-Semilocal": True,
    "BDT-Local": False,
}

# ----- Signal -----
signal = study.simulations(signal_simulation_name, existing=True)

# ----- Background -----
bkgrnd_4l = study.simulations("bg-4l-ptmax")
bkgrnd_3lv = study.simulations("bg-3lv-ptmax")

bkgrnd_wwll = study.simulations("bg-wwll")
bkgrnd_3w = study.simulations("bg-3w")

bkgrnd_ttll = study.simulations("bg-ttll")
bkgrnd_ttlv = study.simulations("bg-ttlv")
bkgrnd_tqll = study.simulations("bg-tqll-ptmax")

bkgrnd_list = [
    bkgrnd_4l,
    bkgrnd_3lv,
    bkgrnd_wwll,
    bkgrnd_3w,
    bkgrnd_ttll,
    bkgrnd_ttlv,
    bkgrnd_tqll,
]

if __name__ == "__main__":
    ################################################################################
    # Setup                                                                        #
    ################################################################################
    madworkflow.analysis.compile()

    ################################################################################
    # Summary                                                                      #
    ################################################################################
    # csv_path = madworkflow.analysis.summary(signal_simulations, tag_id=1)
    # madworkflow.plot.summary(csv_path)

    ################################################################################
    # Detector Reconstruction                                                      #
    ################################################################################
    print(f"Reconstructing {signal_simulation_name}")
    csv_path = madworkflow.analysis.detector(signal, tag_id=1)
    madworkflow.plot.detector(csv_path, study.scaling_factor_xsect)

    for simulation in bkgrnd_list:
        print(f"Reconstructing {simulation.name} ({len(simulation)})")
        madworkflow.analysis.detector(simulation, tag_id=1)

    ################################################################################
    # Signal/Background Analysis                                                   #
    ################################################################################
    madworkflow.signal_background(todo, signal, bkgrnd_list)

    for analysis, has_run in todo.items():
        if not has_run:
            continue

        df = pd.read_csv(signal.analysis_output(analysis, "csv"))

        df["zExcludeSignalTrunc"] = np.where(
            df["thetaSquared"] < 10,
            df["zExcludeSignal"],
            np.maximum(df["zExcludeSignal"], 2),
        )

        df["zExcludeSuppressionTrunc"] = np.where(
            df["thetaSquared"] < 10,
            df["zExcludeSuppression"],
            np.maximum(df["zExcludeSuppression"], 2),
        )

        df.to_csv(signal.analysis_output(analysis, "csv"), index=False)
