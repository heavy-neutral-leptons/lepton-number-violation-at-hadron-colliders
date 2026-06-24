// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

// Standard Library
#include <filesystem>

// ROOT
#include "Statistics/Separation.hpp"
#include "TFile.h"

// HNL
#include "Common/Observable.hpp"
#include "MachineLearning/MethodHistograms.hpp"
#include "MachineLearning/TMVAReaderWrapper.hpp"
#include "ReconstructedData/ReconstructedEventData.hpp"

namespace HNL::ML {
/**
 * Build cumulative survival-rate histograms from processed event samples.
 *
 * The resulting ROOT file can be used to replace TMVA's built-in pdf and efficiency histograms with
 * those measured directly from the event samples used by the analysis.
 */
class TMVASurvivalRate : public TMVAReaderWrapper {
 public:
  /**
   * Create a survival-rate accumulator for a trained dataset.
   *
   * The constructor opens the output ROOT file immediately by appending `-SurvivalRate.root` to the
   * supplied dataset path.
   *
   * @param datasetPath Dataset path used to recover the trained method and to build the output file
   * name.
   * @param observables Observable definitions expected in the input trees.
   */
  explicit TMVASurvivalRate(std::filesystem::path& datasetPath,
                            const std::vector<Common::Observable>* observables);
  TMVASurvivalRate(const TMVASurvivalRate&) = delete;
  TMVASurvivalRate(TMVASurvivalRate&&) = delete;
  TMVASurvivalRate& operator=(const TMVASurvivalRate&) = delete;
  TMVASurvivalRate& operator=(TMVASurvivalRate&&) = delete;
  /**
   * Finalize and write the accumulated histograms.
   */
  ~TMVASurvivalRate();

  /**
   * Accumulate signal survival information from one processed signal file.
   *
   * @param events Reconstructed signal events to evaluate with the trained classifier.
   */
  void ProcessSignalTree(Reconstruct::ReconstructedEventData& events) { ProcessTree(events, true); }

  /**
   * Accumulate background survival information from one processed background file.
   *
   * @param events Reconstructed background events to evaluate with the trained classifier.
   */
  void ProcessBkgrndTree(Reconstruct::ReconstructedEventData& events) {
    ProcessTree(events, false);
  }

  /**
   * Compute separation between signal and background PDFs in training data under the first method
   * to be trained.
   */
  Statistics::DistComparison SeparationTrain2() const {
    return Statistics::Separation(signalTrainPdf, bkgrndTrainPdf);
  }
  /**
   * Compute separation between signal and background PDFs in validation data under the first method
   * to be trained.
   */
  Statistics::DistComparison SeparationValid2() const {
    return Statistics::Separation(signalValidPdf, bkgrndValidPdf);
  }
  /**
   * Compute separation between signal and background PDFs in validation data under the first method
   * to be trained.
   */
  Statistics::DistComparison SeparationApply2() const {
    return Statistics::Separation(signalApplyPdf, bkgrndApplyPdf);
  }

 private:
  void SavePNG();
  void ProcessTree(Reconstruct::ReconstructedEventData& events, bool isSignal);

  const int nBinsPdf{Method().PdfBinCount()};
  const int nBinsEff{Method().EffBinCount()};
  const double min{Method().CutMin()};
  const double max{Method().CutMax()};

  TFile* outputFile{nullptr};

  double totalSignalIntegral{0};
  double totalBkgrndIntegral{0};

  std::filesystem::path pngOutputPath;

  constexpr static const char* const s1p{"signalTrainPdf"};
  TH1D* const signalTrainPdf{new TH1D{s1p, s1p, nBinsPdf, min, max}};
  constexpr static const char* const s1e{"signalTrainEff"};
  TH1D* const signalTrainEff{new TH1D{s1e, s1e, nBinsEff, min, max}};

  constexpr static const char* const s2p{"signalValidPdf"};
  TH1D* const signalValidPdf{new TH1D{s2p, s2p, nBinsPdf, min, max}};
  constexpr static const char* const s2e{"signalValidEff"};
  TH1D* const signalValidEff{new TH1D{s2e, s2e, nBinsEff, min, max}};

  constexpr static const char* const s3p{"signalApplyPdf"};
  TH1D* const signalApplyPdf{new TH1D{s3p, s3p, nBinsPdf, min, max}};
  constexpr static const char* const s3e{"signalApplyEff"};
  TH1D* const signalApplyEff{new TH1D{s3e, s3e, nBinsEff, min, max}};

  constexpr static const char* const b1p{"bkgrndTrainPdf"};
  TH1D* const bkgrndTrainPdf{new TH1D{b1p, b1p, nBinsPdf, min, max}};
  constexpr static const char* const b1e{"bkgrndTrainEff"};
  TH1D* const bkgrndTrainEff{new TH1D{b1e, b1e, nBinsEff, min, max}};
  constexpr static const char* const b1et{"bkgrndTrainEffT"};
  TH1D* const bkgrndTrainEffT{new TH1D{b1et, b1et, nBinsEff, min, max}};

  constexpr static const char* const b2p{"bkgrndValidPdf"};
  TH1D* const bkgrndValidPdf{new TH1D{b2p, b2p, nBinsPdf, min, max}};
  constexpr static const char* const b2e{"bkgrndValidEff"};
  TH1D* const bkgrndValidEff{new TH1D{b2e, b2e, nBinsEff, min, max}};
  constexpr static const char* const b2et{"bkgrndValidEffT"};
  TH1D* const bkgrndValidEffT{new TH1D{b2et, b2et, nBinsEff, min, max}};

  constexpr static const char* const b3p{"bkgrndApplyPdf"};
  TH1D* const bkgrndApplyPdf{new TH1D{b3p, b3p, nBinsPdf, min, max}};
  constexpr static const char* const b3e{"bkgrndApplyEff"};
  TH1D* const bkgrndApplyEff{new TH1D{b3e, b3e, nBinsEff, min, max}};
  constexpr static const char* const b3et{"bkgrndApplyEffT"};
  TH1D* const bkgrndApplyEffT{new TH1D{b3et, b3et, nBinsEff, min, max}};
};
}  // namespace HNL::ML
