// Copyright 2023-2026 Bruno M. S. Oliveira

// Standard Library
#include <string>

// HNL
#include "Common/Log.hpp"
#include "MachineLearning/Methods.hpp"

namespace HNL::ML {

std::string ToString(const MVAMethod method) {
  switch (method) {
    case MVAMethod::Cuts:
      return "Cuts";
    case MVAMethod::BDT:
      return "BDT";
  }

  LogError("Invalid method.");
  exit(1);
}

MVAMethod IdentifyMethod(const std::filesystem::path& datasetPath) {
  if (datasetPath.string().contains("Cuts")) return MVAMethod::Cuts;
  if (datasetPath.string().contains("BDT")) return MVAMethod::BDT;

  LogError("Invalid TMVA method in {}", datasetPath.c_str());
  exit(1);
}

TMVA::Types::EMVA EMVA(const MVAMethod method) {
  switch (method) {
    case MVAMethod::Cuts:
      return TMVA::Types::kCuts;
    case MVAMethod::BDT:
      return TMVA::Types::kBDT;
  }

  LogError("Invalid method.");
  exit(1);
}

std::filesystem::path WeightsPath(const std::filesystem::path& datasetPath, MVAMethod method) {
  return datasetPath / std::format("weights/HNL_{}.weights.xml", ToString(method));
}

double NaiveCut(const MVAMethod method) {
  switch (method) {
    case MVAMethod::Cuts:
      return 0.5;
    case MVAMethod::BDT:
      return 0;
  }

  LogError("Invalid method.");
  exit(1);
}
}  // namespace HNL::ML
