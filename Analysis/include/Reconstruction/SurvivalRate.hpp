// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

// Standard Library
#include <concepts>

// HNL
#include "Common/Log.hpp"

namespace HNL::Reconstruct {
///@name Survival-rate utilities
///@{
/**
 * @brief Computes the fraction of events surviving a cut sequence.
 *
 * This overload is used when both counts share the same integral type. If `afterCut > beforeCut`,
 * it logs an error and terminates the process.
 *
 * @tparam T Integral count type.
 * @param afterCut Number of events after the cut.
 * @param beforeCut Number of events before the cut.
 *
 * @return Survival fraction `afterCut / beforeCut`.
 */
template <std::integral T>
inline double SurvivalRate(const T afterCut, const T beforeCut) {
  if (afterCut > beforeCut) {
    LogError("Invalid cut efficiency {} -> {}", beforeCut, afterCut);
    exit(1);
  }

  return static_cast<double>(afterCut) / static_cast<double>(beforeCut);
}

/**
 * @brief Computes the fraction of events surviving a cut sequence for mixed integral types.
 *
 * If `afterCut > beforeCut`, it logs an error and terminates the process.
 *
 * @tparam T1 Integral type of `afterCut`.
 * @tparam T2 Integral type of `beforeCut`.
 * @param afterCut Number of events after the cut.
 * @param beforeCut Number of events before the cut.
 *
 * @return Survival fraction `afterCut / beforeCut`.
 */
template <std::integral T1, std::integral T2>
inline double SurvivalRate(const T1 afterCut, const T2 beforeCut) {
  const double afterCutD{static_cast<double>(afterCut)};
  const double beforeCutD{static_cast<double>(beforeCut)};

  if (afterCutD > beforeCutD) {
    LogError("Invalid cut efficiency {} -> {}", beforeCut, afterCut);
    exit(1);
  }

  return afterCutD / beforeCutD;
}

/**
 * @brief Numeric type accepted by the generic survival-rate overload.
 *
 * Satisfied by integral and floating-point types.
 *
 * @tparam T Candidate type.
 */
template <typename T>
concept NumericType = std::integral<T> || std::floating_point<T>;

/**
 * @brief Computes the fraction of events surviving a cut sequence for generic numeric inputs.
 *
 * If `afterCut > beforeCut`, it logs an error and terminates the process.
 *
 * @tparam T1 Numeric type of `afterCut`.
 * @tparam T2 Numeric type of `beforeCut`.
 * @param afterCut Number of events after the cut.
 * @param beforeCut Number of events before the cut.
 *
 * @return Survival fraction `afterCut / beforeCut`.
 */
template <NumericType T1, NumericType T2>
inline double SurvivalRate(const T1 afterCut, const T2 beforeCut) {
  const double afterCutD{static_cast<double>(afterCut)};
  const double beforeCutD{static_cast<double>(beforeCut)};

  if (afterCutD > beforeCutD) {
    LogError("Invalid cut efficiency {} -> {}", beforeCut, afterCut);
    exit(1);
  }

  return afterCutD / beforeCutD;
}
///@}
}  // namespace HNL::Reconstruct
