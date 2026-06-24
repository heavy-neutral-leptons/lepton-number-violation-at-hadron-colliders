// Copyright 2023-2026 Bruno M. S. Oliveira

// Standard Library
#include <filesystem>
#include <print>
#include <ranges>
#include <string>
#include <vector>

// CLI11
#include "CLI/CLI.hpp"

// ROOT
#include "TStyle.h"

// HNL
#include "Common/Log.hpp"
#include "MachineLearning/MethodHistograms.hpp"
#include "MachineLearning/Observables.hpp"
#include "MachineLearning/TMVAApplier.hpp"
#include "ReconstructedData/ReconstructedEventData.hpp"
#include "Statistics/Separation.hpp"

int main(const int argc, const char* const argv[]) {
  /* ----- CLI Arguments ----- */
  CLI::App app{};

  std::filesystem::path tmvaPath{};
  app.add_option("--dataset", tmvaPath, "Path to dataset")
      ->required()
      ->check(CLI::ExistingDirectory);

  std::filesystem::path signalPath{};
  app.add_option("--signal", signalPath, "Signal file")->required()->check(CLI::ExistingFile);

  std::vector<std::vector<std::filesystem::path>> bkgrndPaths{};
  app.add_option("--background", bkgrndPaths, "Background files")
      ->required()
      ->check(CLI::ExistingFile);

  std::optional<std::filesystem::path> pngPath{std::nullopt};
  app.add_option("--png", pngPath, "Output png path");

  try {
    app.parse(argc, argv);
  } catch (const CLI::ParseError& e) {
    return app.exit(e);
  }

  /* ----- Analysis ----- */
  const std::vector<HNL::Common::Observable>* const observables{HNL::ML::Observables(tmvaPath)};

  HNL::ML::TMVAApplier applier{tmvaPath, observables};
  if (!applier.Method().CheckHealth()) {
    LogError("Main method is invalid");
    return 1;
  }

  const int nBins{applier.Method().PdfBinCount()};
  const double min{applier.Method().CutMin()};
  const double max{applier.Method().CutMax()};

  std::vector<HNL::ML::CutInfo> cutInfoVector(nBins + 1, {.cut = min, .significance = 0});
  for (int bin{1}; bin <= nBins; ++bin) cutInfoVector[bin].cut = applier.Method().PdfBinMax(bin);

  // Signal
  HNL::Reconstruct::ReconstructedEventFile signal{signalPath};
  const std::vector<double> signalPdf{applier.ProcessTree(signal.Tree(), cutInfoVector)};

  // Background
  double bkgrndCrossSection{0};
  std::vector<double> bkgrndPdf(cutInfoVector.size(), 0);

  for (const std::vector<std::filesystem::path>& thisBkgrndPaths : bkgrndPaths) {
    HNL::Reconstruct::ReconstructedEventChain bkgrnd{thisBkgrndPaths};

    const double crossSection{bkgrnd.EffectiveCrossSection()};
    bkgrndCrossSection += crossSection;

    const std::vector<double> pdf{applier.ProcessTree(bkgrnd.Tree(), cutInfoVector, crossSection)};
    for (auto [total, current] : std::views::zip(bkgrndPdf, pdf)) total += current;
  }

  for (double& eff : bkgrndPdf) eff /= bkgrndCrossSection;

  /* ----- CLI Output ----- */
  std::print("{},", signal.Model()->CSV());

  // ROOT
  HNL::Statistics::DistComparison comparison{applier.SeparationTrain()};
  std::print("{},{},", comparison.separation1, comparison.separation2);

  comparison = applier.SeparationValid();
  std::print("{},{},", comparison.separation1, comparison.separation2);

  // TMVASurvivalRate
  std::filesystem::path survivalRatePath{tmvaPath};
  survivalRatePath.concat("-SurvivalRate.root");
  if (std::filesystem::exists(survivalRatePath)) {
    applier.Method().ReplaceHistograms(survivalRatePath, true);

    HNL::Statistics::DistComparison comparison{applier.SeparationTrain()};
    std::print("{},{},", comparison.separation1, comparison.separation2);

    comparison = applier.SeparationValid();
    std::print("{},{},", comparison.separation1, comparison.separation2);

    comparison = applier.SeparationApply();
    std::print("{},{},", comparison.separation1, comparison.separation2);
  }

  // TMVAApplier
  comparison = HNL::Statistics::Separation(signalPdf, bkgrndPdf);
  std::print("{},{}", comparison.separation1, comparison.separation2);

  std::println();

  /* ----- PNG Output ----- */
  if (!pngPath) return 0;

  gStyle->SetOptStat(111111);

  TH1D histSignalPdf{"histSignalPdf", "Signal Pdf", nBins, min, max};
  TH1D histBkgrndPdf{"histBkgrndPdf", "Background Pdf", nBins, min, max};

  for (int bin{1}; bin <= nBins; ++bin) {
    histSignalPdf.SetBinContent(bin, signalPdf[bin]);
    histBkgrndPdf.SetBinContent(bin, bkgrndPdf[bin]);
  }

  histSignalPdf.Scale(1 / histSignalPdf.Integral(1, nBins, "WIDTH"));
  histBkgrndPdf.Scale(1 / histBkgrndPdf.Integral(1, nBins, "WIDTH"));

  applier.Method().SavePNG(*pngPath, &histSignalPdf, &histBkgrndPdf);

  return 0;
}
