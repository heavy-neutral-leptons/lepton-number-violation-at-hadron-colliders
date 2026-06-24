// Copyright 2023-2026 Bruno M. S. Oliveira

// Standard Library
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

// CLI11
#include "CLI/CLI.hpp"

// ROOT
#include "TFile.h"
#include "TMVA/Config.h"
#include "TMVA/DataLoader.h"
#include "TMVA/Factory.h"
#include "TMVA/MethodBase.h"
#include "TTree.h"

// HNL
#include "Common/Log.hpp"
#include "MachineLearning/DataLoaderWrapper.hpp"
#include "MachineLearning/Observables.hpp"
#include "MachineLearning/TMVAOptions.hpp"
#include "MachineLearning/TMVASurvivalRate.hpp"
#include "ReconstructedData/ReconstructedEventData.hpp"

int main(const int argc, const char* const argv[]) {
  /* ----- CLI Arguments ----- */
  CLI::App app{};

  std::filesystem::path tmvaPath{};
  app.add_option("--dataset", tmvaPath, "Path to dataset")
      ->required()
      ->check(!CLI::ExistingDirectory);

  double luminosity{};
  app.add_option("--luminosity", luminosity, "Luminosity (pb^{-1})")->required();

  std::vector<std::filesystem::path> signalPaths{};
  app.add_option("--signal", signalPaths, "Signal files")->required()->check(CLI::ExistingFile);

  std::vector<std::vector<std::filesystem::path>> bkgrndPaths{};
  app.add_option("--background", bkgrndPaths, "Background files")
      ->required()
      ->check(CLI::ExistingFile);

  try {
    app.parse(argc, argv);
  } catch (const CLI::ParseError& e) {
    return app.exit(e);
  }

  /* ----- Identify Process and MVA Method ----- */
  const std::vector<HNL::Common::Observable>* const observables{HNL::ML::Observables(tmvaPath)};

  const HNL::ML::MVAMethod method{HNL::ML::IdentifyMethod(tmvaPath)};
  const std::string methodName{HNL::ML::ToString(method)};

  /* ----- Load Data ----- */
  TMVA::gConfig().GetIONames().fWeightFileDirPrefix = tmvaPath.parent_path();

  // First pass to confirm data validity and compute event weight normalization
  size_t nSignalEventsSim{0};
  double nSignalEventsExp{0};
  for (const std::filesystem::path& path : signalPaths) {
    HNL::Reconstruct::ReconstructedEventFile file{path};
    nSignalEventsSim += file.GetEntries();
    nSignalEventsExp += file.NumberOfEvents(luminosity);
  }

  if (nSignalEventsSim == 0) {
    LogWarn("No signal events were provided");
    return 0;
  }

  Log("The {} signal(s) have a total of {} simulated events and an expected {} events",
      signalPaths.size(), nSignalEventsSim, nSignalEventsExp);

  size_t nBkgrndEventsSim{0};
  double nBkgrndEventsExp{0};
  for (const std::vector<std::filesystem::path>& thisBkgrndPaths : bkgrndPaths) {
    HNL::Reconstruct::ReconstructedEventChain files{thisBkgrndPaths};
    nBkgrndEventsSim += files.GetEntries();
    nBkgrndEventsExp += files.NumberOfEvents(luminosity);
  }

  if (nBkgrndEventsSim == 0) {
    LogWarn("No background events were provided");
    return 0;
  }

  Log("The {} background(s) have a total of {} simulated events and an expected {} events",
      bkgrndPaths.size(), nBkgrndEventsSim, nBkgrndEventsExp);

  const double signalWeightNorm{1};
  const double bkgrndWeightNorm{1};
  Log("Normalization:\n Signal:     {}\n Background: {}", signalWeightNorm, bkgrndWeightNorm);

  // Second pass to load data
  HNL::ML::DataLoaderWrapper dlw{tmvaPath.filename(), observables};

  double totalSignalWeight{0};
  for (const std::filesystem::path& path : signalPaths) {
    Log("Adding signal — {}", path.string());

    HNL::Reconstruct::ReconstructedEventFile file{path};
    const double treeWeight{signalWeightNorm};
    totalSignalWeight += treeWeight;

    Log(" #Expected:  {:.0f} ({:2.1f}%)", file.NumberOfEvents(luminosity),
        100 * file.ReconstructionEfficiency());
    Log(" #Simulated: {}", file.GetEntries());
    Log(" #E / #S:    {:.2e}",
        file.NumberOfEvents(luminosity) / static_cast<double>(file.GetEntries()));
    Log(" Weight:     {:.2e} -> {:.2e}", treeWeight,
        treeWeight / static_cast<double>(file.GetEntries()));

    dlw.AddSignalTree(file.Tree(), treeWeight);
  }

  double totalBkgrndWeight{0};
  for (const std::vector<std::filesystem::path>& thisBkgrndPaths : bkgrndPaths) {
    LogC("Adding background {} —", thisBkgrndPaths.size());
    for (const std::filesystem::path& path : thisBkgrndPaths) LogC(" {}", path.c_str());
    Log("");

    HNL::Reconstruct::ReconstructedEventChain files{thisBkgrndPaths};
    const double treeWeight{bkgrndWeightNorm * files.NumberOfEvents(luminosity)};
    totalBkgrndWeight += treeWeight;

    Log(" #Expected:  {:.0f} ({:2.1f}%)", files.NumberOfEvents(luminosity),
        100 * files.ReconstructionEfficiency());
    Log(" #Simulated: {}", files.GetEntries());
    Log(" #E / #S:    {:.2e}",
        files.NumberOfEvents(luminosity) / static_cast<double>(files.GetEntries()));
    Log(" Weight:     {:.2e} -> {:.2e}", treeWeight,
        treeWeight / static_cast<double>(files.GetEntries()));

    dlw.AddBkgrndTree(files.Tree(), treeWeight);
  }

  Log("Total weights:\n Signal:     {}\n Background: {}", totalSignalWeight, totalBkgrndWeight);

  dlw.Prepare(HNL::ML::Options::split);

  /* ----- Book Methods ----- */
  std::filesystem::path outPath{tmvaPath};
  outPath.replace_extension(".root");

  TFile outputFile{outPath.c_str(), "CREATE"};
  TMVA::Factory factory{"HNL", &outputFile, HNL::ML::Options::factory};

  const TMVA::Types::EMVA methodTMVA{HNL::ML::EMVA(method)};
  const std::string methodOptions{HNL::ML::Options::Method(method)};

  factory.BookMethod(dlw.Underlying(), methodTMVA, methodName, methodOptions);

  /* ----- Train, Test, and Evaluate ----- */
  factory.TrainAllMethods();
  factory.TestAllMethods();
  factory.EvaluateAllMethods();

  outputFile.Write();
  outputFile.Close();

  /* ----- Optimal Cut List ----- */
  const TMVA::MethodBase* const methodResult{
      dynamic_cast<TMVA::MethodBase*>(factory.GetMethod(tmvaPath.filename().c_str(), methodName))};

  outPath.replace_extension("csv");
  std::ofstream outputStream{outPath};
  for (bool printHeader{true}; const std::filesystem::path& path : signalPaths) {
    HNL::Reconstruct::ReconstructedEventFile file{path};

    const std::unique_ptr<HNL::Model::Generic> model{file.Model()};
    if (std::exchange(printHeader, false)) {
      std::println(outputStream, "{},nSignalEvents,optimalCut,significance", model->CSVHeader());
    }

    nSignalEventsExp = file.NumberOfEvents(luminosity);

    double significance{0};
    const double optimalCut{
        methodResult->GetMaximumSignificance(nSignalEventsExp, nBkgrndEventsExp, significance)};

    std::println(outputStream, "{},{},{},{}", model->CSV(), nSignalEventsExp, optimalCut,
                 significance);
  }

  /* ----- Survival Rate ----- */
  if (method == HNL::ML::MVAMethod::Cuts) return 0;

  HNL::ML::TMVASurvivalRate rw{tmvaPath, observables};

  for (const std::filesystem::path& path : signalPaths) {
    HNL::Reconstruct::ReconstructedEventFile events{path};
    rw.ProcessSignalTree(events);
  }

  for (const std::vector<std::filesystem::path>& thisBkgrndPaths : bkgrndPaths) {
    HNL::Reconstruct::ReconstructedEventChain events{thisBkgrndPaths};
    rw.ProcessBkgrndTree(events);
  }

  return 0;
}
