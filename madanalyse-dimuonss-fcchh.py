# Copyright 2023-2026 Bruno M. S. Oliveira

import numpy as np
import pandas as pd

import madworkflow

################################################################################
# Configuration                                                                #
################################################################################
study = madworkflow.fetch_study("DimuonSS@FCChh")

signal_simulation_name = "SPSS_251209a-qq+gq+aq"

n_bkgrnd_events = 2000000

todo: dict[str, bool] = {
    "Kinematics": False,
    "Cuts-Global": False,
    "Cuts-Semilocal": False,
    "Cuts-Local": False,
    "BDT-Global": False,
    "BDT-Semilocal": True,
    "BDT-Local": False,
}

# ----- Mass Splitting Scan -----
base_scan = np.linspace(1, 10, 10)
deltam = [
    str(x)
    for x in [
        0.0,
        *1e-10 * base_scan,
        *1e-9 * base_scan[1:],
        *1e-8 * base_scan[1:],
        *np.logspace(np.log10(1e-8), np.log10(6e-8), 30),
        study.experiment.mass_splitting_threshold,
    ]
]

base_scan = np.array([1, 2, 5, 10])
mv = [
    str(x)
    for x in [
        *1e-16 * base_scan,
        *1e-15 * base_scan[1:],
        *1e-14 * base_scan[1:],
        *1e-13 * base_scan[1:],
        *1e-12 * base_scan[1:],
        *1e-11 * base_scan[1:],
        *1e-10 * base_scan[1:],
        *1e-9 * base_scan[1:],
        *1e-8 * base_scan[1:],
    ]
]

# ----- Signal -----
signal = study.simulations(signal_simulation_name, existing=True)

# ----- Background -----
bkgrnd_4l = study.simulations("bg-4l-ptmax")
bkgrnd_3lv = study.simulations("bg-3lv-ptmax")

bkgrnd_wwll = study.simulations("bg-wwll-ptmax")
bkgrnd_3w = study.simulations("bg-3w")

bkgrnd_ttll = study.simulations("bg-ttll-jet1")
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
    madworkflow.signal_background(todo, signal, bkgrnd_list, deltam, mv)

    for analysis, has_run in todo.items():
        if not has_run:
            continue

        df = pd.read_csv(signal.analysis_output(analysis, "csv"))
        df_lnb = pd.read_csv(f"Trimuon@FCChh/{signal.name}-{analysis}.csv")

        if "zExcludeSignal_lnb" in df.columns:
            df = df.drop("zExcludeSignal_lnb", axis=1)

        df = df.merge(
            df_lnb[["meanMass", "thetaSquared", "zExcludeSignal"]].rename(
                {"zExcludeSignal": "zExcludeSignal_lnb"}, axis=1
            ),
            on=["meanMass", "thetaSquared"],
            how="left",
        )

        df["zExcludeSuppressionTrunc"] = df[
            [
                "zExcludeSignal",
                "zExcludeSuppression",
                "zExcludeSignal_lnb",
            ]
        ].min(axis=1)

        df["zExcludeSignalTrunc"] = np.where(
            df["thetaSquared"] < 10,
            df["zExcludeSignal"],
            np.maximum(df["zExcludeSignal"], 2),
        )

        df["zExcludeSuppressionTrunc"] = np.where(
            df["thetaSquared"] < 10,
            df["zExcludeSuppressionTrunc"],
            np.maximum(df["zExcludeSuppressionTrunc"], 2),
        )

        df.to_csv(signal.analysis_output(analysis, "csv"), index=False)
