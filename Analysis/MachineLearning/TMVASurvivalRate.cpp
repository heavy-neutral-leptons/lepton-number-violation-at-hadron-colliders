// Copyright 2023-2026 Bruno M. S. Oliveira

// Standard Library
#include <filesystem>
#include <string>

// ROOT
#include "TCanvas.h"
#include "THStack.h"

// HNL
#include "Common/Log.hpp"
#include "MachineLearning/DatasetSplit.hpp"
#include "MachineLearning/TMVASurvivalRate.hpp"

namespace HNL::ML {
TMVASurvivalRate::TMVASurvivalRate(std::filesystem::path& datasetPath,
                                   const std::vector<Common::Observable>* const observables)
    : TMVAReaderWrapper{datasetPath, observables} {
  datasetPath.concat("-SurvivalRate.root");
  outputFile = TFile::Open(datasetPath.c_str(), "CREATE");
  if (outputFile->IsZombie() || !outputFile->IsOpen()) {
    LogError("Failed to open {}.", datasetPath.c_str());
    exit(1);
  }

  pngOutputPath = datasetPath.replace_extension("png");
}

TMVASurvivalRate::~TMVASurvivalRate() {
  // Signal
  if (totalSignalIntegral != 0) {
    signalTrainPdf->Scale(1 / signalTrainPdf->Integral("WIDTH"));
    outputFile->WriteObject(signalTrainPdf, signalTrainPdf->GetName());
    signalTrainEff->Scale(1 / totalSignalIntegral);
    outputFile->WriteObject(signalTrainEff, signalTrainEff->GetName());

    signalValidPdf->Scale(1 / signalValidPdf->Integral("WIDTH"));
    outputFile->WriteObject(signalValidPdf, signalValidPdf->GetName());
    signalValidEff->Scale(1 / totalSignalIntegral);
    outputFile->WriteObject(signalValidEff, signalValidEff->GetName());

    signalApplyPdf->Scale(1 / signalApplyPdf->Integral("WIDTH"));
    outputFile->WriteObject(signalApplyPdf, signalApplyPdf->GetName());
    signalApplyEff->Scale(1 / totalSignalIntegral);
    outputFile->WriteObject(signalApplyEff, signalApplyEff->GetName());
  }

  // Background
  if (totalBkgrndIntegral != 0) {
    bkgrndTrainPdf->Scale(1 / bkgrndTrainPdf->Integral("WIDTH"));
    outputFile->WriteObject(bkgrndTrainPdf, bkgrndTrainPdf->GetName());
    bkgrndTrainEff->Scale(1 / totalBkgrndIntegral);
    outputFile->WriteObject(bkgrndTrainEff, bkgrndTrainEff->GetName());
    bkgrndTrainEffT->Scale(1 / totalBkgrndIntegral);
    outputFile->WriteObject(bkgrndTrainEffT, bkgrndTrainEffT->GetName());

    bkgrndValidPdf->Scale(1 / bkgrndValidPdf->Integral("WIDTH"));
    outputFile->WriteObject(bkgrndValidPdf, bkgrndValidPdf->GetName());
    bkgrndValidEff->Scale(1 / totalBkgrndIntegral);
    outputFile->WriteObject(bkgrndValidEff, bkgrndValidEff->GetName());
    bkgrndValidEffT->Scale(1 / totalBkgrndIntegral);
    outputFile->WriteObject(bkgrndValidEffT, bkgrndValidEffT->GetName());

    bkgrndApplyPdf->Scale(1 / bkgrndApplyPdf->Integral("WIDTH"));
    outputFile->WriteObject(bkgrndApplyPdf, bkgrndApplyPdf->GetName());
    bkgrndApplyEff->Scale(1 / totalBkgrndIntegral);
    outputFile->WriteObject(bkgrndApplyEff, bkgrndApplyEff->GetName());
    bkgrndApplyEffT->Scale(1 / totalBkgrndIntegral);
    outputFile->WriteObject(bkgrndApplyEffT, bkgrndApplyEffT->GetName());
  }

  // Draw plots
  SavePNG();

  // Clean-up
  outputFile->Save();
  outputFile->Close();
}

void TMVASurvivalRate::SavePNG() {
  /* ----- Initialize ----- */
  TCanvas canvas{"canvas", "canvas", 1500, 1000};
  canvas.Divide(3, 2);

  /* ----- Training Data ----- */
  Method().SetDatasetSplit(DatasetSplit::Training);

  // PDFs
  canvas.cd(1)->SetLogy();
  THStack trainPdf{"trainPdf", "Training Probability Density Functions"};

  Method().SignalPdf()->SetLineColor(kBlue);
  trainPdf.Add(Method().SignalPdf());

  Method().BkgrndPdf()->SetLineColor(kRed);
  trainPdf.Add(Method().BkgrndPdf());

  signalTrainPdf->SetLineColorAlpha(kMagenta, 0.3);
  trainPdf.Add(signalTrainPdf);

  bkgrndTrainPdf->SetLineColorAlpha(kCyan, 0.3);
  trainPdf.Add(bkgrndTrainPdf);

  trainPdf.Draw("HIST NOSTACK");

  // Efficiencies
  canvas.cd(4)->SetLogy();
  THStack trainEff{"trainEff", "Training Efficiencies"};

  Method().SignalEff()->SetLineColor(kBlue);
  trainEff.Add(Method().SignalEff());

  Method().BkgrndEff()->SetLineColor(kRed);
  trainEff.Add(Method().BkgrndEff());

  signalTrainEff->SetLineColorAlpha(kMagenta, 0.3);
  trainEff.Add(signalTrainEff);

  bkgrndTrainEff->SetLineColorAlpha(kCyan, 0.3);
  trainEff.Add(bkgrndTrainEff);

  trainEff.Draw("HIST NOSTACK");

  /* ----- Validation Data ----- */
  Method().SetDatasetSplit(DatasetSplit::Validation);

  // PDFs
  canvas.cd(2)->SetLogy();
  THStack validPdf{"validPdf", "Validation Probability Density Functions"};

  Method().SignalPdf()->SetLineColor(kBlue);
  validPdf.Add(Method().SignalPdf());

  Method().BkgrndPdf()->SetLineColor(kRed);
  validPdf.Add(Method().BkgrndPdf());

  signalValidPdf->SetLineColorAlpha(kMagenta, 0.3);
  validPdf.Add(signalValidPdf);

  bkgrndValidPdf->SetLineColorAlpha(kCyan, 0.3);
  validPdf.Add(bkgrndValidPdf);

  validPdf.Draw("HIST NOSTACK");

  // Efficiencies
  canvas.cd(5)->SetLogy();
  THStack validEff{"validEff", "Validation Efficiencies"};

  Method().SignalEff()->SetLineColor(kBlue);
  validEff.Add(Method().SignalEff());

  Method().BkgrndEff()->SetLineColor(kRed);
  validEff.Add(Method().BkgrndEff());

  signalValidEff->SetLineColorAlpha(kMagenta, 0.3);
  validEff.Add(signalValidEff);

  bkgrndValidEff->SetLineColorAlpha(kCyan, 0.3);
  validEff.Add(bkgrndValidEff);

  validEff.Draw("HIST NOSTACK");

  /* ----- Application Data ----- */
  Method().SetDatasetSplit(DatasetSplit::Application);

  // PDFs
  canvas.cd(3)->SetLogy();
  THStack applyPdf{"applyPdf", "Application Probability Density Functions"};

  signalApplyPdf->SetLineColorAlpha(kMagenta, 0.3);
  applyPdf.Add(signalApplyPdf);

  bkgrndApplyPdf->SetLineColorAlpha(kCyan, 0.3);
  applyPdf.Add(bkgrndApplyPdf);

  applyPdf.Draw("HIST NOSTACK");

  // Efficiencies
  canvas.cd(6)->SetLogy();
  THStack applyEff{"applyEff", "Application Efficiencies"};

  signalApplyEff->SetLineColorAlpha(kMagenta, 0.3);
  applyEff.Add(signalApplyEff);

  bkgrndApplyEff->SetLineColorAlpha(kCyan, 0.3);
  applyEff.Add(bkgrndApplyEff);

  applyEff.Draw("HIST NOSTACK");

  /* ----- Save ----- */
  canvas.SaveAs(pngOutputPath.c_str());
}

void TMVASurvivalRate::ProcessTree(Reconstruct::ReconstructedEventData& events,
                                   const bool isSignal) {
  const std::string sourceName{events.Name()};

  int64_t nTrainEvents{0};
  std::string name{std::format("{}-trainPdf", sourceName)};
  TH1D* const trainPdf{new TH1D{name.c_str(), name.c_str(), nBinsPdf, min, max}};
  name = std::format("{}-trainEff", sourceName);
  TH1D* const trainEff{new TH1D{name.c_str(), name.c_str(), nBinsEff, min, max}};
  name = std::format("{}-trainEffT", sourceName);
  TH1D* const trainEffT{new TH1D{name.c_str(), name.c_str(), nBinsEff, min, max}};

  int64_t nValidEvents{0};
  name = std::format("{}-validPdf", sourceName);
  TH1D* const validPdf{new TH1D{name.c_str(), name.c_str(), nBinsPdf, min, max}};
  name = std::format("{}-validEff", sourceName);
  TH1D* const validEff{new TH1D{name.c_str(), name.c_str(), nBinsEff, min, max}};
  name = std::format("{}-validEffT", sourceName);
  TH1D* const validEffT{new TH1D{name.c_str(), name.c_str(), nBinsEff, min, max}};

  int64_t nApplyEvents{0};
  name = std::format("{}-applyPdf", sourceName);
  TH1D* const applyPdf{new TH1D{name.c_str(), name.c_str(), nBinsPdf, min, max}};
  name = std::format("{}-applyEff", sourceName);
  TH1D* const applyEff{new TH1D{name.c_str(), name.c_str(), nBinsEff, min, max}};
  name = std::format("{}-applyEffT", sourceName);
  TH1D* const applyEffT{new TH1D{name.c_str(), name.c_str(), nBinsEff, min, max}};

  SetTreeAddresses(events.Tree());

  const int64_t nSimulatedEvents{events.GetEntries()};
  for (int64_t idx{0}; idx < nSimulatedEvents; ++idx) {
    GetTreeEntry(events.Tree(), idx);

    switch (AssignDatasetSplit(idx)) {
      case DatasetSplit::Training:
        ++nTrainEvents;
        trainPdf->Fill(EvaluateMVA());
        trainEff->Fill(EvaluateMVA());
        break;

      case DatasetSplit::Validation:
        ++nValidEvents;
        validPdf->Fill(EvaluateMVA());
        validEff->Fill(EvaluateMVA());
        break;

      case DatasetSplit::Application:
        ++nApplyEvents;
        applyPdf->Fill(EvaluateMVA());
        applyEff->Fill(EvaluateMVA());
        break;
    }
  }

  double acc{0};
  for (int bin{nBinsEff - 1}; bin > 0; --bin) {
    acc = trainEff->GetBinContent(bin) + trainEff->GetBinContent(bin + 1);
    trainEff->SetBinContent(bin, acc);
    trainEffT->SetBinContent(bin, std::max(acc, 1.));

    acc = validEff->GetBinContent(bin) + validEff->GetBinContent(bin + 1);
    validEff->SetBinContent(bin, acc);
    validEffT->SetBinContent(bin, std::max(acc, 1.));

    acc = applyEff->GetBinContent(bin) + applyEff->GetBinContent(bin + 1);
    applyEff->SetBinContent(bin, acc);
    applyEffT->SetBinContent(bin, std::max(acc, 1.));
  }

  // Write probability density functions
  trainPdf->Scale(1 / trainPdf->Integral("WIDTH"));
  outputFile->WriteObject(trainPdf, trainPdf->GetName());

  validPdf->Scale(1 / validPdf->Integral("WIDTH"));
  outputFile->WriteObject(validPdf, validPdf->GetName());

  applyPdf->Scale(1 / applyPdf->Integral("WIDTH"));
  outputFile->WriteObject(applyPdf, applyPdf->GetName());

  // Write efficiencies
  trainEff->Scale(1 / static_cast<double>(nTrainEvents));
  outputFile->WriteObject(trainEff, trainEff->GetName());

  validEff->Scale(1 / static_cast<double>(nValidEvents));
  outputFile->WriteObject(validEff, validEff->GetName());

  applyEff->Scale(1 / static_cast<double>(nApplyEvents));
  outputFile->WriteObject(applyEff, applyEff->GetName());

  if (!isSignal) {
    trainEffT->Scale(1 / static_cast<double>(nTrainEvents));
    outputFile->WriteObject(trainEffT, trainEffT->GetName());

    validEffT->Scale(1 / static_cast<double>(nValidEvents));
    outputFile->WriteObject(validEffT, validEffT->GetName());

    applyEffT->Scale(1 / static_cast<double>(nApplyEvents));
    outputFile->WriteObject(applyEffT, applyEffT->GetName());
  }

  // Add to signal/background
  if (isSignal) {
    totalSignalIntegral += events.EffectiveCrossSection();

    signalTrainPdf->Add(trainPdf, events.EffectiveCrossSection());
    signalTrainEff->Add(trainEff, events.EffectiveCrossSection());

    signalValidPdf->Add(validPdf, events.EffectiveCrossSection());
    signalValidEff->Add(validEff, events.EffectiveCrossSection());

    signalApplyPdf->Add(applyPdf, events.EffectiveCrossSection());
    signalApplyEff->Add(applyEff, events.EffectiveCrossSection());
  } else {
    totalBkgrndIntegral += events.EffectiveCrossSection();

    bkgrndTrainPdf->Add(trainPdf, events.EffectiveCrossSection());
    bkgrndTrainEff->Add(trainEff, events.EffectiveCrossSection());
    bkgrndTrainEffT->Add(trainEffT, events.EffectiveCrossSection());

    bkgrndValidPdf->Add(validPdf, events.EffectiveCrossSection());
    bkgrndValidEff->Add(validEff, events.EffectiveCrossSection());
    bkgrndValidEffT->Add(validEffT, events.EffectiveCrossSection());

    bkgrndApplyPdf->Add(applyPdf, events.EffectiveCrossSection());
    bkgrndApplyEff->Add(applyEff, events.EffectiveCrossSection());
    bkgrndApplyEffT->Add(applyEffT, events.EffectiveCrossSection());
  }
}
}  // namespace HNL::ML
