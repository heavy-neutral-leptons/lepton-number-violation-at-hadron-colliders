// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

// ROOT
#include "TH1.h"
#include "TTree.h"

// HNL
#include "Statistics/Separation.hpp"

namespace HNL::ML {
/**
 * Description of a cut on the classifier response.
 *
 * The same structure is used both for optimized cuts and for manually supplied cut grids.
 * `expectedSignificance` is meaningful when the cut has been derived from a significance scan.
 */
struct CutInfo {
  double cut{0};
  double nSignalEvents{0};
  double nBkgrndEvents{0};
  double significance{0};
};

/**
 * Histograms for a MVA method.
 *
 * Contains:
 * - Probability density functions, P(x)
 * - Efficiencies
 * \f[
 * \epsilon(x) = 1 - \int_{-1}^{x} P(x') \d{x'}
 * \f]
 */
struct DatasetSplitHistograms {
  TH1* signalPdf{nullptr};  ///< Signal probability distribution
  TH1* signalEff{nullptr};  ///< Signal efficiency

  TH1* bkgrndPdf{nullptr};  ///< Background probability distribution
  TH1* bkgrndEff{nullptr};  ///< Background efficiency

  TH1* significance{nullptr};                         ///< Significance histogram
  CutInfo optimalCut{.cut = -1, .significance = -1};  ///< Optimal cut

  /* -------------------------------------------------------------------------------------------- */

  /**
   * Check that the PDF histograms are available.
   *
   * @return `true` when all required histograms are non-null.
   */
  [[nodiscard]] bool CheckHealthPdf() const { return signalPdf != nullptr && bkgrndPdf != nullptr; }

  /**
   * Check that the Efficiency histograms are available.
   *
   * @return `true` when all required histograms are non-null.
   */
  [[nodiscard]] bool CheckHealthEff() const { return signalEff != nullptr && bkgrndEff != nullptr; }

  /**
   * Check that the PDF and efficiency histograms are available.
   *
   * @return `true` when all required histograms are non-null.
   */
  [[nodiscard]] bool CheckHealth() const {
    return signalPdf != nullptr && signalEff != nullptr && bkgrndPdf != nullptr &&
           bkgrndEff != nullptr;
  }

  /* -------------------------------------------------------------------------------------------- */

  /**
   * Minimum cut, according to signal efficiency.
   */
  [[nodiscard]] double CutMin() const { return signalEff->GetXaxis()->GetBinLowEdge(1); }
  /**
   * Maximum cut, according to signal efficiency.
   */
  [[nodiscard]] double CutMax() const { return signalEff->GetXaxis()->GetBinUpEdge(EffBinCount()); }

  /* -------------------------------------------------------------------------------------------- */

  ///@name Probability density functions
  ///@{
  /**
   * Number of signal PDF bins.
   */
  [[nodiscard]] int PdfBinCount() const { return signalPdf->GetXaxis()->GetNbins(); }
  /**
   * Width of signal PDF bin.
   */
  [[nodiscard]] double PdfBinWidth(const int bin) const {
    return signalPdf->GetXaxis()->GetBinWidth(bin);
  }
  /**
   * Left edge of signal PDF bin.
   */
  [[nodiscard]] double PdfBinMin(const int bin) const {
    return signalPdf->GetXaxis()->GetBinLowEdge(bin);
  }
  /**
   * Right edge of signal PDF bin.
   */
  [[nodiscard]] double PdfBinMax(const int bin) const {
    return signalPdf->GetXaxis()->GetBinUpEdge(bin);
  }
  /**
   * Center of signal PDF bin.
   *
   * @param bin ROOT bin index.
   * @param alt If true, return minimum and maximum cut for the first and last bins, respectively.
   */
  [[nodiscard]] double PdfBinCenter(const int bin, const bool alt = false) const {
    if (alt) {
      if (bin == 1) return CutMin();
      if (bin == PdfBinCount()) return CutMax();
    }
    return signalPdf->GetXaxis()->GetBinCenter(bin);
  }

  /**
   * Compute separation between signal and background PDFs.
   */
  [[nodiscard]] Statistics::DistComparison Separation() const {
    return Statistics::Separation(signalPdf, bkgrndPdf);
  }

  /**
   * Plot signal and background probability density functions.
   *
   * @param lstyle Line style
   * @param dColor Shift from kBlue and kRed for signal and background, respectively
   */
  void PlotPdf(Style_t lstyle, int16_t dColor = 0) const;
  ///@}

  /* -------------------------------------------------------------------------------------------- */

  ///@name Efficiencies
  ///@{
  /**
   * Number of signal efficiency bins.
   */
  [[nodiscard]] int EffBinCount() const { return signalEff->GetXaxis()->GetNbins(); }
  /**
   * Width of signal efficiency bin.
   */
  [[nodiscard]] double EffBinWidth(const int bin) const {
    return signalEff->GetXaxis()->GetBinWidth(bin);
  }
  /**
   * Left edge of signal efficiency bin.
   */
  [[nodiscard]] double EffBinMin(const int bin) const {
    return signalEff->GetXaxis()->GetBinLowEdge(bin);
  }
  /**
   * Right edge of signal efficiency bin.
   */
  [[nodiscard]] double EffBinMax(const int bin) const {
    return signalEff->GetXaxis()->GetBinUpEdge(bin);
  }
  /**
   * Center of signal efficiency bin.
   *
   * @param bin ROOT bin index.
   * @param alt If true, return minimum and maximum cut for the first and last bins, respectively.
   */
  [[nodiscard]] double EffBinCenter(const int bin, const bool alt = false) const {
    if (alt) {
      if (bin == 1) return CutMin();
      if (bin == EffBinCount()) return CutMax();
    }

    return signalEff->GetXaxis()->GetBinCenter(bin);
  }

  /**
   * Plot signal and background efficiencies.
   *
   * @param lstyle Line style
   * @param dColor Shift from kBlue and kRed for signal and background, respectively
   */
  void PlotEff(Style_t lstyle, int16_t dColor = 0) const;
  ///@}

  /* -------------------------------------------------------------------------------------------- */

  ///@name Significance
  ///@{
  /**
   * Compute the significance histogram and return the best cut.
   *
   * The significance is evaluated bin-by-bin from the stored signal and background efficiencies and
   * the supplied total event yields.
   *
   * @param name Name of the significance histogram to create.
   * @param nTotalSignalEvents Expected total number of signal events.
   * @param nTotalBkgrndEvents Expected total number of background events.
   * @return Cut with the largest expected significance.
   */
  CutInfo OptimalCut(const char* name, double nTotalSignalEvents, double nTotalBkgrndEvents);
  ///@}

  /* -------------------------------------------------------------------------------------------- */

  /**
   * Prepare to plot.
   *
   * @param lstyle Line style
   * @param dColor Shift from kBlue and kRed for signal and background, respectively
   */
  void PreparePlot(Style_t lstyle, int16_t dColor = 0) const;
};
}  // namespace HNL::ML
