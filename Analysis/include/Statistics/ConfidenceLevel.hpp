// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

namespace HNL::Statistics {
/**
 * Confidence level of a normally distributed variable at Z=1σ.
 *
 * Source: https://en.wikipedia.org/wiki/68%E2%80%9395%E2%80%9399.7_rule
 * */
inline constexpr double sd1ConfidenceLevel{0.6827};
/**
 * Confidence level of a normally distributed variable at Z=2σ.
 *
 * Source: https://en.wikipedia.org/wiki/68%E2%80%9395%E2%80%9399.7_rule
 * */
inline constexpr double sd2ConfidenceLevel{0.9545};
/**
 * Confidence level of a normally distributed variable at Z=3σ.
 *
 * Source: https://en.wikipedia.org/wiki/68%E2%80%9395%E2%80%9399.7_rule
 * */
inline constexpr double sd3ConfidenceLevel{0.9973};
}  // namespace HNL::Statistics
