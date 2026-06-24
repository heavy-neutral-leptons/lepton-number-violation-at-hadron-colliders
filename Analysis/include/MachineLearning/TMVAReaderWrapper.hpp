// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

// Standard Library
#include <filesystem>
#include <string>
#include <vector>

// ROOT
#include "TMVA/Reader.h"

// HNL
#include "Common/Observable.hpp"
#include "MachineLearning/MethodHistograms.hpp"
#include "MachineLearning/Methods.hpp"

namespace HNL::ML {
/**
 * Common TMVA reader backend used after a dataset has been trained.
 *
 * The wrapper binds the configured observables to a `TMVA::Reader`, loads the corresponding XML
 * weights file, discovers the trained method information from the TMVA ROOT output, and provides
 * helper functions used by the higher-level application, plotting, and optimization classes.
 */
class TMVAReaderWrapper {
 public:
  /**
   * Construct a reader for a trained dataset.
   *
   * @param datasetPath Dataset directory used to recover the method, ROOT file, and XML weights
   * file.
   * @param observables Observable definitions expected in the processed trees.
   */
  TMVAReaderWrapper(const std::filesystem::path& datasetPath,
                    const std::vector<Common::Observable>* observables);

  /**
   * Return trained method recovered from the TMVA output.
   */
  MethodHistograms& Method(const size_t idx = 0) { return methods.at(idx); }
  /**
   * Const overload of `MainMethod()`.
   */
  const MethodHistograms& Method(const size_t idx = 0) const { return methods.at(idx); }

  /**
   * Select dataset spilt to use.
   *
   * @param value Dataset split to use
   */
  void SetDatasetSplit(DatasetSplit value);

  /**
   * Compute the expected significance for a given cut value under the first method to be trained.
   *
   * @param cut Cut value
   * @param nTotalSignalEvents Expected number of signal events
   * @param nTotalBkgrndEvents Expected number of background events
   *
   * @return Cut value and expected significance
   */
  CutInfo ProcessCut(double cut, double nTotalSignalEvents, double nTotalBkgrndEvents) const {
    return Method().ProcessCut(cut, nTotalSignalEvents, nTotalBkgrndEvents);
  }

  /**
   * Compute cut which maximizes the significance under the first method to be trained.
   *
   * Produces an histogram containing the expected significance as a function of the cut value.
   *
   * @param nTotalSignalEvents Expected number of signal events
   * @param nTotalBkgrndEvents Expected number of background events
   *
   * @return Optimal cut value and expected significance
   */
  CutInfo OptimalCut(double nTotalSignalEvents, double nTotalBkgrndEvents) {
    return Method().OptimalCut(nTotalSignalEvents, nTotalBkgrndEvents);
  }

  /**
   * Compute separation between signal and background PDFs in training data under the first method
   * to be trained.
   */
  Statistics::DistComparison SeparationTrain() const { return Method().SeparationTrain(); }
  /**
   * Compute separation between signal and background PDFs in validation data under the first method
   * to be trained.
   */
  Statistics::DistComparison SeparationValid() const { return Method().SeparationValid(); }
  /**
   * Compute separation between signal and background PDFs in validation data under the first method
   * to be trained.
   */
  Statistics::DistComparison SeparationApply() const { return Method().SeparationApply(); }

 protected:
  /**
   * Bind the configured observables to a tree.
   *
   * @param tree Tree whose branches will be read by this reader.
   */
  void SetTreeAddresses(TTree* tree);
  /**
   * Load one event from the tree into the reader buffers.
   *
   * The implementation reads double-precision branch storage from the tree and mirrors it into the
   * float buffers consumed by `TMVA::Reader`.
   *
   * @param tree Input tree.
   * @param idx Entry index to load.
   */
  void GetTreeEntry(TTree* tree, int64_t idx);

  /**
   * Evaluate the classifier response for the currently loaded event.
   *
   * @return Classifier response.
   */
  [[nodiscard]] double EvaluateMVA();

  /**
   * Evaluate whether the current event survives the cut.
   */
  bool (TMVAReaderWrapper::*EvaluateMVACut)(double cut);
  /**
   * Evaluate whether the current event survives the cut (Cuts).
   */
  [[nodiscard]] bool EvaluateCutsMVACut(double cut);
  /**
   * Evaluate whether the current event survives the cut (BDT).
   */
  [[nodiscard]] bool EvaluateBDTMVACut(double cut);

 private:
  void FetchMethodInfo(const std::filesystem::path& rootPath);

 protected:
  TMVA::Reader reader{"Silent"};

  std::vector<float> fEvent;   ///< TMVA reader buffers.
  std::vector<double> dEvent;  ///< Temporary tree buffers bound to ROOT branches.

  const MVAMethod method;       ///< Method inferred from the dataset path.
  const std::string methodTag;  ///< TMVA booking name of the method.
  const std::vector<Common::Observable>* const
      observables;  ///< Observable set expected in input trees.

  std::vector<MethodHistograms> methods;  ///< Methods discovered in the TMVA output ROOT file.
};

/**
 * Compute cut which maximizes the significance by interpolating from pre-defined points.
 *
 * @param datasetPath Path to the TMVA dataset; the extension is replaced by csv
 * @param nTotalSignalEvents Expected number of signal events
 *
 * @return Interpolated optimal cut value and expected significance
 */
CutInfo InterpolateOptimalCut(std::filesystem::path& datasetPath, double nTotalSignalEvents);
}  // namespace HNL::ML
