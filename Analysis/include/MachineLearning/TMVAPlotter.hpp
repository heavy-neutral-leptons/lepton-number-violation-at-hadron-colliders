// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

// Standard Library
#include <filesystem>
#include <string>
#include <vector>

// ROOT
#include "THStack.h"
#include "TLegend.h"

// HNL
#include "Common/Observable.hpp"
#include "MachineLearning/MethodHistograms.hpp"
#include "MachineLearning/TMVAReaderWrapper.hpp"
#include "ReconstructedData/ReconstructedEventData.hpp"

namespace HNL::ML {
/**
 * Build classifier-response plots and cut-performance summaries.
 *
 * This class processes signal and background event samples with a trained TMVA classifier, stores
 * their response distributions split by train/validation/application subsets, and exports combined
 * diagnostic plots and CSV summaries.
 */
class TMVAPlotter : public TMVAReaderWrapper {
 public:
  /**
   * Construct a plotter for a trained dataset.
   *
   * @param datasetPath Path identifying the trained dataset.
   * @param observables Observable definitions expected in the input trees.
   * @param signalLuminosity Luminosity used to normalize signal trees.
   * @param bkgrndLuminosity Luminosity used to normalize background trees.
   */
  explicit TMVAPlotter(const std::filesystem::path& datasetPath,
                       const std::vector<Common::Observable>* const observables,
                       const double signalLuminosity, const double bkgrndLuminosity)
      : TMVAReaderWrapper{datasetPath, observables},
        signalLuminosity{signalLuminosity},
        bkgrndLuminosity{bkgrndLuminosity} {}

  /**
   * Process one signal tree and accumulate its response distribution.
   *
   * @param events Reconstructed signal events to process.
   */
  void ProcessSignalTree(Reconstruct::ReconstructedEventData& events);
  /**
   * Process one background tree and accumulate its response distribution.
   *
   * @param events Reconstructed background events to process.
   */
  void ProcessBkgrndTree(Reconstruct::ReconstructedEventData& events);

  /**
   * Compute the cut that maximizes the expected significance.
   *
   * The optimization uses the total background yield accumulated from all processed background
   * trees together with the total signal yield from the first processed signal tree.
   *
   * @return Optimal cut value and expected significance
   */
  CutInfo OptimalCut() { return Method().OptimalCut(nTotalSignalEvents, nTotalBkgrndEvents); }

  /**
   * Save the combined diagnostic canvas as a PNG image.
   *
   * @param outPath Output path.
   */
  void SavePNG(const std::filesystem::path& outPath);
  /**
   * Save the processed classifier-response histograms as CSV.
   *
   * @param outPath Output path.
   */
  void SaveCSV(const std::filesystem::path& outPath) const;

  /**
   * Return the total signal yield taken from the first processed signal tree.
   */
  [[nodiscard]] double NTotalSignalEvents() const { return nTotalSignalEvents; }
  /**
   * Return the accumulated total background yield.
   */
  [[nodiscard]] double NTotalBkgrndEvents() const { return nTotalBkgrndEvents; }

 private:
  /**
   * Process one tree into train/validation/application response histograms.
   *
   * @param tree Input tree.
   * @param weight Total expected yield represented by the tree.
   * @param lineColor Color assigned to the corresponding histograms.
   */
  void ProcessTree(TTree* tree, double weight, int16_t lineColor);

  /**
   * Compute the number of events surviving a given cut in one histogram.
   *
   * @param idx Histogram index inside the train/test stack.
   * @param cut Cut to be applied.
   * @param split Dataset split to use.
   * @return Expected surviving yield above the supplied cut.
   */
  double SurvivingEvents(int idx, double cut, DatasetSplit split);

  /**
   * Number of classifier response bins
   */
  [[nodiscard]] int NBins() const { return bins; }
  /**
   * Minimum classifier response
   */
  [[nodiscard]] double Min() const { return min; }
  /**
   * Maximum classifier response
   */
  [[nodiscard]] double Max() const { return max; }

  /**
   * Width of classifier response bin.
   */
  [[nodiscard]] double BinWidth(const int bin) const {
    return trainStack.GetXaxis()->GetBinWidth(bin);
  }
  /**
   * Left edge of classifier response bin.
   */
  [[nodiscard]] double BinMin(const int bin) const {
    return trainStack.GetXaxis()->GetBinLowEdge(bin);
  }
  /**
   * Right edge of classifier response bin.
   */
  [[nodiscard]] double BinMax(const int bin) const {
    return trainStack.GetXaxis()->GetBinUpEdge(bin);
  }
  /**
   * Center of classifier response bin.
   *
   * @param alt If true, return minimum and maximum cut for the first and last bins, respectively.
   */
  [[nodiscard]] double BinCenter(const int bin, const bool alt = false) const {
    if (alt) {
      if (bin == 1) return Min();
      if (bin == NBins()) return Max();
    }
    return trainStack.GetXaxis()->GetBinCenter(bin);
  }

  /**
   * Print histogram contents for a bin as a comma-separated list.
   */
  void BinToCSV(std::ofstream& stream, int bin) const;
  /**
   * Print empty comma-separated list for first bin
   */
  void ZeroBinToCSV(std::ofstream& stream) const;
  /**
   * Print empty comma-separated list for last bin
   */
  void LastBinToCSV(std::ofstream& stream) const;

  /**
   * Index of first background to be processed.
   */
  int FirstBackgroundIndex() const {
    return trainStack.GetNhists() - static_cast<int>(nBkgrndEvents.size());
  }

  constexpr static int bins{20};        ///< Number of bins
  const double min{Method().CutMin()};  ///< Minimum classifier response
  const double max{Method().CutMax()};  ///< Maximum classifier response

  const double signalLuminosity;  ///< Luminosity of signal trees (pb^{-1})
  double nTotalSignalEvents{0};   ///< Total number of signal events

  const double bkgrndLuminosity;         ///< Luminosity of background trees
  std::vector<double> nBkgrndEvents{0};  ///< Yield represented by each processed background tree.
  double nTotalBkgrndEvents{0};          ///< Total number of background events

  std::vector<std::string> sourceNames;  ///< Names of processed trees

  TLegend sourcesLegend{0.1, 0.1, 0.9, 0.9};  ///< Legend with names of processed trees
  THStack trainStack{"trainStack", "Training Data Distribution"};     ///< Training data
  THStack validStack{"validStack", "Validation Data Distribution"};   ///< Validation data
  THStack applyStack{"applyStack", "Application Data Distribution"};  ///< Application data

  TH1D trainBkgrnd{"trainBkgrnd", "Background (Training Data)", bins, min, max};
  TH1D validBkgrnd{"validBkgrnd", "Background (Validation Data)", bins, min, max};
  TH1D applyBkgrnd{"applyBkgrnd", "Background (Application Data)", bins, min, max};
};
}  // namespace HNL::ML
