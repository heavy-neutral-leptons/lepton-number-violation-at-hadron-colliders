# Copyright 2023-2026 Bruno M. S. Oliveira

from pathlib import Path

import madworkflow

study_path = Path("Trimuon@FCChh")
study = madworkflow.fetch_study(study_path.name)
# madworkflow.plot.summary(study_path / "SPSS_251209a-qq+gq+aq-Summary.csv")
# madworkflow.plot.detector(study_path / "SPSS_251209a-qq+gq+aq-Detector.csv")
madworkflow.plot.tmva.apply_all(study, "SPSS_251209a-qq+gq+aq", "BDT")
madworkflow.plot.tmva.separation_all(study, "SPSS_251209a-qq+gq+aq", "BDT")

study_path = Path("DimuonSS@FCChh")
study = madworkflow.fetch_study(study_path.name)
# madworkflow.plot.summary(study_path / "SPSS_251209a-qq+gq+aq-Summary.csv")
# madworkflow.plot.detector(study_path / "SPSS_251209a-qq+gq+aq-Detector.csv")
madworkflow.plot.tmva.apply_all(study, "SPSS_251209a-qq+gq+aq", "BDT")
madworkflow.plot.tmva.separation_all(study, "SPSS_251209a-qq+gq+aq", "BDT")
