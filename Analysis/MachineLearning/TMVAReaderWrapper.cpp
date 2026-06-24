// Copyright 2023-2026 Bruno M. S. Oliveira

// Standard Library
#include <filesystem>
#include <ranges>
#include <string>
#include <vector>

// ROOT
#include "ROOT/RCsvDS.hxx"
#include "ROOT/RDataFrame.hxx"
#include "TDirectory.h"
#include "TFile.h"
#include "TMVA/DataLoader.h"
#include "TMVA/Reader.h"
#include "TTree.h"

// HNL
#include "Common/Log.hpp"
#include "MachineLearning/TMVAReaderWrapper.hpp"

namespace HNL::ML {
TMVAReaderWrapper::TMVAReaderWrapper(const std::filesystem::path& datasetPath,
                                     const std::vector<Common::Observable>* const observables)
    : fEvent(observables->size()),
      dEvent(observables->size()),
      method{IdentifyMethod(datasetPath)},
      methodTag{ToString(method)},
      observables{observables} {
  for (const auto [observable, container] : std::views::zip(*observables, fEvent)) {
    reader.AddVariable(observable.name, &container);
  }

  std::filesystem::path rootPath{datasetPath};
  rootPath.replace_extension("root");
  FetchMethodInfo(rootPath);

  const std::filesystem::path weightsPath{WeightsPath(datasetPath, method)};
  reader.BookMVA(methodTag.c_str(), weightsPath.c_str());

  switch (method) {
    case MVAMethod::Cuts:
      EvaluateMVACut = &TMVAReaderWrapper::EvaluateCutsMVACut;
      break;
    case MVAMethod::BDT:
      EvaluateMVACut = &TMVAReaderWrapper::EvaluateBDTMVACut;
      break;
  }
}

void TMVAReaderWrapper::FetchMethodInfo(const std::filesystem::path& rootPath) {
  TFile* file{TFile::Open(rootPath.c_str())};
  if (file->IsZombie() || !file->IsOpen()) {
    LogError("Failed to open {}.", rootPath.c_str());
    exit(1);
  }

  const TKey* const dsKey{dynamic_cast<TKey*>(file->GetListOfKeys()->At(0))};
  const TDirectory* const dsDir{file->GetDirectory(dsKey->GetName())};
  TIter dsIter{dsDir->GetListOfKeys()};

  for (TKey* mKey{nullptr}; (mKey = dynamic_cast<TKey*>(dsIter())) != nullptr;) {
    if (!TString(mKey->GetName()).BeginsWith("Method_")) continue;

    if (!gROOT->GetClass(mKey->GetClassName())->InheritsFrom("TDirectory")) {
      continue;
    }

    const TDirectory* const mDir = dynamic_cast<TDirectory*>(mKey->ReadObj());
    TIter mIter(mDir->GetListOfKeys());

    for (TKey* tKey{nullptr}; (tKey = dynamic_cast<TKey*>(mIter())) != nullptr;) {
      if (!gROOT->GetClass(tKey->GetClassName())->InheritsFrom("TDirectory")) {
        continue;
      }

      methods.emplace_back(mKey->GetName(), tKey);
      if (!methods.back().CheckHealth()) methods.pop_back();
    }
  }

  if (methods.size() != 1) {
    LogError("Expected a single method, but have {}", methods.size());
    exit(1);
  }
}

void TMVAReaderWrapper::SetDatasetSplit(const DatasetSplit value) {
  for (MethodHistograms& method : methods) method.SetDatasetSplit(value);
}

void TMVAReaderWrapper::SetTreeAddresses(TTree* const tree) {
  for (const auto [observable, branchContainer] : std::views::zip(*observables, dEvent)) {
    tree->SetBranchAddress(observable.name, &branchContainer);
  }
}

void TMVAReaderWrapper::GetTreeEntry(TTree* const tree, const int64_t idx) {
  tree->GetEntry(idx);
  for (size_t jdx{0}; jdx < fEvent.size(); ++jdx) {
    fEvent[jdx] = static_cast<float>(dEvent[jdx]);
  }
}

double TMVAReaderWrapper::EvaluateMVA() { return reader.EvaluateMVA(methodTag); }

bool TMVAReaderWrapper::EvaluateCutsMVACut(const double cut) {
  return reader.EvaluateMVA(methodTag, cut) == 1;
}

bool TMVAReaderWrapper::EvaluateBDTMVACut(const double cut) {
  return reader.EvaluateMVA(methodTag) > cut;
}

CutInfo InterpolateOptimalCut(std::filesystem::path& datasetPath, const double nTotalSignalEvents) {
  auto df{ROOT::RDF::FromCSV(datasetPath.replace_extension("csv").c_str())};

  auto xvector_resultptr{df.Take<double>("nSignalEvents")};
  auto yvector_resultptr{df.Take<double>("optimalCut")};
  auto zvector_resultptr{df.Take<double>("significance")};

  std::vector<double>* const xvector{xvector_resultptr.GetPtr()};
  std::vector<double>* const yvector{yvector_resultptr.GetPtr()};
  std::vector<double>* const zvector{zvector_resultptr.GetPtr()};
  std::ranges::sort(std::views::zip(*xvector, *yvector, *zvector));

  const auto xIt{std::ranges::lower_bound(*xvector, nTotalSignalEvents)};
  if (xIt == xvector->begin()) {
    return {.cut = yvector->front(), .significance = zvector->front()};
  }
  if (xIt == xvector->end()) {
    return {.cut = yvector->back(), .significance = zvector->back()};
  }

  const size_t idx{static_cast<size_t>(std::distance(xvector->begin(), xIt))};

  const double xmin{xvector->at(idx - 1)};
  const double xmax{xvector->at(idx)};
  const double t{(nTotalSignalEvents - xmin) / (xmax - xmin)};

  const double ymin{yvector->at(idx - 1)};
  const double ymax{yvector->at(idx)};

  const double zmin{zvector->at(idx - 1)};
  const double zmax{zvector->at(idx)};

  return {.cut = std::lerp(ymin, ymax, t), .significance = std::lerp(zmin, zmax, t)};
}
}  // namespace HNL::ML
