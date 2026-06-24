// Copyright 2023-2026 Bruno M. S. Oliveira

// Standard Library
#include <algorithm>
#include <filesystem>
#include <string>
#include <vector>

// HNL
#include "Common/Log.hpp"
#include "MachineLearning/Observables.hpp"
#include "MachineLearning/TMVAApplier.hpp"
#include "Models/SPSS.hpp"
#include "ReconstructedData/ReconstructedEventData.hpp"

// CLI11
#include "CLI/CLI.hpp"

namespace {
constexpr const char* const out{"{},{},{},{},{},{},{},{},{},{},{},{},{}"};
void CSVOutput(const HNL::Model::Generic* model, const double nTotalSignalEvents,
               const double suppression) {
  std::println(out, model->CSV(), model->XSectionScaling(), nTotalSignalEvents,
               suppression * nTotalSignalEvents, "NA", "NA", "NA", "NA", "NA", "NA", "NA", 0., 0.);
}

void CSVOutput(const HNL::ML::ModelWrapper& mw) {
  const double zExcludeSignal{mw.ExcludeSignalZ()};
  const double zExcludeSuppression{mw.ExcludeEventSuppressionZ()};

  std::println(out, mw.model->CSV(), mw.model->XSectionScaling(), mw.nTotalSignalEventsU,
               mw.nTotalSignalEventsS, mw.optimalCut.cut, mw.optimalCut.nSignalEvents,
               mw.optimalCut.nBkgrndEvents, mw.optimalCut.significance, mw.nSignalEventsU,
               mw.nSignalEventsS, mw.nBkgrndEvents, zExcludeSignal, zExcludeSuppression);
}
}  // namespace

