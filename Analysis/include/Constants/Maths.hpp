// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

// Standard Library
#include <numbers>

namespace HNL::Constants {
///@name Mathematical constants
///@{
/** The ratio of a circle's area to its radius (π = 3.1415...) */
inline constexpr double pi{std::numbers::pi};
/** The ratio of a circle's perimeter to its radius (τ = 2π = 6.2931...) */
inline constexpr double tau{2 * pi};
///@}
}  // namespace HNL::Constants
