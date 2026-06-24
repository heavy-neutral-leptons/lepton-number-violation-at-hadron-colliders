# Copyright 2023-2026 Bruno M. S. Oliveira


class Model:
    def __init__(self, name: str, columns: list[str]):
        self.name = name
        self.columns = columns


columns_heavyn: list[str] = [
    "mn1",
    "ven1",
    "vmun1",
    "vtan1",
    "wn1",
    "mn2",
    "ven2",
    "vmun2",
    "vtan2",
    "wn2",
    "mn3",
    "ven3",
    "vmun3",
    "vtan3",
    "wn3",
]
heavyn_majorana = Model("HeavyN", columns_heavyn)
heavyn_dirac = Model("HeavyN_Dirac", columns_heavyn)

columns_seesawi_currents: list[str] = [
    "mn1",
    "ve1",
    "vm1",
    "vt1",
    "wn1",
    "mn2",
    "ve2",
    "vm2",
    "vt2",
    "wn2",
    "mn3",
    "ve3",
    "vm3",
    "vt3",
    "wn3",
]
seesawi_currents = Model("SeesawICurrents", columns_seesawi_currents)


columns_pspss: list[str] = [
    "meanMass",
    "thetaSquared",
    "meanWidth",
    "deltam",
    "damping",
    "lnv2lncRatio",
    "inverseSeesawMass",
]
pspss = Model("pSPSS", columns_pspss)
spss = Model("SPSS", columns_pspss)


def model(string: str) -> Model:
    if string == "HeavyN" or string == "HeavyN_Majorana":
        return heavyn_majorana

    if string == "HeavyN_Dirac":
        return heavyn_dirac

    elif string == "seesawICurrents":
        return seesawi_currents

    elif string == "pSPSS":
        return pspss

    elif string == "SPSS":
        return spss

    else:
        raise Exception(f"No valid model was found in '{string}'")
