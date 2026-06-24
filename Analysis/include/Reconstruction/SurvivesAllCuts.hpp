// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

// Standard Library
#include <concepts>

namespace HNL::Reconstruct {
/**
 * @brief Determines whether a given event survives all cuts.
 *
 * In this context, cuts are represented as enum values, where each value encodes a specific
 * selection state. A value of 0 indicates that the event has passed all applied cuts. This function
 * checks whether the provided integral value corresponds to that state.
 *
 * @tparam T An integral type, typically an enum.
 * @param cut The cut value to evaluate.
 * @return true if the event survived all cuts, false otherwise.
 */
template <std::integral T>
constexpr bool SurvivesAllCuts(const T cut) {
  return cut == 0;
}
}  // namespace HNL::Reconstruct
