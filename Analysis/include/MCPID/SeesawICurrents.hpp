// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

namespace HNL::PID::SeesawICurrents {
inline constexpr int n1{8000012};  ///< First-generation Seesaw-I current-eigenstate HNL PDG id.
inline constexpr int n2{8000014};  ///< Second-generation Seesaw-I current-eigenstate HNL PDG id.
inline constexpr int n3{8000016};  ///< Third-generation Seesaw-I current-eigenstate HNL PDG id.

inline constexpr int nn[]{n1, n2, n3};  ///< Supported Seesaw-I current-eigenstate HNL PDG ids.
/** @brief Return whether a PDG id matches one of the Seesaw-I current-eigenstate HNLs. */
constexpr bool IsHNL(const int pid) { return pid == n1 || pid == n2 || pid == n3; }
}  // namespace HNL::PID::SeesawICurrents
