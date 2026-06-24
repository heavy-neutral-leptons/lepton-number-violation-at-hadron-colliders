// Copyright 2023-2026 Bruno M. S. Oliveira

// Standard Library
#include <string>

// HNL
#include "Models/SM.hpp"

namespace HNL::Model {
SM::SM() : Generic{ModelID::SM} {};

std::string SM::CSV() const { return ""; }

std::vector<double> SM::Vector() const { return {}; }
}  // namespace HNL::Model
