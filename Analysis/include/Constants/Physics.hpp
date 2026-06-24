// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

namespace HNL::Constants {
///@name General physics constants
///@{
inline constexpr double speedOfLight{299792458000};  ///< Speed of light in the vacuum (mm/s)

inline constexpr double hbar_GeVs{6.58211915e-25};    ///< Planck's constant (GeV.s)
inline constexpr double hbar_ueVs{hbar_GeVs * 1e15};  ///< Planck's constant (µeV.s)
///@}

///@name Standard Model physics constants
///@{
/** Squared sine of the weak mixing angle */
inline constexpr double weakAngleSquaredSin{0.23121};
/** Squared cosine of the weak mixing angle */
inline constexpr double weakAngleSquaredCos{1 - weakAngleSquaredSin};

/** Z boson coupling to left-chiral fermions */
inline constexpr double gZL{1 - (2 * weakAngleSquaredSin)};
/** Z boson coupling to right-chiral fermions */
inline constexpr double gZR{2 * weakAngleSquaredSin};

/** Normalized Z boson coupling to left-chiral fermions */
inline constexpr double gZLNorm{gZL * gZL / ((gZL * gZL) + (gZL * gZL))};
/** Normalized Z boson coupling to right-chiral fermions */
inline constexpr double gZRNorm{gZL * gZL / ((gZL * gZL) + (gZL * gZL))};
///@}
}  // namespace HNL::Constants
