# Copyright 2023-2026 Bruno M. S. Oliveira

from pathlib import Path

import pandas as pd

from ..common import analysis_bin
from ..runners import run_cmd_table


def kinematics(
    signal_reconstructed: list[Path],
    bkgrnd_reconstructed: list[Path],
    csv_path: Path,
):
    cmd: list[str] = [
        str(analysis_bin("Kinematics")),
        "--signal",
        str(signal_reconstructed),
        "--background",
        str(bkgrnd_reconstructed),
    ]

    result = run_cmd_table(cmd)

    df = pd.DataFrame(result)
    df.columns = df.iloc[0]
    df = df[1:]

    print(df)

    print(f"Writing result to {csv_path}")
    df.to_csv(csv_path)
