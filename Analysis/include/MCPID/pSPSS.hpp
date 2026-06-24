// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

namespace HNL::PID::pSPSS {
inline constexpr int n4{8000011};  ///< First pseudo-Dirac SPSS HNL PDG id.
inline constexpr int n5{8000012};  ///< Second pseudo-Dirac SPSS HNL PDG id.

inline constexpr int nn[]{n4, n5};  ///< Supported pseudo-Dirac SPSS HNL PDG ids.
/** @brief Return whether a PDG id matches one of the pseudo-Dirac SPSS HNLs. */
constexpr bool IsHNL(const int pid) { return pid == n4 || pid == n5; }
}  // namespace HNL::PID::pSPSS
