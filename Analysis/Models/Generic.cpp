// Copyright 2023-2026 Bruno M. S. Oliveira

// ROOT
#include "ReconstructedData/ReconstructedEventDataKeys.hpp"
#include "TParameter.h"

// HNL
#include "Common/Log.hpp"
#include "MCPID/HeavyN.hpp"
#include "MCPID/SPSS.hpp"
#include "MCPID/SeesawICurrents.hpp"
#include "MCPID/pSPSS.hpp"
#include "Models/Generic.hpp"

namespace HNL::Model {
std::string ToString(ModelID model) {
  switch (model) {
    case ModelID::SM:
      return "Standard Model";
    case ModelID::HeavyN_Majorana:
      return "HeavyN (Majorana)";
    case ModelID::HeavyN_Dirac:
      return "HeavyN (Dirac)";
    case ModelID::SeesawICurrents:
      return "SeesawICurrents";
    case ModelID::pSPSS:
      return "pSPSS";
    case ModelID::SPSS:
      return "SPSS";
  }

  LogError("Invalid model.");
  exit(1);
}

void Generic::WriteToFile(TFile &file) const {
  TParameter<int> modelParameter{Reconstruct::modelID, std::to_underlying(id)};
  file.WriteObject(&modelParameter, Reconstruct::modelID);

  const std::vector parametersVector{Vector()};
  if (!parametersVector.empty()) {
    file.WriteObject(&parametersVector, Reconstruct::modelParameters);
  }
}
}  // namespace HNL::Model

bool HNL::PID::IsHNL(const Model::ModelID modelID, const int pid) {
  switch (modelID) {
    case Model::ModelID::HeavyN_Majorana:
      return HeavyN::Majorana::IsHNL(pid);
    case Model::ModelID::HeavyN_Dirac:
      return HeavyN::Dirac::IsHNL(pid);
    case Model::ModelID::pSPSS:
      return pSPSS::IsHNL(pid);
    case Model::ModelID::SeesawICurrents:
      return SeesawICurrents::IsHNL(pid);
    case Model::ModelID::SPSS:
      return SPSS::IsHNL(pid);
    default:
      LogError("Model '{}' has no HNLs", Model::ToString(modelID));
      exit(1);
  }
}
