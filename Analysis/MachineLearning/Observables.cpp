// Copyright 2023-2026 Bruno M. S. Oliveira

// HNL
#include "MachineLearning/Observables.hpp"

#include "Common/Log.hpp"

namespace HNL::ML {
const std::vector<Common::Observable>* Observables(const std::filesystem::path& path) {
  if (path.string().contains("DimuonSS")) return &DimuonSS::observables;
  if (path.string().contains("Trimuon")) return &Trimuon::observables;

  LogError("Unexpected  path '{}'", path.string());
  exit(1);
}

}  // namespace HNL::ML
