// Copyright 2023-2026 Bruno M. S. Oliveira

// Standard Library
#include <fstream>

// ROOT
#include "MachineLearning/DatasetSplit.hpp"
#include "ROOT/RDataFrame.hxx"
#include "TCanvas.h"
#include "TDirectory.h"
#include "TFile.h"
#include "THStack.h"
#include "TMVA/Tools.h"
#include "TTree.h"

// HNL
#include <TH1.h>

#include "Common/Log.hpp"
#include "MachineLearning/MethodHistograms.hpp"
#include "Statistics/Significance.hpp"

namespace HNL::ML {
namespace {
template <typename T>
[[nodiscard]] T* CloneObject(TDirectory* const dir, const char* const namecycle,
                             const char* const cloneName) {
  assert(dir->GetListOfKeys()->Contains(namecycle) && std::format("{} not found", namecycle));
  return dynamic_cast<T*>(dir->Get<T>(namecycle)->Clone(cloneName));
}

template <typename T>
[[nodiscard]] T* CloneObject(TDirectory* const dir, const char* const name) {
  return dynamic_cast<T*>(dir->Get<T>(name)->Clone(name));
}

template <typename T>
void ReplaceObject(T*& obj, TDirectory* const dir, const char* const namecycle) {
  assert(obj && "Cannot replace object which does not exist.");
  assert(dir->GetListOfKeys()->Contains(namecycle));

  const char* const cloneName{obj->GetName()};

  delete obj;
  obj = CloneObject<T>(dir, namecycle, cloneName);
}
}  // namespace

MethodHistograms::MethodHistograms(const char* const methodName, TKey* const key)
    : methodName{methodName} {
  auto* const dir = dynamic_cast<TDirectory*>(key->ReadObj());

  methodTitle = dir->GetName();

  const TString name{"MVA_" + methodTitle};

  // Training
  histTrain.signalPdf = CloneObject<TH1>(dir, name + "_Train_S", "signalTrainPdfMain");
  histTrain.signalEff = CloneObject<TH1>(dir, name + "_trainingEffS", "signalTrainEffMain");

  histTrain.bkgrndPdf = CloneObject<TH1>(dir, name + "_Train_B", "bkgrndTrainPdfMain");
  histTrain.bkgrndEff = CloneObject<TH1>(dir, name + "_trainingEffB", "bkgrndTrainEffMain");

  // Validation
  histValid.signalPdf = CloneObject<TH1>(dir, name + "_S", "signalValidPdfMain");
  histValid.signalEff = CloneObject<TH1>(dir, name + "_effS", "signalValidEffMain");

  histValid.bkgrndPdf = CloneObject<TH1>(dir, name + "_B", "bkgrndValidPdfMain");
  histValid.bkgrndEff = CloneObject<TH1>(dir, name + "_effB", "bkgrndValidEffMain");
}

void MethodHistograms::ReplaceHistograms(const std::filesystem::path& path,
                                         const bool truncateBkgrnd) {
  TFile* file{TFile::Open(path.c_str())};
  if (file->IsZombie() || !file->IsOpen()) {
    LogError("Failed to open {}.", path.c_str());
    exit(1);
  }

  // Training
  ReplaceObject(histTrain.signalPdf, file, "signalTrainPdf");
  ReplaceObject(histTrain.signalEff, file, "signalTrainEff");

  ReplaceObject(histTrain.bkgrndPdf, file, "bkgrndTrainPdf");
  ReplaceObject(histTrain.bkgrndEff, file, truncateBkgrnd ? "bkgrndTrainEffT" : "bkgrndTrainEff");

  // Validation
  ReplaceObject(histValid.signalPdf, file, "signalValidPdf");
  ReplaceObject(histValid.signalEff, file, "signalValidEff");

  ReplaceObject(histValid.bkgrndPdf, file, "bkgrndValidPdf");
  ReplaceObject(histValid.bkgrndEff, file, truncateBkgrnd ? "bkgrndValidEffT" : "bkgrndValidEff");

  // Application
  if (histApply.CheckHealth()) {
    ReplaceObject(histApply.signalPdf, file, "signalApplyPdf");
    ReplaceObject(histApply.signalEff, file, "signalApplyEff");

    ReplaceObject(histApply.bkgrndPdf, file, "bkgrndApplyPdf");
    ReplaceObject(histApply.bkgrndEff, file, truncateBkgrnd ? "bkgrndApplyEffT" : "bkgrndApplyEff");
  } else {
    histApply.signalPdf = CloneObject<TH1>(file, "signalApplyPdf", "signalApplyPdfMain");
    histApply.signalEff = CloneObject<TH1>(file, "signalApplyEff", "signalApplyEffMain");

    histApply.bkgrndPdf = CloneObject<TH1>(file, "bkgrndApplyPdf", "bkgrndApplyPdfMain");
    histApply.bkgrndEff = CloneObject<TH1>(
        file, truncateBkgrnd ? "bkgrndApplyEffT" : "bkgrndApplyEff", "bkgrndApplyEffMain");
  }
}

void MethodHistograms::SetDatasetSplit(const DatasetSplit value) { dataset = value; }

CutInfo MethodHistograms::ProcessCut(const double cut, const double nTotalSignalEvents,
                                     const double nTotalBkgrndEvents) const {
  if (cut < CutMin() || cut > CutMax()) return {.cut = cut, .significance = 0};

  for (int bin{1}; bin < EffBinCount(); ++bin) {
    const double thisBinCenter{EffBinCenter(bin)};
    if (cut > thisBinCenter) continue;

    const double nextBinCenter{EffBinCenter(bin + 1)};
    const double t{(cut - thisBinCenter) / (nextBinCenter - thisBinCenter)};

    const double cutSignalEff{std::lerp(SignalEff(bin), SignalEff(bin + 1), t)};
    const double nSignalEvents{cutSignalEff * nTotalSignalEvents};

    const double cutBkgrndEff{std::lerp(BkgrndEff(bin), BkgrndEff(bin + 1), t)};
    const double nBkgrndEvents{cutBkgrndEff * nTotalBkgrndEvents};

    return {.cut = cut,
            .nSignalEvents = nSignalEvents,
            .nBkgrndEvents = nBkgrndEvents,
            .significance = Statistics::ExclusionSignificance(nSignalEvents, nBkgrndEvents)};
  }

  return {.cut = cut, .significance = 0};
}

CutInfo MethodHistograms::OptimalCut(const double nTotalSignalEvents,
                                     const double nTotalBkgrndEvents) {
  switch (dataset) {
    case DatasetSplit::Training:
      return histTrain.OptimalCut("trainSignificance", nTotalSignalEvents, nTotalBkgrndEvents);
    case HNL::ML::DatasetSplit::Validation:
      return histValid.OptimalCut("validSignificance", nTotalSignalEvents, nTotalBkgrndEvents);
    case DatasetSplit::Application:
      if (!histApply.CheckHealthEff()) {
        LogError("Application data is not available.");
        exit(1);
      }

      return histApply.OptimalCut("applySignificance", nTotalSignalEvents, nTotalBkgrndEvents);
  }

  LogError("How did we get here?");
  exit(1);
}

[[nodiscard]] TH1* MethodHistograms::Significance() const {
  TH1* significance{nullptr};
  switch (dataset) {
    case DatasetSplit::Training:
      significance = histTrain.significance;
      break;
    case HNL::ML::DatasetSplit::Validation:
      significance = histValid.significance;
      break;
    case DatasetSplit::Application:
      significance = histApply.significance;
      break;
  }

  if (significance != nullptr) return significance;

  LogError("Attempting to access significance without running OptimalCut()");
  exit(1);
}

void MethodHistograms::SavePNG(const std::filesystem::path& outPath, TH1* const extraSignalPDF,
                               TH1* const extraBkgrndPDF) const {
  // Initialize
  TCanvas canvas{"canvas", "canvas", 1920, 1080};
  canvas.Divide(2);

  histTrain.PreparePlot(kSolid);
  histValid.PreparePlot(kDashed, -4);

  // PDF
  canvas.cd(1)->SetLogy();

  THStack stackPDF{"stackPDF", "Probability Density Functions"};

  stackPDF.Add(histTrain.signalPdf);
  stackPDF.Add(histTrain.bkgrndPdf);

  stackPDF.Add(histValid.signalPdf);
  stackPDF.Add(histValid.bkgrndPdf);

  if (extraSignalPDF != nullptr) {
    extraSignalPDF->SetLineColor(kBlue - 7);
    extraSignalPDF->SetLineStyle(kDotted);
    extraSignalPDF->Draw("SAME HIST");
    stackPDF.Add(extraSignalPDF);
  }

  if (extraBkgrndPDF != nullptr) {
    extraBkgrndPDF->SetLineColor(kRed - 7);
    extraBkgrndPDF->SetLineStyle(kDotted);
    extraBkgrndPDF->Draw("SAME HIST");
    stackPDF.Add(extraBkgrndPDF);
  }

  stackPDF.Draw("NOSTACK HIST");

  // Efficiency
  canvas.cd(2);

  THStack stackEff{"stackCDF", "Efficiency"};

  stackEff.Add(histTrain.signalEff);
  stackEff.Add(histTrain.bkgrndEff);

  stackEff.Add(histValid.signalEff);
  stackEff.Add(histValid.bkgrndEff);

  stackEff.Draw("NOSTACK");

  // Save
  canvas.SaveAs(outPath.c_str());
}

void MethodHistograms::SaveCSV(const std::filesystem::path& outPath) const {
  std::ofstream stream{outPath};

  if (histApply.CheckHealthEff()) {
    std::print(stream,
               "bin,cut,"
               "signalTrainEff,bkgrndTrainEff,trainSignificance,"
               "signalValidEff,bkgrndValidEff,validSignificance,"
               "signalApplyEff,bkgrndApplyEff,applySignificance\n");

    for (int bin{1}; bin <= EffBinCount(); ++bin) {
      std::print(stream, "{},{},", bin, EffBinCenter(bin, true));
      std::print(stream, "{},{},{},", histTrain.signalEff->GetBinContent(bin),
                 histTrain.bkgrndEff->GetBinContent(bin),
                 histTrain.significance->GetBinContent(bin));
      std::print(stream, "{},{},{},", histValid.signalEff->GetBinContent(bin),
                 histValid.bkgrndEff->GetBinContent(bin),
                 histValid.significance->GetBinContent(bin));
      std::print(stream, "{},{},{}\n", histApply.signalEff->GetBinContent(bin),
                 histApply.bkgrndEff->GetBinContent(bin),
                 histApply.significance->GetBinContent(bin));
    }
  } else {
    std::print(stream,
               "bin,cut,"
               "signalTrainEff,bkgrndTrainEff,trainSignificance,"
               "signalValidEff,bkgrndValidEff,validSignificance\n");

    for (int bin{1}; bin <= EffBinCount(); ++bin) {
      std::print(stream, "{},{},", bin, EffBinCenter(bin, true));
      std::print(stream, "{},{},{},", histTrain.signalEff->GetBinContent(bin),
                 histTrain.bkgrndEff->GetBinContent(bin),
                 histTrain.significance->GetBinContent(bin));
      std::print(stream, "{},{},{}\n", histValid.signalEff->GetBinContent(bin),
                 histValid.bkgrndEff->GetBinContent(bin),
                 histValid.significance->GetBinContent(bin));
    }
  }
}

[[nodiscard]] const DatasetSplitHistograms* MethodHistograms::CurrentHistograms() const {
  switch (dataset) {
    case DatasetSplit::Training:
      return &histTrain;
    case DatasetSplit::Validation:
      return &histValid;
    case DatasetSplit::Application:
      if (!histApply.CheckHealthEff()) {
        LogError("Application data is not available.");
        exit(1);
      }

      return &histApply;
  }

  LogError("How did we get here?");
  exit(1);
};
}  // namespace HNL::ML
