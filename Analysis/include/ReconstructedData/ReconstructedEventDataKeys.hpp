// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

namespace HNL::Reconstruct {
/** @brief ROOT tree name storing reconstructed event entries. */
inline constexpr const char* const eventTree{"Data"};

/** @brief ROOT object name storing the serialized model identifier. */
inline constexpr const char* const modelID{"model"};
/** @brief ROOT object name storing the serialized model parameters. */
inline constexpr const char* const modelParameters{"parameters"};

/** @brief ROOT object name storing the total generated cross section. */
inline constexpr const char* const totalCrossSection{"totalCrossSection"};
/** @brief ROOT object name storing the effective cross section after filtering. */
inline constexpr const char* const effectiveCrossSection{"effectiveCrossSection"};
}  // namespace HNL::Reconstruct
