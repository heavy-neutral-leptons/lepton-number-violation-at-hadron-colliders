// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

// Standard Library
#include <string>

// HNL
#include "Generic.hpp"
#include "SimulationReaders/read_slha.h"

namespace HNL::Model {
/**
 * @brief Phenomenological symmetry-protected seesaw with a pseudo-Dirac heavy-neutrino pair.
 */
struct pSPSS final : Generic {
 public:
  /**
   * @brief Construct an empty benchmark with optional imposed oscillation parameters.
   *
   * @param deltam Mass splitting to impose, in GeV.
   * @param damping Effective damping parameter to impose.
   */
  explicit pSPSS(double deltam = 0, double damping = 0);
  /**
   * @brief Build the benchmark from an SLHA card.
   *
   * @param slhaReader Parsed SLHA input.
   * @param deltam Optional mass splitting override, in GeV.
   * @param damping Optional damping override.
   */
  explicit pSPSS(const MG5_aMC::SLHAReader &slhaReader, double deltam = 0, double damping = 0);
  /**
   * @brief Reconstruct the benchmark from `Vector()` output.
   *
   * @param parametersVector Serialized model parameters.
   * @param deltam Optional imposed mass splitting, in GeV.
   * @param damping Optional imposed damping parameter.
   */
  explicit pSPSS(const std::vector<double> &parametersVector, double deltam = 0,
                 double damping = 0);

  /** @brief Assign from the serialized parameter vector returned by `Vector()`. */
  pSPSS &operator=(const std::vector<double> &parametersVector);
  /** @brief Assign from the serialized parameter vector returned by `Vector()`. */
  pSPSS &operator=(const std::vector<double> &&parametersVector);

  [[nodiscard]] std::unique_ptr<Generic> Clone() const final {
    return std::make_unique<pSPSS>(*this);
  }

  [[nodiscard]] constexpr const char *CSVHeader() const final {
    return "meanMass,thetaSquared,meanWidth,deltam,damping,lnv2lncRatio,inverseSeesawMass";
  }
  [[nodiscard]] std::string CSV() const final;

  [[nodiscard]] std::vector<double> Vector() const final;

  bool imposeOscillations{};  ///< Whether `deltam` and `damping` are taken from constructor inputs.

  double meanMass{-1};  ///< Mean heavy-neutrino mass, in GeV.
  double deltam{-1};    ///< Mass splitting between the pseudo-Dirac states, in GeV.
  double theta1{-1};    ///< Electron-flavor coupling component.
  double theta2{-1};    ///< Muon-flavor coupling component.
  double theta3{-1};    ///< Tau-flavor coupling component.
  double damping{-1};   ///< Effective damping parameter used in oscillation observables.

  double n4Width{-1};  ///< Width of the lighter pseudo-Dirac state, in GeV.
  double n5Width{-1};  ///< Width of the heavier pseudo-Dirac state, in GeV.

  double inverseSeesawMass{};  ///< Light-neutrino mass from the inverse-seesaw relation, in GeV.

  /**
   * @brief Override the pseudo-Dirac mass splitting.
   *
   * @param massSplitting Mass splitting, in GeV.
   * @param fixDamping When `true`, recompute `damping` from the new splitting.
   */
  void SetMassSplitting(double massSplitting, bool fixDamping = false);
  /**
   * @brief Override the inverse-seesaw light-neutrino mass.
   *
   * @param inverseSeesawMass Light-neutrino mass in the inverse type-I seesaw limit, in GeV.
   * @param fixDamping When `true`, recompute `damping` from the updated mass splitting.
   */
  void SetInverseSeesawMass(double inverseSeesawMass, bool fixDamping = false);

  /**
   * @brief Return the mean heavy-neutrino mass.
   */
  [[nodiscard]] double MeanMass() const { return meanMass; };
  /**
   * @brief Return the total coupling squared `|theta|^2`.
   */
  [[nodiscard]] double ThetaSquared() const;
  /**
   * @brief Return the coupling squared implied directly by `theta1`, `theta2`, and `theta3`.
   */
  [[nodiscard]] double OriginalThetaSquared() const;
  /**
   * @brief Return the oscillation wavenumber converted to `mm^-1`.
   */
  [[nodiscard]] double OscillationLength() const;
  /**
   * @brief Return the damping parameter converted to `mm^-1`.
   */
  [[nodiscard]] double DampingLength() const;
  /**
   * @brief Return the average decay width of the pseudo-Dirac pair.
   */
  [[nodiscard]] double MeanWidth() const;
  /**
   * @brief Return the average decay constant converted to `mm^-1`.
   */
  [[nodiscard]] double MeanDecayConstant() const;

  /**
   * @brief Return the predicted LNV-to-LNC decay ratio.
   */
  [[nodiscard]] double LNV2LNCRatio() const;
  /**
   * @brief Return the fraction of lepton-number-conserving decays.
   *
   * `epsilon_c(R=0) = 1` and `epsilon_c(R=1) = 1/2`.
   */
  [[nodiscard]] double LNCFraction() const;
  /**
   * @brief Return the fraction of lepton-number-violating decays.
   *
   * `epsilon_v(R=0) = 0` and `epsilon_v(R=1) = 1/2`.
   */
  [[nodiscard]] double LNVFraction() const;

 private:
  void FixMassSplitting();
  void FixInverseSeesawMass();
  void FixDamping();
};
}  // namespace HNL::Model
