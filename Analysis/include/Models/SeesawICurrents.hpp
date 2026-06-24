// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

// HNL
#include "Generic.hpp"
#include "SimulationReaders/read_slha.h"

namespace HNL::Model {
/**
 * @brief Parameter set for the Seesaw-I currents benchmark with three heavy-neutrino states.
 */
struct SeesawICurrents final : Generic {
 public:
  explicit SeesawICurrents(const MG5_aMC::SLHAReader &slhaReader);
  explicit SeesawICurrents(const std::vector<double> &parametersVector);

  [[nodiscard]] std::unique_ptr<Generic> Clone() const final {
    return std::make_unique<SeesawICurrents>(*this);
  }

  [[nodiscard]] constexpr const char *CSVHeader() const final {
    return "mn1,ve1,vm1,vt1,wn1,"
           "mn2,ve2,vm2,vt2,wn2,"
           "mn3,ve3,vm3,vt3,wn3";
  }
  [[nodiscard]] std::string CSV() const final;

  [[nodiscard]] std::vector<double> Vector() const final;

  double mn1{-1};  ///< Mass of the first heavy-neutrino state, in GeV.
  double mn2{-1};  ///< Mass of the second heavy-neutrino state, in GeV.
  double mn3{-1};  ///< Mass of the third heavy-neutrino state, in GeV.

  double ve1{-1};  ///< Electron-flavor mixing of the first heavy-neutrino state.
  double ve2{-1};  ///< Electron-flavor mixing of the second heavy-neutrino state.
  double ve3{-1};  ///< Electron-flavor mixing of the third heavy-neutrino state.

  double vm1{-1};  ///< Muon-flavor mixing of the first heavy-neutrino state.
  double vm2{-1};  ///< Muon-flavor mixing of the second heavy-neutrino state.
  double vm3{-1};  ///< Muon-flavor mixing of the third heavy-neutrino state.

  double vt1{-1};  ///< Tau-flavor mixing of the first heavy-neutrino state.
  double vt2{-1};  ///< Tau-flavor mixing of the second heavy-neutrino state.
  double vt3{-1};  ///< Tau-flavor mixing of the third heavy-neutrino state.

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
}  // namespace HNL::Model
