// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

namespace HNL::PID::HeavyN {
namespace Majorana {
inline constexpr int n1{9900012};  ///< First-generation Majorana HNL PDG id.
inline constexpr int n2{9900014};  ///< Second-generation Majorana HNL PDG id.
inline constexpr int n3{9900016};  ///< Third-generation Majorana HNL PDG id.

inline constexpr int nn[]{n1, n2, n3};  ///< Supported Majorana HNL PDG ids.
/** @brief Return whether a PDG id matches one of the supported Majorana HNLs. */
constexpr bool IsHNL(const int pid) { return pid == n1 || pid == n2 || pid == n3; }
}  // namespace Majorana

namespace Dirac {
inline constexpr int n1{9990012};  ///< First-generation Dirac HNL PDG id.
inline constexpr int n2{9990014};  ///< Second-generation Dirac HNL PDG id.
inline constexpr int n3{9990016};  ///< Third-generation Dirac HNL PDG id.

inline constexpr int nn[]{n1, n2, n3};  ///< Supported Dirac HNL PDG ids.
/** @brief Return whether a PDG id matches one of the supported Dirac HNLs. */
constexpr bool IsHNL(int pid) { return pid == n1 || pid == n2 || pid == n3; }
}  // namespace Dirac
}  // namespace HNL::PID::HeavyN
