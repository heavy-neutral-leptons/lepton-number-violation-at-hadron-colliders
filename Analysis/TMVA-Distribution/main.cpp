// Copyright 2023-2026 Bruno M. S. Oliveira

// Standard Library
#include <filesystem>
#include <string>
#include <vector>

// CLI11
#include "CLI/CLI.hpp"

// HNL
#include "Common/Log.hpp"
#include "MachineLearning/Observables.hpp"
#include "MachineLearning/TMVAPlotter.hpp"

int main(const int argc, const char* const argv[]) {
  /* ----- CLI Arguments ----- */
  CLI::App app{};

  std::filesystem::path tmvaPath{};
  app.add_option("--dataset", tmvaPath, "Path to dataset")
      ->required()
      ->check(CLI::ExistingDirectory);

  double luminosity{};
  app.add_option("--luminosity", luminosity, "Luminosity (pb^{-1})")->required();

  double signalFactor{1.};
  app.add_option("--signal-factor", signalFactor, "Signal factor")->check(CLI::NonNegativeNumber);

  double bkgrndFactor{1.};
  app.add_option("--background-factor", bkgrndFactor, "Background factor")
      ->check(CLI::NonNegativeNumber);

  std::vector<std::filesystem::path> signalPaths{};
  app.add_option("--signal", signalPaths, "Path to events")->required()->check(CLI::ExistingFile);

  std::vector<std::vector<std::filesystem::path>> bkgrndPaths{};
  app.add_option("--background", bkgrndPaths, "Background files")
      ->required()
      ->check(CLI::ExistingFile);

  std::filesystem::path outPath{};
  app.add_option("--output", outPath, "Output files")->required();

  try {
    app.parse(argc, argv);
  } catch (const CLI::ParseError& e) {
    return app.exit(e);
  }

  /* ----- Luminosity & Event Suppression ----- */
  const double signalLuminosity{signalFactor * luminosity};
  const double bkgrndLuminosity{bkgrndFactor * luminosity};

  /* ----- Setup Reader and Process Trees ----- */
  const std::vector<HNL::Common::Observable>* const observables{HNL::ML::Observables(tmvaPath)};

  constexpr bool truncateBkgrnd{true};
  HNL::ML::TMVAPlotter rw{tmvaPath, observables, signalLuminosity, bkgrndLuminosity};
  if (!rw.Method().CheckHealth()) {
    LogError("Main method is invalid");
    return 1;
  }

  std::filesystem::path survivalRatePath{tmvaPath};
  survivalRatePath.concat("-SurvivalRate.root");
  if (std::filesystem::exists(survivalRatePath)) {
    rw.Method().ReplaceHistograms(survivalRatePath, truncateBkgrnd);
  }

  for (const std::filesystem::path& path : signalPaths) {
    HNL::Reconstruct::ReconstructedEventFile events{path};
    rw.ProcessSignalTree(events);
  }

  for (const std::vector<std::filesystem::path>& thisBkgrndPaths : bkgrndPaths) {
    HNL::Reconstruct::ReconstructedEventChain events{thisBkgrndPaths};
    rw.ProcessBkgrndTree(events);
  }

  /* ----- Output ----- */
  std::filesystem::create_directories(outPath.parent_path());
  rw.SavePNG(outPath.replace_extension("png"));
  rw.SaveCSV(outPath.replace_extension("csv"));

  outPath = outPath.replace_extension().concat("-significance.csv");
  rw.Method().SaveCSV(outPath);

  return 0;
}
