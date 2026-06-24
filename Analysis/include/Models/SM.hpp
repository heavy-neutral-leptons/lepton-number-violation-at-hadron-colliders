// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

// HNL
#include "Generic.hpp"

namespace HNL::Model {
/** @brief Trivial Standard Model placeholder used by model-aware utilities. */
struct SM final : Generic {
 public:
  /** @brief Construct the Standard Model placeholder. */
  SM();

  [[nodiscard]] std::unique_ptr<Generic> Clone() const final { return std::make_unique<SM>(*this); }

  [[nodiscard]] constexpr const char *CSVHeader() const final { return ""; }
  [[nodiscard]] std::string CSV() const final;

  [[nodiscard]] std::vector<double> Vector() const final;
};
}  // namespace HNL::Model
