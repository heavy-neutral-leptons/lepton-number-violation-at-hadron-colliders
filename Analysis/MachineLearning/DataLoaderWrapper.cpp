// Copyright 2023-2026 Bruno M. S. Oliveira

// Standard Library
#include <ranges>
#include <string>
#include <vector>

// ROOT
#include "MachineLearning/DataLoaderWrapper.hpp"
#include "MachineLearning/DatasetSplit.hpp"

namespace HNL::ML {
DataLoaderWrapper::DataLoaderWrapper(const std::string& datasetName,
                                     const std::vector<Common::Observable>* const observables)
    : observables{observables}, dataLoader{datasetName}, event(observables->size()) {
  for (const Common::Observable& observable : *observables) {
    dataLoader.AddVariable(observable.name, observable.title, observable.unit);
  }
}

void DataLoaderWrapper::AddSignalTree(TTree* const tree, const double treeWeight) {
  SetTreeAddresses(tree);

  const double eventWeight{treeWeight / static_cast<double>(tree->GetEntries())};
  for (UInt_t idx = 0; idx < tree->GetEntries(); ++idx) {
    tree->GetEntry(idx);

    switch (AssignDatasetSplit(idx)) {
      case DatasetSplit::Training:
        AddSignalTrainingEvent(eventWeight);
        break;
      case HNL::ML::DatasetSplit::Validation:
        AddSignalValidationEvent(eventWeight);
        break;
      case DatasetSplit::Application:
        break;
    }
  }
}

void DataLoaderWrapper::AddBkgrndTree(TTree* const tree, const double treeWeight) {
  SetTreeAddresses(tree);

  const double eventWeight{treeWeight / static_cast<double>(tree->GetEntries())};
  for (UInt_t idx = 0; idx < tree->GetEntries(); ++idx) {
    tree->GetEntry(idx);

    switch (AssignDatasetSplit(idx)) {
      case DatasetSplit::Training:
        AddBkgrndTrainingEvent(eventWeight);
        break;
      case HNL::ML::DatasetSplit::Validation:
        AddBkgrndValidationEvent(eventWeight);
        break;
      case DatasetSplit::Application:
        break;
    }
  }
}

void DataLoaderWrapper::AddBkgrndTrees(const std::vector<TTree*>& trees, const double treeWeight) {
  size_t nSimulatedEvents{0};
  for (const TTree* const tree : trees) nSimulatedEvents += tree->GetEntries();

  const double eventWeight{treeWeight / static_cast<double>(nSimulatedEvents)};

  for (TTree* const tree : trees) {
    SetTreeAddresses(tree);

    for (UInt_t idx = 0; idx < tree->GetEntries(); ++idx) {
      tree->GetEntry(idx);

      switch (AssignDatasetSplit(idx)) {
        case DatasetSplit::Training:
          AddBkgrndTrainingEvent(eventWeight);
          break;
        case HNL::ML::DatasetSplit::Validation:
          AddBkgrndValidationEvent(eventWeight);
          break;
        case DatasetSplit::Application:
          break;
      }
    }
  }
}

void DataLoaderWrapper::Prepare(const std::string& options) {
  dataLoader.PrepareTrainingAndTestTree("", options);
}

void DataLoaderWrapper::SetTreeAddresses(TTree* tree) {
  for (const auto [observable, container] : std::views::zip(*observables, event)) {
    tree->SetBranchAddress(observable.name, &container);
  }
}

void DataLoaderWrapper::AddSignalTrainingEvent(const double eventWeight) {
  dataLoader.AddSignalTrainingEvent(event, eventWeight);
}

void DataLoaderWrapper::AddBkgrndTrainingEvent(const double eventWeight) {
  dataLoader.AddBackgroundTrainingEvent(event, eventWeight);
}

void DataLoaderWrapper::AddSignalValidationEvent(const double eventWeight) {
  dataLoader.AddSignalTestEvent(event, eventWeight);
}

void DataLoaderWrapper::AddBkgrndValidationEvent(const double eventWeight) {
  dataLoader.AddBackgroundTestEvent(event, eventWeight);
}
}  // namespace HNL::ML
