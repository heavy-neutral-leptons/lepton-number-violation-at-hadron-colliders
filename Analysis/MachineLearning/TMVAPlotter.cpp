// Copyright 2023-2026 Bruno M. S. Oliveira

// Standard Library
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

// ROOT
#include "ROOT/RDataFrame.hxx"
#include "TCanvas.h"
#include "TGaxis.h"
#include "TMVA/DataLoader.h"
#include "TPaveText.h"
#include "TTree.h"

// HNL
#include "Common/Log.hpp"
#include "MachineLearning/DatasetSplit.hpp"
#include "MachineLearning/TMVAPlotter.hpp"
#include "Statistics/Significance.hpp"

namespace HNL::ML {
using Statistics::ExclusionSignificance;

void TMVAPlotter::ProcessSignalTree(Reconstruct::ReconstructedEventData& events) {
  sourceNames.emplace_back(events.Name());

  const double weight{events.NumberOfEvents(signalLuminosity)};
  if (nTotalSignalEvents == 0) nTotalSignalEvents = weight;

  static int16_t color{kAzure};
  ProcessTree(events.Tree(), weight, color++);
}

void TMVAPlotter::ProcessBkgrndTree(Reconstruct::ReconstructedEventData& events) {
  sourceNames.emplace_back(events.Name());

  const double weight{events.NumberOfEvents(bkgrndLuminosity)};
  nBkgrndEvents.emplace_back(weight);
  nTotalBkgrndEvents += weight;

  static int16_t color{kOrange};
  ProcessTree(events.Tree(), weight, color++);

  const int idx{trainStack.GetNhists() - 1};
  trainBkgrnd.Add(dynamic_cast<TH1D*>(trainStack.GetHists()->At(idx)), weight);
  validBkgrnd.Add(dynamic_cast<TH1D*>(validStack.GetHists()->At(idx)), weight);
  applyBkgrnd.Add(dynamic_cast<TH1D*>(applyStack.GetHists()->At(idx)), weight);
}

void TMVAPlotter::ProcessTree(TTree* const tree, const double weight, const int16_t lineColor) {
  std::string name{std::format("{} (Training)", sourceNames.back())};
  TH1D* const trainHist{new TH1D{name.c_str(), name.c_str(), bins, min, max}};
  trainHist->SetDirectory(nullptr);
  trainHist->SetLineColor(lineColor);
  trainStack.Add(trainHist);

  name = std::format("{} (Validation)", sourceNames.back());
  TH1D* const validHist{new TH1D{name.c_str(), name.c_str(), bins, min, max}};
  validHist->SetDirectory(nullptr);
  validHist->SetLineColor(lineColor);
  validStack.Add(validHist);

  name = std::format("{} (Application)", sourceNames.back());
  TH1D* const applyHist{new TH1D{name.c_str(), name.c_str(), bins, min, max}};
  applyHist->SetDirectory(nullptr);
  applyHist->SetLineColor(lineColor);
  applyStack.Add(applyHist);

  SetTreeAddresses(tree);

  const int64_t nSimulatedEvents{tree->GetEntries()};
  for (int64_t idx{0}; idx < nSimulatedEvents; ++idx) {
    GetTreeEntry(tree, idx);

    switch (AssignDatasetSplit(idx)) {
      case DatasetSplit::Training:
        trainHist->Fill(EvaluateMVA());
        break;
      case HNL::ML::DatasetSplit::Validation:
        validHist->Fill(EvaluateMVA());
        break;
      case DatasetSplit::Application:
        applyHist->Fill(EvaluateMVA());
        break;
    }
  }

  trainHist->Scale(weight / trainHist->Integral());
  validHist->Scale(weight / validHist->Integral());
  applyHist->Scale(weight / applyHist->Integral());

  sourcesLegend.AddEntry(trainHist, sourceNames.back().c_str());
}

double TMVAPlotter::SurvivingEvents(const int idx, const double cut, const DatasetSplit split) {
  if (idx < 0 || idx >= trainStack.GetNhists()) {
    LogError("Histogram {} outside range from {} to {}", idx, 0, trainStack.GetNhists());
    exit(1);
  }

  const int idxFirstBkgrnd{FirstBackgroundIndex()};
  if (idx > 0 && idx < idxFirstBkgrnd) {
    LogError("Histogram {} is neither main signal nor background from ({})", idx, idxFirstBkgrnd);
    exit(1);
  }

  const double weight{idx == 0 ? nTotalSignalEvents : nBkgrndEvents[idx - idxFirstBkgrnd]};

  TObject* histObj{nullptr};
  switch (split) {
    case DatasetSplit::Training:
      histObj = trainStack.GetHists()->At(idx);
      break;
    case DatasetSplit::Validation:
      histObj = validStack.GetHists()->At(idx);
      break;
    case DatasetSplit::Application:
      histObj = applyStack.GetHists()->At(idx);
      break;
  }

  TH1* hist{dynamic_cast<TH1*>(histObj)};
  const int bin{hist->FindBin(cut)};
  const double integral{hist->Integral(bin, hist->GetNbinsX(), "WIDTH")};
  const double integralFull{hist->Integral("WIDTH")};

  Log<1>("{} ({:.0f}/{:.0f}):\n{} is in bin {} ({} - {})\n{}/{} -> {:.0f}\n", sourceNames.at(idx),
         weight, nTotalBkgrndEvents, cut, bin, BinMin(bin), BinMax(bin), integral, integralFull,
         integral * weight);

  return weight * integral;
}

void TMVAPlotter::SavePNG(const std::filesystem::path& outPath) {
  /* ----- Initialize ----- */
  TCanvas canvas{"canvas", "canvas", 1920, 1080};
  canvas.Divide(4, 2);

  /* ----- Plot probability density functions ----- */
  // Train
  canvas.cd(1)->SetLogy();
  Method().SetDatasetSplit(DatasetSplit::Training);

  trainStack.Draw("NOSTACK");

  Method().SignalPdf()->SetLineColor(kBlue + 3);
  Method().SignalPdf()->Draw("HIST SAME");

  Method().BkgrndPdf()->SetLineColor(kRed + 3);
  Method().BkgrndPdf()->Draw("HIST SAME");

  // Validation
  canvas.cd(2)->SetLogy();
  Method().SetDatasetSplit(DatasetSplit::Validation);

  validStack.Draw("NOSTACK");

  Method().SignalPdf()->SetLineColor(kBlue + 3);
  Method().SignalPdf()->Draw("HIST SAME");

  Method().BkgrndPdf()->SetLineColor(kRed + 3);
  Method().BkgrndPdf()->Draw("HIST SAME");

  // Application
  canvas.cd(3)->SetLogy();
  Method().SetDatasetSplit(DatasetSplit::Application);

  applyStack.Draw("NOSTACK");

  // MainMethod().SignalPdf()->SetLineColor(kBlue + 3);
  // MainMethod().SignalPdf()->Draw("HIST SAME");
  //
  // MainMethod().BkgrndPdf()->SetLineColor(kRed + 3);
  // MainMethod().BkgrndPdf()->Draw("HIST SAME");

  // Ensure same axis
  double ymin{std::min(trainStack.GetMinimum(), validStack.GetMinimum())};
  ymin = std::min(ymin, applyStack.GetMinimum());

  double ymax{std::max(trainStack.GetMaximum(), validStack.GetMaximum())};
  ymax = std::max(ymax, applyStack.GetMaximum());

  trainStack.SetMinimum(ymin);
  trainStack.SetMaximum(ymax);

  validStack.SetMinimum(ymin);
  validStack.SetMaximum(ymax);

  applyStack.SetMinimum(ymin);
  applyStack.SetMaximum(ymax);

  // Legend
  canvas.cd(4);
  sourcesLegend.Draw();

  /* ----- Compute significances ----- */
  // Training
  Method().SetDatasetSplit(DatasetSplit::Training);
  const HNL::ML::CutInfo trainCut{OptimalCut()};
  TH1* const trainSig{Method().SignificanceClone()};

  const double train2trainSignal{SurvivingEvents(0, trainCut.cut, DatasetSplit::Training)};
  const double train2validSignal{SurvivingEvents(0, trainCut.cut, DatasetSplit::Validation)};
  const double train2applySignal{SurvivingEvents(0, trainCut.cut, DatasetSplit::Application)};

  double train2trainBkgrnd{0};
  double train2validBkgrnd{0};
  double train2applyBkgrnd{0};

  for (int idx{FirstBackgroundIndex()}; idx < trainStack.GetNhists(); ++idx) {
    train2trainBkgrnd += SurvivingEvents(idx, trainCut.cut, DatasetSplit::Training);
    train2validBkgrnd += SurvivingEvents(idx, trainCut.cut, DatasetSplit::Validation);
    train2applyBkgrnd += SurvivingEvents(idx, trainCut.cut, DatasetSplit::Application);
  }

  const double train2trainZ{ExclusionSignificance(train2trainSignal, train2trainBkgrnd)};
  const double train2validZ{ExclusionSignificance(train2validSignal, train2validBkgrnd)};
  const double train2applyZ{ExclusionSignificance(train2applySignal, train2applyBkgrnd)};

  // Validation
  Method().SetDatasetSplit(DatasetSplit::Validation);
  const HNL::ML::CutInfo validCut{OptimalCut()};
  TH1* const validSig{Method().SignificanceClone()};

  const double valid2trainSignal{SurvivingEvents(0, validCut.cut, DatasetSplit::Training)};
  const double valid2validSignal{SurvivingEvents(0, validCut.cut, DatasetSplit::Validation)};
  const double valid2applySignal{SurvivingEvents(0, validCut.cut, DatasetSplit::Application)};

  double valid2trainBkgrnd{0};
  double valid2validBkgrnd{0};
  double valid2applyBkgrnd{0};

  for (int idx{FirstBackgroundIndex()}; idx < trainStack.GetNhists(); ++idx) {
    valid2trainBkgrnd += SurvivingEvents(idx, validCut.cut, DatasetSplit::Training);
    valid2validBkgrnd += SurvivingEvents(idx, validCut.cut, DatasetSplit::Validation);
    valid2applyBkgrnd += SurvivingEvents(idx, validCut.cut, DatasetSplit::Application);
  }

  const double valid2trainZ{ExclusionSignificance(valid2trainSignal, valid2trainBkgrnd)};
  const double valid2validZ{ExclusionSignificance(valid2validSignal, valid2validBkgrnd)};
  const double valid2applyZ{ExclusionSignificance(valid2applySignal, valid2applyBkgrnd)};

  // Application
  Method().SetDatasetSplit(DatasetSplit::Application);
  const HNL::ML::CutInfo applyCut{OptimalCut()};
  TH1* const applySig{Method().SignificanceClone()};

  const double apply2trainSignal{SurvivingEvents(0, applyCut.cut, DatasetSplit::Training)};
  const double apply2validSignal{SurvivingEvents(0, applyCut.cut, DatasetSplit::Validation)};
  const double apply2applySignal{SurvivingEvents(0, applyCut.cut, DatasetSplit::Application)};

  double apply2trainBkgrnd{0};
  double apply2validBkgrnd{0};
  double apply2applyBkgrnd{0};

  for (int idx{FirstBackgroundIndex()}; idx < trainStack.GetNhists(); ++idx) {
    apply2trainBkgrnd += SurvivingEvents(idx, applyCut.cut, DatasetSplit::Training);
    apply2validBkgrnd += SurvivingEvents(idx, applyCut.cut, DatasetSplit::Validation);
    apply2applyBkgrnd += SurvivingEvents(idx, applyCut.cut, DatasetSplit::Application);
  }

  const double apply2trainZ{ExclusionSignificance(apply2trainSignal, apply2trainBkgrnd)};
  const double apply2validZ{ExclusionSignificance(apply2validSignal, apply2validBkgrnd)};
  const double apply2applyZ{ExclusionSignificance(apply2applySignal, apply2applyBkgrnd)};

  /* ----- Plot efficiencies and significances ----- */
  // Train
  canvas.cd(5)->SetLogy();
  Method().SetDatasetSplit(DatasetSplit::Training);

  for (TObject* const obj : trainStack) {
    TH1* const hist{dynamic_cast<TH1*>(obj)};
    hist->Integral(hist->FindBin(trainCut.cut), hist->GetNbinsX(), "WIDTH");
  }

  THStack trainEffStack{"trainEffStack", "Training Data Efficiency"};
  TLegend trainEffLegend{};

  Method().SignalEff()->SetLineColor(kBlue);
  trainEffStack.Add(Method().SignalEff());
  trainEffLegend.AddEntry(Method().SignalEff(), "Signal (Training)");

  TH1* const signalTrainEff{dynamic_cast<TH1*>(trainStack.GetHists()->At(0))->GetCumulative(false)};
  signalTrainEff->Scale(1 / signalTrainEff->GetMaximum());
  signalTrainEff->SetLineColor(kBlue - 9);
  trainEffStack.Add(signalTrainEff);
  trainEffLegend.AddEntry(signalTrainEff, "Signal (Training*)");

  Method().BkgrndEff()->SetLineColor(kRed);
  trainEffStack.Add(Method().BkgrndEff());
  trainEffLegend.AddEntry(Method().BkgrndEff(), "Background (Training)");

  trainBkgrnd.Scale(1 / trainBkgrnd.Integral("WIDTH"));
  TH1* const bkgrndTrainEff{trainBkgrnd.GetCumulative(false)};
  bkgrndTrainEff->Scale(1 / bkgrndTrainEff->GetMaximum());
  bkgrndTrainEff->SetLineColor(kRed - 9);
  trainEffStack.Add(bkgrndTrainEff);
  trainEffLegend.AddEntry(bkgrndTrainEff, "Background (Training*)");

  trainEffStack.Draw("NOSTACK");

  gPad->Update();

  const double trainSigMin{0};
  trainSig->SetMinimum(trainSigMin);

  const double trainSigMax{trainSig->GetMaximum()};
  trainSig->Scale(gPad->GetUymax() / trainSigMax);

  trainSig->SetLineColor(kGreen);
  trainSig->Draw("HIST SAME");

  trainEffLegend.Draw();

  TGaxis* trainSigAxis{new TGaxis(gPad->GetUxmax(), gPad->GetUymin(), gPad->GetUxmax(),
                                  gPad->GetUymax(), trainSigMin, trainSigMax, 510, "+L")};
  trainSigAxis->SetLabelColor(kGreen);
  trainSigAxis->Draw();

  // Validation
  canvas.cd(6)->SetLogy();
  Method().SetDatasetSplit(DatasetSplit::Validation);

  THStack validEffStack{"validEffStack", "Validation Data Efficiency"};
  TLegend validEffLegend{};

  Method().SignalEff()->SetLineColor(kBlue);
  validEffStack.Add(Method().SignalEff());
  validEffLegend.AddEntry(Method().SignalEff(), "Signal (Validation)");

  TH1* const signalValidEff{dynamic_cast<TH1*>(validStack.GetHists()->At(0))->GetCumulative(false)};
  signalValidEff->Scale(1 / signalValidEff->GetMaximum());
  signalValidEff->SetLineColor(kBlue - 9);
  validEffStack.Add(signalValidEff);
  validEffLegend.AddEntry(signalValidEff, "Signal (Validation*)");

  Method().BkgrndEff()->SetLineColor(kRed);
  validEffStack.Add(Method().BkgrndEff());
  validEffLegend.AddEntry(Method().BkgrndEff(), "Background (Validation)");

  validBkgrnd.Scale(1 / validBkgrnd.Integral("WIDTH"));
  TH1* const bkgrndValidEff{validBkgrnd.GetCumulative(false)};
  bkgrndValidEff->Scale(1 / bkgrndValidEff->GetMaximum());
  bkgrndValidEff->SetLineColor(kRed - 9);
  validEffStack.Add(bkgrndValidEff);
  validEffLegend.AddEntry(bkgrndValidEff, "Background (Validation*)");

  validEffStack.Draw("NOSTACK");

  gPad->Update();

  const double validSigMin{0};
  validSig->SetMinimum(validSigMin);

  const double validSigMax{validSig->GetMaximum()};
  validSig->Scale(gPad->GetUymax() / validSigMax);

  validSig->SetLineColor(kGreen);
  validSig->Draw("HIST SAME");

  validEffLegend.Draw();

  TGaxis* validSigAxis{new TGaxis(gPad->GetUxmax(), gPad->GetUymin(), gPad->GetUxmax(),
                                  gPad->GetUymax(), validSigMin, validSigMax, 510, "+L")};
  validSigAxis->SetLabelColor(kGreen);
  validSigAxis->Draw();

  // Application
  canvas.cd(7)->SetLogy();
  Method().SetDatasetSplit(DatasetSplit::Application);

  THStack applyEffStack{"applyEffStack", "Application Data Efficiency"};
  TLegend applyEffLegend{};

  Method().SignalEff()->SetLineColor(kBlue);
  applyEffStack.Add(Method().SignalEff());
  applyEffLegend.AddEntry(Method().SignalEff(), "Signal (Application)");

  TH1* const signalApplyEff{dynamic_cast<TH1*>(applyStack.GetHists()->At(0))->GetCumulative(false)};
  signalApplyEff->Scale(1 / signalApplyEff->GetMaximum());
  signalApplyEff->SetLineColor(kBlue - 9);
  applyEffStack.Add(signalApplyEff);
  applyEffLegend.AddEntry(signalApplyEff, "Signal (Application*)");

  Method().BkgrndEff()->SetLineColor(kRed);
  applyEffStack.Add(Method().BkgrndEff());
  applyEffLegend.AddEntry(Method().BkgrndEff(), "Background (Application)");

  applyBkgrnd.Scale(1 / applyBkgrnd.Integral("WIDTH"));
  TH1* const bkgrndApplyEff{applyBkgrnd.GetCumulative(false)};
  bkgrndApplyEff->Scale(1 / bkgrndApplyEff->GetMaximum());
  bkgrndApplyEff->SetLineColor(kRed - 9);
  applyEffStack.Add(bkgrndApplyEff);
  applyEffLegend.AddEntry(bkgrndApplyEff, "Background (Application*)");

  applyEffStack.Draw("NOSTACK");

  gPad->Update();

  const double applySigMin{0};
  applySig->SetMinimum(applySigMin);

  const double applySigMax{applySig->GetMaximum()};
  applySig->Scale(gPad->GetUymax() / applySigMax);

  applySig->SetLineColor(kGreen);
  applySig->Draw("HIST SAME");

  applyEffLegend.Draw();

  TGaxis* applySigAxis{new TGaxis(gPad->GetUxmax(), gPad->GetUymin(), gPad->GetUxmax(),
                                  gPad->GetUymax(), applySigMin, applySigMax, 510, "+L")};
  applySigAxis->SetLabelColor(kGreen);
  applySigAxis->Draw();

  /* ----- Summary ----- */
  canvas.cd(8);

  TPaveText summary{0.0, 0.0, 1.0, 1.0};
  summary.SetTextAlign(12);
  summary.SetTextSize(0.06);

  summary.AddText(
      std::format("Signal: {:.2e} ({:.2e} events)", signalLuminosity, nTotalSignalEvents).data());
  summary.AddText(
      std::format("Bkgrnd: {:.2e} ({:.2e} events)", bkgrndLuminosity, nTotalBkgrndEvents).data());

  summary.AddText(std::format("Training cut: {:.2f}", trainCut.cut).data());
  summary.AddText(std::format("Training: ({:.2e} | {:.2e}) -> {:.2f} [{:.2f}]",  //
                              train2trainSignal, train2trainBkgrnd, train2trainZ,
                              trainCut.significance)
                      .data());
  summary.AddText(std::format("Validation: ({:.2e} | {:.2e}) -> {:.2f} [{:.2f}]",  //
                              train2validSignal, train2validBkgrnd, train2validZ,
                              validSig->GetBinContent(validSig->FindBin(trainCut.cut)))
                      .data());
  summary.AddText(std::format("Application: ({:.2e} | {:.2e}) -> {:.2f} [{:.2f}]",  //
                              train2applySignal, train2applyBkgrnd, train2applyZ,
                              applySig->GetBinContent(applySig->FindBin(trainCut.cut)))
                      .data());

  summary.AddText(std::format("Validation cut: {:.2f}", validCut.cut).data());
  summary.AddText(std::format("Training: ({:.2e} | {:.2e}) -> {:.2f} [{:.2f}]",  //
                              valid2trainSignal, valid2trainBkgrnd, valid2trainZ,
                              trainSig->GetBinContent(trainSig->FindBin(validCut.cut)))
                      .data());
  summary.AddText(std::format("Validation: ({:.2e} | {:.2e}) -> {:.2f} [{:.2f}]",  //
                              valid2validSignal, valid2validBkgrnd, valid2validZ,
                              validCut.significance)
                      .data());
  summary.AddText(std::format("Application: ({:.2e} | {:.2e}) -> {:.2f} [{:.2f}]",  //
                              valid2applySignal, valid2applyBkgrnd, valid2applyZ,
                              applySig->GetBinContent(applySig->FindBin(validCut.cut)))
                      .data());

  summary.AddText(std::format("Application cut: {:.2f}", validCut.cut).data());
  summary.AddText(std::format("Training: ({:.2e} | {:.2e}) -> {:.2f} [{:.2f}]",  //
                              apply2trainSignal, apply2trainBkgrnd, apply2trainZ,
                              trainSig->GetBinContent(trainSig->FindBin(applyCut.cut)))
                      .data());
  summary.AddText(std::format("Validation: ({:.2e} | {:.2e}) -> {:.2f} [{:.2f}]",  //
                              apply2validSignal, apply2validBkgrnd, apply2validZ,
                              applySig->GetBinContent(applySig->FindBin(applyCut.cut)))
                      .data());
  summary.AddText(std::format("Application: ({:.2e} | {:.2e}) -> {:.2f} [{:.2f}]",  //
                              apply2applySignal, apply2applyBkgrnd, apply2applyZ,
                              applyCut.significance)
                      .data());

  summary.Draw();

  /* ----- Save ----- */
  canvas.SaveAs(outPath.c_str());
}

void TMVAPlotter::SaveCSV(const std::filesystem::path& outPath) const {
  std::ofstream stream{outPath};

  std::print(stream, "bin,cut");
  for (const std::string& sourceName : sourceNames) {
    std::print(stream, ",{}-train,{}-valid,{}-apply", sourceName, sourceName, sourceName);
  }
  std::print(stream, "\n");

  ZeroBinToCSV(stream);
  for (int bin{1}; bin <= NBins(); ++bin) BinToCSV(stream, bin);
  LastBinToCSV(stream);
}

void TMVAPlotter::BinToCSV(std::ofstream& stream, const int bin) const {
  std::print(stream, "{},{}", bin, BinCenter(bin));

  for (int idxHist{0}; idxHist < trainStack.GetNhists(); ++idxHist) {
    const TH1D* const trainHist{dynamic_cast<TH1D*>(trainStack.GetHists()->At(idxHist))};
    const double trainBinContent{trainHist->GetBinContent(bin)};

    const TH1D* const validHist{dynamic_cast<TH1D*>(validStack.GetHists()->At(idxHist))};
    const double validBinContent{validHist->GetBinContent(bin)};

    const TH1D* const applyHist{dynamic_cast<TH1D*>(applyStack.GetHists()->At(idxHist))};
    const double applyBinContent{applyHist->GetBinContent(bin)};

    std::print(stream, ",{},{},{}", trainBinContent, validBinContent, applyBinContent);
  }

  std::print(stream, "\n");
}

void TMVAPlotter::ZeroBinToCSV(std::ofstream& stream) const {
  std::print(stream, "{},{}", 0, BinCenter(1) - BinWidth(1));

  for (int idxHist{0}; idxHist < trainStack.GetNhists(); ++idxHist) {
    std::print(stream, ",{},{},{}", 0, 0, 0);
  }

  std::print(stream, "\n");
}

void TMVAPlotter::LastBinToCSV(std::ofstream& stream) const {
  std::print(stream, "{},{}", 0, BinCenter(bins) + BinWidth(bins));

  for (int idxHist{0}; idxHist < trainStack.GetNhists(); ++idxHist) {
    std::print(stream, ",{},{},{}", 0, 0, 0);
  }

  std::print(stream, "\n");
}
}  // namespace HNL::ML
