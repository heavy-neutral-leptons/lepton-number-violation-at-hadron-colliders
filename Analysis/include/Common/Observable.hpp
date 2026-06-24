// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

namespace HNL::Common {
/**
 * @brief Describes a named physics observable for plotting and reporting.
 *
 * A physics observable described by its name, title, and unit.
 */
struct Observable {
  const char* name;   ///< Internal identifier used in code and storage.
  const char* title;  ///< Human-readable title used in labels and plots.
  const char* unit;   ///< Display unit associated with the observable.
};
}  // namespace HNL::Common
