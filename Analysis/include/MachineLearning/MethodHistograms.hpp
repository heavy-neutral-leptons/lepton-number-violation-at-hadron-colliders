// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

// Standard Library
#include <filesystem>

// ROOT
#include "MachineLearning/DatasetSplit.hpp"
#include "TH1.h"
#include "TKey.h"
#include "TTree.h"

// HNL
#include "MachineLearning/DatasetSplitHistograms.hpp"
#include "Statistics/Separation.hpp"

namespace HNL::ML {
/**
 * Read-only wrapper around the histograms produced by one trained TMVA method.
 *
 * The object exposes the signal/background PDFs and efficiencies stored in the TMVA output ROOT
 * file and can turn them into significance scans for specific expected signal and background
 * yields.
 */
class MethodHistograms {
 public:
  /**
   * Build the wrapper from a method directory in a TMVA output file.
   *
   * @param methodName Name of the TMVA method family
   * @param key ROOT key pointing to the concrete method directory.
   */
  MethodHistograms(const char* methodName, TKey* key);

  /**
   * Check that all required training and validation histograms are present.
   *
   * @return `true` when both dataset splits are usable.
   */
  [[nodiscard]] bool CheckHealth() const {
    return histTrain.CheckHealth() && histValid.CheckHealth();
  };

  /* -------------------------------------------------------------------------------------------- */

  /**
   * Minimum cut.
   */
  [[nodiscard]] double CutMin() const { return histTrain.CutMin(); }
  /**
   * Maximum cut.
   */
  [[nodiscard]] double CutMax() const { return histTrain.CutMax(); }

  /* -------------------------------------------------------------------------------------------- */

  /**
   * Select dataset spilt to use.
   *
   * @param value Dataset split to use
   */
  void SetDatasetSplit(DatasetSplit value);

  /* -------------------------------------------------------------------------------------------- */

  ///@name Probability density functions
  ///@{
  /**
   * Number of PDF bins.
   */
  [[nodiscard]] int PdfBinCount() const { return histTrain.PdfBinCount(); }
  /**
   * Width of PDF bin.
   */
  [[nodiscard]] double PdfBinWidth(const int bin) const { return histTrain.PdfBinWidth(bin); }
  /**
   * Left-edge of PDF bin.
   */
  [[nodiscard]] double PdfBinMin(const int bin) const { return histTrain.PdfBinMin(bin); }
  /**
   * Right-edge of PDF bin.
   */
  [[nodiscard]] double PdfBinMax(const int bin) const { return histTrain.PdfBinMax(bin); }
  /**
   * Center of PDF bin.
   *
   * @param alt If true, return minimum and maximum cut for the first and last bins, respectively.
   */
  [[nodiscard]] double PdfBinCenter(const int bin, const bool alt = false) const {
    return histTrain.PdfBinCenter(bin, alt);
  }

  /**
   * Signal probability density function for the currently selected dataset split.
   */
  [[nodiscard]] TH1* SignalPdf() const { return CurrentHistograms()->signalPdf; }
  /**
   * Signal probability density function for the currently selected dataset split.
   */
  [[nodiscard]] double SignalPdf(const int bin) const { return SignalPdf()->GetBinContent(bin); }

  /**
   * Background probability density function for the currently selected dataset split.
   */
  [[nodiscard]] TH1* BkgrndPdf() const { return CurrentHistograms()->bkgrndPdf; }
  /**
   * Background probability density function for the currently selected dataset split.
   */
  [[nodiscard]] double BkgrndPdf(const int bin) const { return BkgrndPdf()->GetBinContent(bin); }

  /**
   * Compute separation between signal and background PDFs in training data.
   */
  [[nodiscard]] Statistics::DistComparison SeparationTrain() const {
    return histTrain.Separation();
  }
  /**
   * Compute separation between signal and background PDFs in validation data.
   */
  [[nodiscard]] Statistics::DistComparison SeparationValid() const {
    return histValid.Separation();
  }
  /**
   * Compute separation between signal and background PDFs in application data.
   */
  [[nodiscard]] Statistics::DistComparison SeparationApply() const {
    return histApply.Separation();
  }
  ///@}

  /* -------------------------------------------------------------------------------------------- */

  ///@name Efficiencies
  ///@{
  /**
   * Replace the internally stored probability density and efficiency histograms from an external
   * file.
   *
   * @param path ROOT file containing replacement efficiency histograms.
   * @param truncateBkgrnd Whether to use the truncated background efficiencies.
   */
  void ReplaceHistograms(const std::filesystem::path& path, bool truncateBkgrnd);

