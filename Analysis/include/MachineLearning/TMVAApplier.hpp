// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

// Standard Library
#include <filesystem>
#include <memory>
#include <vector>

// ROOT
#include "TMVA/DataLoader.h"
#include "TTree.h"

// HNL
#include "Common/Observable.hpp"
#include "MachineLearning/TMVAReaderWrapper.hpp"
#include "Models/Generic.hpp"

namespace HNL::ML {
struct ModelWrapper;

/**
 * Apply a trained TMVA classifier to ROOT trees.
 *
 * This class is the main event-counting backend used by `TMVA-Apply` and by the separation studies.
 * It evaluates the trained classifier on held-out application events and converts the resulting
 * survival fractions into expected event yields.
 */
class TMVAApplier : public TMVAReaderWrapper {
 public:
  /**
   * Create an applier for a trained dataset.
   *
   * @param datasetPath Path identifying the trained dataset.
   * @param observables Observable definitions expected in the input trees.
   * @param truncateBkgrnd Whether background yields should be floored to one surviving event when
   * the analysis would otherwise remove all simulated events.
   */
  explicit TMVAApplier(const std::filesystem::path& datasetPath,
                       const std::vector<Common::Observable>* observables,
                       const bool truncateBkgrnd = true)
      : TMVAReaderWrapper{datasetPath, observables}, truncateBkgrnd{truncateBkgrnd} {}

  /**
   * Apply a single cut to one tree.
   *
   * Only events in the application partition contribute to the returned survival rate.
   *
   * @param tree Input tree.
   * @param cut Classifier threshold.
   * @param weight Total weight assigned to the tree.
   * @return Weighted number of surviving events.
   */
  double ProcessTree(TTree* tree, double cut, double weight = 1);

  /**
   * Apply a sorted list of cuts to one tree.
   *
   * Only events in the application partition are considered. The cut list must be sorted in
   * ascending order because the implementation stops at the first bin crossed by each event.
   *
   * @param tree Input tree.
   * @param cutList List of cuts to apply in ascending order.
   * @param weight Total weight assigned to the tree.
   * @return Weighted cut distribution.
   */
  std::vector<double> ProcessTree(TTree* tree, const std::vector<CutInfo>& cutList,
                                  double weight = 1);

  /**
   * Apply the optimized cuts stored in a model list to a signal tree.
   *
   * The model list must be sorted in ascending cut order.
   *
   * @param tree Input signal tree.
   * @param mwList Signal hypotheses to update.
   */
  void ProcessSignalTree(TTree* tree, std::vector<ModelWrapper>& mwList);

  /**
   * Apply the optimized cuts stored in a model list to a background tree.
   *
   * The model list must be sorted in ascending cut order.
   *
   * @param tree Input background tree.
   * @param mwList Signal hypotheses to update with background yields.
   * @param weight Total expected weight assigned to the background tree.
   */
  void ProcessBkgrndTree(TTree* tree, std::vector<ModelWrapper>& mwList, double weight);
  void ProcessBkgrndTrees(const std::vector<TTree*>& tree, std::vector<ModelWrapper>& mwList,
                          double weight);

 private:
  bool truncateBkgrnd;
};

/**
 * Working state for one signal hypothesis during cut optimization/application.
 *
 * The wrapper owns a clone of the corresponding physics model together with the
 * optimized cut, the expected pre-cut signal yield, and the post-cut signal and
 * background yields accumulated from the processed trees.
 */
struct ModelWrapper {
  /**
   * Construct from a model definition.
   *
   * @param model Model to clone and track through the MVA application.
   */
  explicit ModelWrapper(const Model::Generic* model) : model{model->Clone()} {}

  std::unique_ptr<Model::Generic> model;  ///< Pointer to underlying model

  CutInfo optimalCut{};

  /**
   * Store the optimized cut information associated with this model.
   *
   * @param optimalCut Cut description returned by the optimizer.
   */
  void SetCut(const CutInfo& optimalCut) { this->optimalCut = optimalCut; }

  double suppression{1};          ///< Event suppression
  double nTotalSignalEventsU{0};  ///< Expected signal events (unsuppressed)
  double nTotalSignalEventsS{0};  ///< Expected signal events (suppressed)
  /**
   * Set expected total signal events.
   *
   * @param nMaxSignalEvents Expected total signal events, without suppression
   * @param lnvSuppression Assume search for LN-violation, which suppressed under SPSS
   */
  void SetTotalSignalEvents(double nBaselineSignalEvents, bool lnvSuppression);

  double nSignalEventsU{0};  ///< Expected surviving signal events (unsuppressed)
  double nSignalEventsS{0};  ///< Expected surviving signal events (suppressed)
  double nBkgrndEvents{0};   ///< Expected surviving background events
  /**
   * Compute the exclusion significance from the accumulated post-cut yields.
   *
   * @return Exclusion significance for this model hypothesis.
   */
  [[nodiscard]] double ExcludeSignalZ() const;
  /**
   * Compute the exclusion significance from the accumulated post-cut yields.
   *
   * @return Exclusion significance for this model hypothesis.
   */
  [[nodiscard]] double ExcludeEventSuppressionZ() const;

  std::partial_ordering operator<=>(const ModelWrapper& rhs) const;

 private:
  size_t nSurvivingEvents{0};
  /**
   * Require that there is at least one surviving event.
   *
   * If the expected number of background events exceeds the amount of generated events, the claim
   * that an analysis is capable of fully removing the background is only valid up to
   * \f[
   *    \frac{\mathcal{L} \sigma}{\text{\#Simulated Events}} .
   * \f]
   * It is then necessary that at least one event is present for each background source.
   */
  void TruncateNSurvivingEvents() {
    nSurvivingEvents = nSurvivingEvents > 1UL ? nSurvivingEvents : 1UL;
  }

  friend void TMVAApplier::ProcessSignalTree(TTree* tree, std::vector<ModelWrapper>& mwList);

  friend void TMVAApplier::ProcessBkgrndTree(TTree* tree, std::vector<ModelWrapper>& mwList,
                                             double weight);

  friend void TMVAApplier::ProcessBkgrndTrees(const std::vector<TTree*>& tree,
                                              std::vector<ModelWrapper>& mwList, double weight);
};
}  // namespace HNL::ML
