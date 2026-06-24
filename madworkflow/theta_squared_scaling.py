# Copyright 2023-2026 Bruno M. S. Oliveira

from collections.abc import Callable
from pathlib import Path

import pandas as pd

from .common import sv


def scaling_factor_width(theta_squared_0: float, theta_squared: float) -> float:
    if theta_squared == theta_squared_0:
        return 1

    theta_squared_ratio = theta_squared / theta_squared_0
    sv_ratio = sv(theta_squared) / sv(theta_squared_0)
    return theta_squared_ratio / (sv_ratio**2.5)


def theta_squared_scaling_df(
    df_orig: pd.DataFrame,
    theta_squared_0: float,
    theta_squared_list: list[float],
    scaling_factor_xsect: Callable[[float, float], float],
    scaling_factor_width: Callable[[float, float], float] = scaling_factor_width,
) -> pd.DataFrame:
    df_filt = df_orig.loc[df_orig.thetaSquared == theta_squared_0]
    df_scld = df_filt.copy()

    if theta_squared_0 in theta_squared_list:
        theta_squared_list.remove(theta_squared_0)

    for theta_squared in theta_squared_list:
        df_fake = df_filt.assign(thetaSquared=theta_squared)

        width_factor = scaling_factor_width(theta_squared_0, theta_squared)
        if "meanWidth" in df_fake:
            df_fake["meanWidth"] = width_factor * df_filt["meanWidth"]

        xsect_factor = scaling_factor_xsect(theta_squared_0, theta_squared)
        for col in [
            "crossSection",
            "effectiveCrossSection",
            "expectedEvents",
            "nExpected",
        ]:
            if col in df_fake:
                df_fake[col] = xsect_factor * df_filt[col]

        df_scld = pd.concat([df_scld, df_fake], ignore_index=True)

    df_scld = df_scld.sort_values(
        by=["meanMass", "thetaSquared"], ascending=[False, True]
    )

    return df_scld


def theta_squared_scaling(
    in_path: Path,
    scaling_factor_xsect: Callable[[float, float], float],
    scaling_factor_width: Callable[[float, float], float] = scaling_factor_width,
) -> Path:
    df_orig = pd.read_csv(in_path)
    df_scld = theta_squared_scaling_df(
        df_orig,
        theta_squared_0=df_orig.thetaSquared.min(),
        theta_squared_list=list(df_orig["thetaSquared"].unique()),
        scaling_factor_width=scaling_factor_width,
        scaling_factor_xsect=scaling_factor_xsect,
    )

    out_path = in_path.with_suffix(".scaled.csv")
    df_scld.to_csv(out_path, index=False, na_rep="nan")

    if in_path.with_suffix(".png").exists():
        out_path.with_suffix(".png").touch()

    return out_path
