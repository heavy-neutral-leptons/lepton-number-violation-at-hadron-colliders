// Copyright 2023-2026 Bruno M. S. Oliveira

// Standard Library
#include <ranges>
#include <vector>

// ROOT
#include "ROOT/RDataFrame.hxx"
#include "TMVA/DataLoader.h"
#include "TTree.h"

// HNL
#include "Common/Log.hpp"
#include "MachineLearning/DatasetSplit.hpp"
#include "MachineLearning/TMVAApplier.hpp"
#include "Models/SPSS.hpp"
#include "Reconstruction/SurvivalRate.hpp"
#include "Statistics/Significance.hpp"

namespace HNL::ML {
double TMVAApplier::ProcessTree(TTree* const tree, const double cut, const double weight) {
  SetTreeAddresses(tree);

  const int64_t nSimulatedEvents{tree->GetEntries()};
  int64_t nApplyEvents{0};
  int64_t nSurvivingEvents{0};

  for (int64_t idx{0}; idx < nSimulatedEvents; ++idx) {
    if (!ToApplicationSet(idx)) continue;
    GetTreeEntry(tree, idx);
    ++nApplyEvents;

    if ((this->*EvaluateMVACut)(cut)) ++nSurvivingEvents;
  }

  return weight * Reconstruct::SurvivalRate(nSurvivingEvents, nApplyEvents);
}

std::vector<double> TMVAApplier::ProcessTree(TTree* const tree, const std::vector<CutInfo>& cutList,
                                             const double weight) {
  SetTreeAddresses(tree);

  std::vector<double> cutDistribution(cutList.size(), 0);
  int64_t nApplyEvents{0};
  for (int64_t idx{0}; idx < tree->GetEntries(); ++idx) {
    if (!ToApplicationSet(idx)) continue;
    ++nApplyEvents;
    GetTreeEntry(tree, idx);

    if (method == MVAMethod::Cuts) {
      for (auto [cut, nEventsCut] : std::views::zip(cutList, cutDistribution)) {
        if (!(this->*EvaluateMVACut)(cut.cut)) {
          ++nEventsCut;
          break;
        }
      }
    } else {
      const double bdt{reader.EvaluateMVA(methodTag)};
      for (auto [cut, nEventsCut] : std::views::zip(cutList, cutDistribution)) {
        if (bdt <= cut.cut) {
          ++nEventsCut;
          break;
        }
      }
    }
  }

  for (double& nEventsCut : cutDistribution) {
    nEventsCut = weight * Reconstruct::SurvivalRate(nEventsCut, nApplyEvents);
  }

  return cutDistribution;
}

void TMVAApplier::ProcessSignalTree(TTree* const tree, std::vector<ModelWrapper>& mwList) {
  SetTreeAddresses(tree);

  const int64_t nSimulatedEvents{tree->GetEntries()};
  int64_t nApplyEvents{0};

  for (ModelWrapper& mw : mwList) mw.nSurvivingEvents = 0;
  for (int64_t idx{0}; idx < nSimulatedEvents; ++idx) {
    if (!ToApplicationSet(idx)) continue;
    GetTreeEntry(tree, idx);
    ++nApplyEvents;

    if (method == MVAMethod::Cuts) {
      for (ModelWrapper& mw : mwList) {
        if ((this->*EvaluateMVACut)(mw.optimalCut.cut)) {
          ++mw.nSurvivingEvents;
        } else {
          break;
        }
      }
    } else {
      const double bdt{reader.EvaluateMVA(methodTag)};
      for (ModelWrapper& mw : mwList) {
        if (bdt > mw.optimalCut.cut) {
          ++mw.nSurvivingEvents;
        } else {
          break;
        }
      }
    }
  }

  for (ModelWrapper& mw : mwList) {
    const double survivalRate{Reconstruct::SurvivalRate(mw.nSurvivingEvents, nApplyEvents)};
    mw.nSignalEventsU += survivalRate * mw.nTotalSignalEventsU;
    mw.nSignalEventsS += survivalRate * mw.nTotalSignalEventsS;
  }
}

void TMVAApplier::ProcessBkgrndTree(TTree* const tree, std::vector<ModelWrapper>& mwList,
                                    const double weight) {
  if (weight <= 0) {
    LogError("Non-positive weight {}", weight);
    exit(1);
  }

  SetTreeAddresses(tree);

  const int64_t nSimulatedEvents{tree->GetEntries()};
  int64_t nApplyEvents{0};

  for (ModelWrapper& mw : mwList) mw.nSurvivingEvents = 0;
  for (int64_t idx{0}; idx < nSimulatedEvents; ++idx) {
    if (!ToApplicationSet(idx)) continue;
    GetTreeEntry(tree, idx);
    ++nApplyEvents;

    if (method == MVAMethod::Cuts) {
      for (ModelWrapper& mw : mwList) {
        if ((this->*EvaluateMVACut)(mw.optimalCut.cut)) {
          ++mw.nSurvivingEvents;
        } else {
          break;
        }
      }
    } else {
      const double bdt{reader.EvaluateMVA(methodTag)};
      for (ModelWrapper& mw : mwList) {
        if (bdt > mw.optimalCut.cut) {
          ++mw.nSurvivingEvents;
        } else {
          break;
        }
      }
    }
  }

  for (ModelWrapper& mw : mwList) {
    if (truncateBkgrnd) mw.TruncateNSurvivingEvents();
    const double survivalRate{Reconstruct::SurvivalRate(mw.nSurvivingEvents, nApplyEvents)};
    mw.nBkgrndEvents += survivalRate * weight;
  }
}

void TMVAApplier::ProcessBkgrndTrees(const std::vector<TTree*>& trees,
                                     std::vector<ModelWrapper>& mwList, const double weight) {
  if (weight <= 0) {
    LogError("Non-positive weight {}", weight);
    exit(1);
  }

  int64_t nApplyEvents{0};
  for (ModelWrapper& mw : mwList) mw.nSurvivingEvents = 0;
  for (TTree* tree : trees) {
    SetTreeAddresses(tree);

    for (int64_t idx{0}; idx < tree->GetEntries(); ++idx) {
      if (!ToApplicationSet(idx)) continue;
      GetTreeEntry(tree, idx);
      ++nApplyEvents;

      if (method == MVAMethod::Cuts) {
        for (ModelWrapper& mw : mwList) {
          if ((this->*EvaluateMVACut)(mw.optimalCut.cut)) {
            ++mw.nSurvivingEvents;
          } else {
            break;
          }
        }
      } else {
        const double bdt{reader.EvaluateMVA(methodTag)};
        for (ModelWrapper& mw : mwList) {
          if (bdt > mw.optimalCut.cut) {
            ++mw.nSurvivingEvents;
          } else {
            break;
          }
        }
      }
    }
  }

  for (ModelWrapper& mw : mwList) {
    if (truncateBkgrnd) mw.TruncateNSurvivingEvents();
    const double survivalRate{Reconstruct::SurvivalRate(mw.nSurvivingEvents, nApplyEvents)};
    mw.nBkgrndEvents += survivalRate * weight;
  }
}

void ModelWrapper::SetTotalSignalEvents(const double nBaselineSignalEvents,
                                        const bool lnvSuppression) {
  if (lnvSuppression) {
    if (model->ID() != Model::ModelID::SPSS) {
      LogError("Attempted to use LNV suppression in unsupported model");
      exit(1);
    }

    const Model::SPSS* const spss{dynamic_cast<Model::SPSS*>(model.get())};
    suppression = 2 * spss->LNVFraction();
  } else {
    suppression = 1;
  }

  nTotalSignalEventsU = model->XSectionScaling() * nBaselineSignalEvents;
  nTotalSignalEventsS = suppression * nTotalSignalEventsU;
}

[[nodiscard]] double ModelWrapper::ExcludeSignalZ() const {
  return HNL::Statistics::ExclusionSignificance(nSignalEventsS, nBkgrndEvents);
}

[[nodiscard]] double ModelWrapper::ExcludeEventSuppressionZ() const {
  return HNL::Statistics::PoissonSignificance(nBkgrndEvents + nSignalEventsU,
                                              nBkgrndEvents + nSignalEventsS);
}

std::partial_ordering ModelWrapper::operator<=>(const ModelWrapper& rhs) const {
  return optimalCut.cut <=> rhs.optimalCut.cut;
}
}  // namespace HNL::ML
