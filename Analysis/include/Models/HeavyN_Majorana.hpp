// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

// HNL
#include "Generic.hpp"
#include "SimulationReaders/read_slha.h"

namespace HNL::Model::HeavyN {
/**
 * @brief Three-generation Majorana heavy-neutrino.
 */
struct Majorana final : Generic {
 public:
  /**
   * @brief Build the benchmark from an SLHA card.
   *
   * @param slhaReader Parsed SLHA input.
   */
  explicit Majorana(const MG5_aMC::SLHAReader &slhaReader);
  /**
   * @brief Reconstruct the benchmark from `Vector()` output.
   *
   * @param parametersVector Serialized model parameters.
   */
  explicit Majorana(const std::vector<double> &parametersVector);

  [[nodiscard]] std::unique_ptr<Generic> Clone() const final {
    return std::make_unique<Majorana>(*this);
  }

  [[nodiscard]] constexpr const char *CSVHeader() const final {
    return "mn1,ven1,vmun1,vtan1,wn1,"
           "mn2,ven2,vmun2,vtan2,wn2,"
           "mn3,ven3,vmun3,vtan3,wn3";
  }
  [[nodiscard]] std::string CSV() const final;

  [[nodiscard]] std::vector<double> Vector() const final;

  double mn1{-1};  ///< Mass of the first heavy-neutrino state, in GeV.
  double mn2{-1};  ///< Mass of the second heavy-neutrino state, in GeV.
  double mn3{-1};  ///< Mass of the third heavy-neutrino state, in GeV.

  double ven1{-1};  ///< Electron-flavor mixing of the first heavy-neutrino state.
  double ven2{-1};  ///< Electron-flavor mixing of the second heavy-neutrino state.
  double ven3{-1};  ///< Electron-flavor mixing of the third heavy-neutrino state.

  double vmun1{-1};  ///< Muon-flavor mixing of the first heavy-neutrino state.
  double vmun2{-1};  ///< Muon-flavor mixing of the second heavy-neutrino state.
  double vmun3{-1};  ///< Muon-flavor mixing of the third heavy-neutrino state.

  double vtan1{-1};  ///< Tau-flavor mixing of the first heavy-neutrino state.
  double vtan2{-1};  ///< Tau-flavor mixing of the second heavy-neutrino state.
  double vtan3{-1};  ///< Tau-flavor mixing of the third heavy-neutrino state.

  double n1Width{-1};  ///< Decay width of the first heavy-neutrino state, in GeV.
  double n2Width{-1};  ///< Decay width of the second heavy-neutrino state, in GeV.
  double n3Width{-1};  ///< Decay width of the third heavy-neutrino state, in GeV.

  /**
   * @brief Return the total coupling squared of the first heavy-neutrino state.
   */
  [[nodiscard]] double Theta1Squared() const;
  /**
   * @brief Return the total coupling squared of the second heavy-neutrino state.
   */
  [[nodiscard]] double Theta2Squared() const;
  /**
   * @brief Return the total coupling squared of the third heavy-neutrino state.
   */
  [[nodiscard]] double Theta3Squared() const;
};
}  // namespace HNL::Model::HeavyN