  /**
   * Number of efficiency bins.
   */
  [[nodiscard]] int EffBinCount() const { return histTrain.EffBinCount(); }
  /**
   * Width of efficiency bin.
   */
  [[nodiscard]] double EffBinWidth(const int bin) const { return histTrain.EffBinWidth(bin); }
  /**
   * Left-edge of efficiency bin.
   */
  [[nodiscard]] double EffBinMin(const int bin) const { return histTrain.EffBinMin(bin); }
  /**
   * Right-edge of efficiency bin.
   */
  [[nodiscard]] double EffBinMax(const int bin) const { return histTrain.EffBinMax(bin); }
  /**
   * Center of efficiency bin.
   *
   * @param alt If true, return minimum and maximum cut for the first and last bins, respectively.
   */
  [[nodiscard]] double EffBinCenter(const int bin, const bool alt = false) const {
    return histTrain.EffBinCenter(bin, alt);
  }

  /**
   * Signal efficiency for the currently selected dataset split.
   */
  [[nodiscard]] double SignalEff(const int bin) const { return SignalEff()->GetBinContent(bin); }
  /**
   * Signal efficiency for the currently selected dataset split.
   */
  [[nodiscard]] TH1* SignalEff() const { return CurrentHistograms()->signalEff; }

  /**
   * Background efficiency for the currently selected dataset split.
   */
  [[nodiscard]] TH1* BkgrndEff() const { return CurrentHistograms()->bkgrndEff; }
  /**
   * Background efficiency for the currently selected dataset split.
   */
  [[nodiscard]] double BkgrndEff(const int bin) const { return BkgrndEff()->GetBinContent(bin); }
  ///@}

  /* -------------------------------------------------------------------------------------------- */

  ///@name Significance
  ///@{
  /**
   * Compute the expected significance for a given cut value.
   *
   * @param cut Cut value
   * @param nTotalSignalEvents Expected number of signal events
   * @param nTotalBkgrndEvents Expected number of background events
   *
   * @return Cut value and expected significance
   */
  [[nodiscard]] CutInfo ProcessCut(double cut, double nTotalSignalEvents,
                                   double nTotalBkgrndEvents) const;
  /**
   * Compute cut which maximizes the significance.
   *
   * Produces an histogram containing the expected significance as a function of the cut value.
   *
   * @param nTotalSignalEvents Expected number of signal events
   * @param nTotalBkgrndEvents Expected number of background events
   *
   * @return Optimal cut value and expected significance
   */
  [[nodiscard]] CutInfo OptimalCut(double nTotalSignalEvents, double nTotalBkgrndEvents);

  /**
   * Return the significance histogram for the selected dataset split.
   *
   * This requires `OptimalCut()` to have been executed first.
   */
  [[nodiscard]] TH1* Significance() const;
  /**
   * Return the significance histogram for the selected dataset split.
   *
   * This requires `OptimalCut()` to have been executed first.
   */
  [[nodiscard]] double Significance(const int bin) const {
    return Significance()->GetBinContent(bin);
  }

  /**
   * Return a clone of the significance histogram for the selected dataset split.
   *
   * This requires `OptimalCut()` to have been executed first.
   */
  [[nodiscard]] TH1* SignificanceClone() const {
    return dynamic_cast<TH1*>(Significance()->Clone("significance_clone"));
  }
  ///@}

  /* -------------------------------------------------------------------------------------------- */

  ///@name Exporting
  ///@{
  /**
   * Saves a png with the probability density functions and efficiencies.
   *
   * @param outPath Output path.
   * @param extraSignalPDF Plot this signal probability density function on top.
   * @param extraBkgrndPDF Plot this background probability density function on top.
   */
  void SavePNG(const std::filesystem::path& outPath, TH1* extraSignalPDF,
               TH1* extraBkgrndPDF) const;
  /**
   * Saves a csv with the efficiencies and significance.
   *
   * Crashes if run before OptimalCut().
   *
   * @param outPath Output path.
   */
  void SaveCSV(const std::filesystem::path& outPath) const;
  ///@}

 private:
  TString methodName{""};   ///< Name of the TMVA method family.
  TString methodTitle{""};  ///< Title of the concrete TMVA method instance.

  DatasetSplit dataset{DatasetSplit::Validation};  ///< Dataset split currently exposed.
  /**
   * MVAHistogram for the currently selected dataset split.
   */
  [[nodiscard]] const DatasetSplitHistograms* CurrentHistograms() const;

  DatasetSplitHistograms histTrain;  ///< Histograms for the training partition.
  DatasetSplitHistograms histValid;  ///< Histograms for the validation partition.
  DatasetSplitHistograms histApply;  ///< Histograms for the application partition.
};
}  // namespace HNL::ML
