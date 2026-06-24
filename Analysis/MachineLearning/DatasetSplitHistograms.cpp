// Copyright 2023-2026 Bruno M. S. Oliveira

// ROOT
#include "Common/Log.hpp"
#include "TMVA/Tools.h"
#include "TTree.h"

// HNL
#include "MachineLearning/DatasetSplitHistograms.hpp"
#include "Statistics/Significance.hpp"

namespace HNL::ML {
void DatasetSplitHistograms::PlotPdf(const Style_t lstyle, const int16_t dColor) const {
  signalPdf->SetLineColor(static_cast<Color_t>(kBlue + dColor));
  signalPdf->SetLineStyle(lstyle);
  signalPdf->Draw("SAME HIST");

  bkgrndPdf->SetLineColor(static_cast<Color_t>(kRed + dColor));
  bkgrndPdf->SetLineStyle(lstyle);
  bkgrndPdf->Draw("SAME HIST");
}

void DatasetSplitHistograms::PlotEff(const Style_t lstyle, const int16_t dColor) const {
  signalEff->SetLineColor(static_cast<Color_t>(kBlue + dColor));
  signalEff->SetLineStyle(lstyle);
  signalEff->Draw("SAME");

  bkgrndEff->SetLineColor(static_cast<Color_t>(kRed + dColor));
  bkgrndEff->SetLineStyle(lstyle);
  bkgrndEff->Draw("SAME");
}

void DatasetSplitHistograms::PreparePlot(const Style_t lstyle, const int16_t dColor) const {
  signalPdf->SetLineColor(static_cast<Color_t>(kBlue + dColor));
  signalPdf->SetLineStyle(lstyle);

  bkgrndPdf->SetLineColor(static_cast<Color_t>(kRed + dColor));
  bkgrndPdf->SetLineStyle(lstyle);

  signalEff->SetLineColor(static_cast<Color_t>(kBlue + dColor));
  signalEff->SetLineStyle(lstyle);

  bkgrndEff->SetLineColor(static_cast<Color_t>(kRed + dColor));
  bkgrndEff->SetLineStyle(lstyle);
}

CutInfo DatasetSplitHistograms::OptimalCut(const char* const name, const double nTotalSignalEvents,
                                           const double nTotalBkgrndEvents) {
  if (significance != nullptr) {
    delete significance;

    optimalCut.cut = 0;
    optimalCut.nSignalEvents = 0;
    optimalCut.nBkgrndEvents = 0;
    optimalCut.significance = -1;
  }

  if (!CheckHealthEff()) {
    LogError("Attempting to compute optimal cut without efficiency histograms ({} {}).",
             signalEff == nullptr, bkgrndEff == nullptr);
    exit(1);
  }

  significance = new TH1D(name, "Significance", EffBinCount(), CutMin(), CutMax());

  for (int bin{1}; bin <= EffBinCount(); ++bin) {
    const double nSignalEvents{signalEff->GetBinContent(bin) * nTotalSignalEvents};
    const double nBkgrndEvents{bkgrndEff->GetBinContent(bin) * nTotalBkgrndEvents};

    const double binSignificance{Statistics::ExclusionSignificance(nSignalEvents, nBkgrndEvents)};
    significance->SetBinContent(bin, binSignificance);

    Log<2>("{} ({}): {} ({}) {} ({}) -> {}", bin, EffBinCenter(bin), signalEff->GetBinContent(bin),
           nSignalEvents, bkgrndEff->GetBinContent(bin), nBkgrndEvents, binSignificance);

    if (binSignificance > optimalCut.significance) {
      optimalCut.cut = EffBinCenter(bin);
      optimalCut.nSignalEvents = nSignalEvents;
      optimalCut.nBkgrndEvents = nBkgrndEvents;
      optimalCut.significance = binSignificance;
    }
  }

  Log<1>(
      "With {:.3e} signal events and {:.3e} background events, "
      "the maximum significance is achieved with a cut {:.3f}."
      "This yields {:.3e} signal events and {:.3e} background events, "
      "corresponding to Z = {:.1f} σ.",
      nTotalSignalEvents, nTotalBkgrndEvents, optimalCut.cut, optimalCut.nSignalEvents,
      optimalCut.nBkgrndEvents, optimalCut.significance);

  return optimalCut;
}
}  // namespace HNL::ML
