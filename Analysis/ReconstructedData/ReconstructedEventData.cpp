// Copyright 2023-2026 Bruno M. S. Oliveira

// ROOT
#include "TParameter.h"

// HNL
#include "Common/Log.hpp"
#include "Models/HeavyN_Dirac.hpp"
#include "Models/HeavyN_Majorana.hpp"
#include "Models/SM.hpp"
#include "Models/SPSS.hpp"
#include "Models/SeesawICurrents.hpp"
#include "Models/pSPSS.hpp"
#include "ReconstructedData/ReconstructedEventData.hpp"
#include "ReconstructedData/ReconstructedEventDataKeys.hpp"

namespace HNL::Reconstruct {
std::string SimulationName(const std::filesystem::path& path) {
  return path.parent_path().filename() / path.filename().replace_extension();
}

ReconstructedEventFile::ReconstructedEventFile(const std::filesystem::path& path)
    : path{path}, file{TFile::Open(path.c_str())}, tree{file->Get<TTree>(eventTree)} {
  if (file->IsZombie() || !file->IsOpen()) {
    LogError("Failed to open {}.", path.c_str());
    exit(1);
  }

  totalCrossSection = file->Get<TParameter<double>>(Reconstruct::totalCrossSection)->GetVal();
  effectiveCrossSection =
      file->Get<TParameter<double>>(Reconstruct::effectiveCrossSection)->GetVal();
}

ReconstructedEventFile::~ReconstructedEventFile() { file->Close(); }

[[nodiscard]] std::string ReconstructedEventFile::Name() const { return SimulationName(path); };

std::vector<double>* ReconstructedEventFile::Parameters() const {
  return file->Get<std::vector<double>>(Reconstruct::modelParameters);
}

Model::ModelID ReconstructedEventFile::ModelID() const {
  return Model::ModelID{file->Get<TParameter<int>>(Reconstruct::modelID)->GetVal()};
}

std::unique_ptr<Model::Generic> ReconstructedEventFile::Model() const {
  const Model::ModelID modelID{ModelID()};

  switch (modelID) {
    case Model::ModelID::SM: {
      return std::make_unique<Model::SM>();
    }

    case Model::ModelID::HeavyN_Majorana: {
      return std::make_unique<Model::HeavyN::Majorana>(*Parameters());
    }

    case Model::ModelID::HeavyN_Dirac: {
      return std::make_unique<Model::HeavyN::Dirac>(*Parameters());
    }

    case Model::ModelID::pSPSS: {
      return std::make_unique<Model::pSPSS>(*Parameters());
    }

    case Model::ModelID::SeesawICurrents: {
      return std::make_unique<Model::SeesawICurrents>(*Parameters());
    }

    case Model::ModelID::SPSS: {
      return std::make_unique<Model::SPSS>(*Parameters());
    }
  }

  return nullptr;
}

ReconstructedEventChain::ReconstructedEventChain(const std::vector<std::filesystem::path>& paths)
    : paths{paths}, chain{eventTree} {
  for (const std::filesystem::path& path : paths) {
    TFile* file{TFile::Open(path.c_str())};
    totalCrossSection += file->Get<TParameter<double>>(Reconstruct::totalCrossSection)->GetVal();
    effectiveCrossSection +=
        file->Get<TParameter<double>>(Reconstruct::effectiveCrossSection)->GetVal();
    file->Close();

    chain.Add(path.c_str());
  }

  totalCrossSection /= static_cast<double>(paths.size());
  effectiveCrossSection /= static_cast<double>(paths.size());
}

[[nodiscard]] std::string ReconstructedEventChain::Name() const {
  if (paths.size() == 1) return SimulationName(paths.front());

  std::string name{SimulationName(paths.front())};
  if (name.size() > 3) name.resize(name.size() - 3);
  return name;
}

[[nodiscard]] int64_t ReconstructedEventChain::GetEntries() const { return chain.GetEntries(); }
}  // namespace HNL::Reconstruct