int main(const int argc, const char* const argv[]) {
  /* ----- CLI Arguments ----- */
  CLI::App app{};

  std::filesystem::path tmvaPath{};
  app.add_option("--dataset", tmvaPath, "Path to dataset")
      ->required()
      ->check(CLI::ExistingDirectory);

  double luminosity{};
  app.add_option("--luminosity", luminosity, "Luminosity (L / pb^{-1})")->required();

  double bkgrndFactor{1.};
  app.add_option("--background-factor", bkgrndFactor, "Background factor")
      ->check(CLI::NonNegativeNumber);

  std::filesystem::path signalPath{};
  app.add_option("--signal", signalPath, "Signal file")->required()->check(CLI::ExistingFile);

  std::vector<HNL::Model::SPSS::Scaling> tssList{};
  app.add_option("--theta-squared", tssList,
                 "Absolute mixing angle squared and resulting cross section enhancement (|θ|²)");

  std::vector<double> deltamList{};
  app.add_option("--mass-splitting", deltamList, "Mass splitting for LNV searches (Δm / GeV)")
      ->check(CLI::NonNegativeNumber);

  std::vector<double> issMassList{};
  app.add_option("--inverse-seesaw-mass", issMassList,
                 "Inverse seesaw mass for LNV searches (m₃ / GeV)")
      ->check(CLI::NonNegativeNumber);

  double massSplittingThreshold{};
  app.add_option("--mass-splitting-threshold", massSplittingThreshold,
                 "Mass splitting threshold when damping takes hold (Δm₀ / GeV)")
      ->check(CLI::NonNegativeNumber)
      ->required();

  std::vector<std::vector<std::filesystem::path>> bkgrndPaths{};
  app.add_option("--background", bkgrndPaths, "Background files")
      ->required()
      ->check(CLI::ExistingFile);

  try {
    app.parse(argc, argv);
  } catch (const CLI::ParseError& e) {
    return app.exit(e);
  }

  /* ----- Luminosity & Event Suppression ----- */
  const double signalLuminosity{luminosity};
  const double bkgrndLuminosity{bkgrndFactor * luminosity};

  const bool noSuppression{deltamList.empty() && issMassList.empty()};
  if (noSuppression) {
    Log("Assuming no suppression in the absense of mass splittings.");
    deltamList.emplace_back(0);
  } else {
    Log("Assuming lepton number violation suppression by NNOs.");
  }

  /* ----- Signal ----- */
  HNL::Reconstruct::ReconstructedEventFile signal{signalPath};

  const std::unique_ptr<HNL::Model::Generic> _model{signal.Model()};
  HNL::Model::SPSS* const model{dynamic_cast<HNL::Model::SPSS*>(_model.get())};

  if (tssList.empty()) {
    tssList.emplace_back(model->ThetaSquared(), 1.);
  } else {
    std::ranges::sort(tssList, std::greater{}, &HNL::Model::SPSS::Scaling::scalingFactor);
  }

  const double nBaselineSignalEvents{signal.NumberOfEvents(signalLuminosity)};
  if (tssList.front().scalingFactor * nBaselineSignalEvents < 1) {
    if (noSuppression) {
      model->SetMassSplitting(0, massSplittingThreshold);

      for (const HNL::Model::SPSS::Scaling& tss : tssList) {
        model->SetThetaSquared(tss.thetaSquared, tss.scalingFactor);
        CSVOutput(model, model->XSectionScaling() * nBaselineSignalEvents, 1.);
      }

      return 0;
    }

    for (const HNL::Model::SPSS::Scaling& tss : tssList) {
      model->SetThetaSquared(tss.thetaSquared, tss.scalingFactor);

      for (const double deltam : deltamList) {
        model->SetMassSplitting(deltam, massSplittingThreshold);
        CSVOutput(model, nBaselineSignalEvents,
                  2 * model->LNVFraction() * model->XSectionScaling());
      }

      for (const double issMass : issMassList) {
        model->SetInverseSeesawMass(issMass, massSplittingThreshold);
        CSVOutput(model, nBaselineSignalEvents,
                  2 * model->LNVFraction() * model->XSectionScaling());
      }
    }

    return 0;
  }

  std::vector<HNL::ML::ModelWrapper> mwList;
  mwList.reserve(tssList.size() * (deltamList.size() + issMassList.size()));

  for (const HNL::Model::SPSS::Scaling& tss : tssList) {
    model->SetThetaSquared(tss.thetaSquared, tss.scalingFactor);

    for (const double deltam : deltamList) {
      model->SetMassSplitting(deltam, massSplittingThreshold);
      mwList.emplace_back(model);
    }

    for (const double issMass : issMassList) {
      const auto predicate{[tss, issMass](const HNL::ML::ModelWrapper& mw) {
        const HNL::Model::SPSS* const model{dynamic_cast<HNL::Model::SPSS*>(mw.model.get())};
        return model->ThetaSquared() == tss.thetaSquared && model->inverseSeesawMass == issMass;
      }};

      if (std::ranges::none_of(mwList, predicate)) {
        model->SetInverseSeesawMass(issMass, massSplittingThreshold);
        mwList.emplace_back(model);
      }
    }
  }

  /* ----- Background ----- */
  double nTotalBkgrndEvents{0};
  for (const std::vector<std::filesystem::path>& thisBkgrndPaths : bkgrndPaths) {
    HNL::Reconstruct::ReconstructedEventChain bkgrndWrapper{thisBkgrndPaths};
    nTotalBkgrndEvents += bkgrndWrapper.NumberOfEvents(bkgrndLuminosity);

    Log<1>("Background {} has {} events ({} / {})\nTotal = {} events", bkgrndWrapper.Name(),
           bkgrndWrapper.NumberOfEvents(bkgrndLuminosity), bkgrndWrapper.EffectiveCrossSection(),
           bkgrndWrapper.TotalCrossSection(), nTotalBkgrndEvents);
  }

  /* ----- Analysis ----- */
  const std::vector<HNL::Common::Observable>* const observables{HNL::ML::Observables(tmvaPath)};

  constexpr bool truncateBkgrnd{true};
  HNL::ML::TMVAApplier rw{tmvaPath, observables, truncateBkgrnd};
  if (!rw.Method().CheckHealth()) {
    LogError("Main method is invalid");
    return 1;
  }

  std::filesystem::path survivalRatePath{tmvaPath};
  survivalRatePath.concat("-SurvivalRate.root");
  if (std::filesystem::exists(survivalRatePath)) {
    rw.Method().ReplaceHistograms(survivalRatePath, truncateBkgrnd);
  }

  for (HNL::ML::ModelWrapper& mw : mwList) {
    mw.SetTotalSignalEvents(nBaselineSignalEvents, !noSuppression);
    rw.SetDatasetSplit(HNL::ML::DatasetSplit::Validation);
    mw.SetCut(rw.OptimalCut(mw.nTotalSignalEventsS, nTotalBkgrndEvents));
  }

  std::ranges::sort(mwList, std::less{});

  rw.ProcessSignalTree(signal.Tree(), mwList);
  for (const std::vector<std::filesystem::path>& thisBkgrndPaths : bkgrndPaths) {
    HNL::Reconstruct::ReconstructedEventChain bkgrndWrapper{thisBkgrndPaths};

    rw.ProcessBkgrndTree(bkgrndWrapper.Tree(), mwList,
                         bkgrndWrapper.NumberOfEvents(bkgrndLuminosity));
  }

  /* ----- CLI Output ----- */
  for (const HNL::ML::ModelWrapper& mw : mwList) CSVOutput(mw);

  return 0;
}
