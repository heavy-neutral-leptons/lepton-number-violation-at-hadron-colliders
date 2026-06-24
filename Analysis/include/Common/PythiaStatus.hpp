// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

namespace HNL::Pythia {
/**
 * @brief Checks whether a Pythia status code corresponds to a hard incoming particle.
 *
 * The accepted code depends on whether the event record is inspected before or after Pythia's
 * processing.
 *
 * @param status Pythia status code.
 * @param afterPythia If true, interpret `status` after Pythia processing; otherwise use the
 * generator-level convention.
 *
 * @return true if the particle is marked as hard incoming.
 */
inline bool IsHardIncoming(const int status, const bool afterPythia) {
  if (!afterPythia) return status == -1;
  return status == 21;
}

/**
 * @brief Checks whether a Pythia status code corresponds to a hard intermediate particle.
 *
 * @param status Pythia status code.
 * @param afterPythia If true, interpret `status` after Pythia processing; otherwise use the
 * generator-level convention.
 *
 * @return true if the particle is marked as hard intermediate.
 */
inline bool IsHardIntermediate(const int status, const bool afterPythia) {
  if (!afterPythia) return status == 2;
  return status == 22;
}

/**
 * @brief Checks whether a Pythia status code corresponds to a hard outgoing particle.
 *
 * @param status Pythia status code.
 * @param afterPythia If true, interpret `status` after Pythia processing; otherwise use the
 * generator-level convention.
 *
 * @return true if the particle is marked as hard outgoing.
 */
inline bool IsHardOutgoing(const int status, const bool afterPythia) {
  if (!afterPythia) return status == 1;
  return status == 23 || status == 24;
}
}  // namespace HNL::Pythia
