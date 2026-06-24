// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

// Standard Library
#include <string>
#include <vector>

// ROOT
#include "TMVA/DataLoader.h"

// HNL
#include "Common/Observable.hpp"

namespace HNL::ML {
/**
 * Thin adapter around `TMVA::DataLoader` for analysis trees.
 *
 * This class registers the configured observables as TMVA variables, binds the corresponding
 * branches in input trees, and forwards weighted events to TMVA after assigning each event to the
 * deterministic split defined in `DatasetSplit.hpp`.
 *
 * Only the training and validation partitions are forwarded to TMVA. Events in the application
 * partition are intentionally left unused at this stage.
 */
class DataLoaderWrapper {
 public:
  /**
   * Create a data-loader wrapper for a named TMVA dataset.
   *
   * @param datasetName Name passed to the underlying `TMVA::DataLoader`.
   * @param observables Observable definitions to register as TMVA variables.
   */
  explicit DataLoaderWrapper(const std::string& datasetName,
                             const std::vector<Common::Observable>* observables);

  /**
   * Add a signal tree to the loader.
   *
   * The tree is traversed event-by-event. The supplied tree weight is converted into a per-event
   * weight and only the training/validation partitions are forwarded to TMVA.
   *
   * @param tree Input signal tree.
   * @param treeWeight Total expected weight of the full tree.
   */
  void AddSignalTree(TTree* tree, double treeWeight);
  /**
   * Add a background tree to the loader.
   *
   * The tree is traversed event-by-event. The supplied tree weight is converted into a per-event
   * weight and only the training/validation partitions are forwarded to TMVA.
   *
   * @param tree Input background tree.
   * @param treeWeight Total expected weight of the full tree.
   */
  void AddBkgrndTree(TTree* tree, double treeWeight);
  /**
   * Add multiple background trees that share a common total normalization.
   *
   * The supplied `treeWeight` is distributed across all events in all trees, so each background
   * event receives the same per-event weight regardless of its source tree.
   *
   * @param trees Input background trees.
   * @param treeWeight Total expected weight of all trees combined.
   */
  void AddBkgrndTrees(const std::vector<TTree*>& trees, double treeWeight);
  /**
   * Finalize the TMVA training/test trees.
   *
   * @param options Option string forwarded to `TMVA::DataLoader::PrepareTrainingAndTestTree`.
   */
  void Prepare(const std::string& options);

  /**
   * Access the wrapped `TMVA::DataLoader`.
   *
   * @return Pointer to the underlying TMVA object.
   */
  [[nodiscard]] TMVA::DataLoader* Underlying() { return &dataLoader; }

 private:
  void SetTreeAddresses(TTree* tree);

  void AddSignalTrainingEvent(double eventWeight);
  void AddBkgrndTrainingEvent(double eventWeight);
  void AddSignalValidationEvent(double eventWeight);
  void AddBkgrndValidationEvent(double eventWeight);

  const std::vector<Common::Observable>* const observables;

  TMVA::DataLoader dataLoader{nullptr};
  std::vector<double> event;
};
}  // namespace HNL::ML
