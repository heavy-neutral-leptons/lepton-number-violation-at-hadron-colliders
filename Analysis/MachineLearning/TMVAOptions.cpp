// Copyright 2023-2026 Bruno M. S. Oliveira

// Standard Library
#include <string>

// HNL
#include "Common/Log.hpp"
#include "MachineLearning/TMVAOptions.hpp"

namespace HNL::ML::Options {
std::string Method(const MVAMethod method) {
  switch (method) {
    case MVAMethod::Cuts:
      return Options::cuts;
    case MVAMethod::BDT:
      return Options::bdt;
  }

  LogError("Invalid method.");
  exit(1);
}
}  // namespace HNL::ML::Options
