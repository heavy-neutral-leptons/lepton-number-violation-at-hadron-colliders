// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

// Standard Library
#include <istream>
#include <string>

// HNL
#include "Generic.hpp"
#include "SimulationReaders/read_slha.h"

namespace HNL::Model {
/**
 * @brief Symmetry-protected seesaw with a pseudo-Dirac heavy-neutrino pair.
 */
struct SPSS final : Generic {
 public:
  /**
   * @brief Build the benchmark from an SLHA card.
   *
   * @param slhaReader Parsed SLHA input.
   * @param massSplitting Optional pseudo-Dirac mass splitting to override the card value.
   * @param damping Optional effective damping parameter.
   */
  explicit SPSS(const MG5_aMC::SLHAReader &slhaReader, double massSplitting = 0,
                double damping = 0);
  /**
   * @brief Reconstruct the benchmark from `Vector()` output.
   *
   * @param parametersVector Serialized model parameters.
   */
  explicit SPSS(const std::vector<double> &parametersVector);

  /** @brief Assign from the serialized parameter vector returned by `Vector()`. */
  SPSS &operator=(const std::vector<double> &parametersVector);
  /** @brief Assign from the serialized parameter vector returned by `Vector()`. */
  SPSS &operator=(const std::vector<double> &&parametersVector);

  [[nodiscard]] std::unique_ptr<Generic> Clone() const final {
    return std::make_unique<SPSS>(*this);
  }

  [[nodiscard]] constexpr const char *CSVHeader() const final {
    return "meanMass,thetaSquared,meanWidth,massSplitting,damping,lnv2lncRatio,inverseSeesawMass";
  }
  [[nodiscard]] std::string CSV() const final;

  [[nodiscard]] std::vector<double> Vector() const final;

  double meanMass{-1};  ///< Mean heavy-neutrino mass, in GeV.

  double ve{-1};  ///< Electron-flavor active mixing component.
  double vm{-1};  ///< Muon-flavor active mixing component.
  double vt{-1};  ///< Tau-flavor active mixing component.

  double fe{-1};  ///< Electron-flavor sterile component.
  double fm{-1};  ///< Muon-flavor sterile component.
  double ft{-1};  ///< Tau-flavor sterile component.

  double n4Width{-1};  ///< Width of the lighter pseudo-Dirac state, in GeV.
  double n5Width{-1};  ///< Width of the heavier pseudo-Dirac state, in GeV.

  double massSplitting{-1};      ///< Mass splitting between the pseudo-Dirac states, in GeV.
  double damping{-1};            ///< Effective damping parameter used in oscillation observables.
  double inverseSeesawMass{-1};  ///< Light-neutrino mass from the inverse-seesaw relation, in GeV.

  struct Scaling;
  double thetaSquared{-1};  ///< Override for the total squared coupling.
  double meanWidth{-1};     ///< Override for the mean width.

  /**
   * @brief Override the pseudo-Dirac coupling squared `|theta|^2`.
   *
   * @param thetaSquared Coupling squared.
   * @param crossSectionEnhancement Multiplicative cross-section scaling paired with the override.
   */
  void SetThetaSquared(double thetaSquared, double crossSectionEnhancement);
  /**
   * @brief Override the pseudo-Dirac mass splitting.
   *
   * @param massSplitting Mass splitting, in GeV.
   * @param massSplittingThreshold When non-zero, recompute `damping` from the new mass splitting.
   */
  void SetMassSplitting(double massSplitting, double massSplittingThreshold = 0);
  /**
   * @brief Override the inverse-seesaw light-neutrino mass.
   *
   * @param inverseSeesawMass Light-neutrino mass in the inverse type-I seesaw limit, in GeV.
   * @param massSplittingThreshold When non-zero, recompute `damping` from the new mass splitting.
   */
  void SetInverseSeesawMass(double inverseSeesawMass, double massSplittingThreshold = 0);

  /**
   * @brief Return the mean heavy-neutrino mass.
   */
  [[nodiscard]] double MeanMass() const { return meanMass; };
  /**
   * @brief Return the effective coupling squared `|theta|^2`.
   */
  [[nodiscard]] double ThetaSquared() const { return thetaSquared; };
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
  [[nodiscard]] double MeanWidth() const { return meanWidth; };
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
  [[nodiscard]] double BaselineThetaSquared() const;
  [[nodiscard]] double BaselineMeanWidth() const;

  void FixMassSplitting();
  void FixInverseSeesawMass();
  void FixDamping(double massSplittingThreshold);
};

struct SPSS::Scaling {
  double thetaSquared;
  double scalingFactor;
};

std::istream &operator>>(std::istream &in, SPSS::Scaling &tss);
}  // namespace HNL::Model
