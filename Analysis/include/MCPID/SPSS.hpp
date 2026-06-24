// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

namespace HNL::PID::SPSS {
inline constexpr int n1{8000011};  ///< First SPSS HNL PDG id.
inline constexpr int n2{8000012};  ///< Second SPSS HNL PDG id.

inline constexpr int nn[]{n1, n2};  ///< Supported SPSS HNL PDG ids.
/** @brief Return whether a PDG id matches one of the SPSS HNLs. */
constexpr bool IsHNL(const int pid) { return pid == n1 || pid == n2; }
}  // namespace HNL::PID::SPSS
