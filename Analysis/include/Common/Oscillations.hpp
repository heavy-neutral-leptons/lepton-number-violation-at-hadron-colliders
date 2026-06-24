// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

// Standard Library
#include <cmath>
#include <cstdint>
#include <format>
#include <string_view>

// HNL
#include "Constants/Experiments.hpp"
#include "Constants/Physics.hpp"

namespace HNL::Common {
///@name Event classification
///@{
/**
 * Classification of events as  Lepton Number (LN) conserving and violating.
 */
enum class LNState : uint8_t {
  error,  ///< Error
  lnc,    ///< Lepton Number Conserving
  lnv     ///< Lepton Number Violating
};

/**
 * @brief Converts an LN state to its string representation.
 *
 * @param lns LNS value
 *
 * @return String view representation ("LNC", "LNV", or "ERROR")
 */
constexpr std::string_view ToString(const LNState lns) {
  switch (lns) {
    case LNState::error:
      return "ERROR";
    case LNState::lnc:
      return "LNC";
    case LNState::lnv:
      return "LNV";
  }
}

/** @brief Identifier for truth-level LNS tagging. */
inline constexpr char const *const lnsTruth{"lnsTruth"};
/** @brief Identifier for detector-level LNS tagging. */
inline constexpr char const *const lnsDetector{"lnsDetector"};

/**
 * Checks if LNS state is LNC.
 *
 * @param lns LNS value
 *
 * @return true if LNC, false otherwise
 */
constexpr bool IsLNC(const LNState lns) { return lns == LNState::lnc; }
/**
 * Checks if LNS state is LNC.
 *
 * @param lns LNS value
 *
 * @return true if LNC, false otherwise
 */
constexpr bool IsLNCu(const ushort lns) { return IsLNC(static_cast<LNState>(lns)); }

/**
 * @brief Checks if LNS state is LNV.
 *
 * @param lns LNS value
 *
 * @return true if LNV, false otherwise
 */
constexpr bool IsLNV(const LNState lns) { return lns == LNState::lnv; }
/**
 * @brief Checks if LNS state is LNV.
 *
 * @param lns LNS value
 *
 * @return true if LNV, false otherwise
 */
constexpr bool IsLNVu(const ushort lns) { return IsLNV(static_cast<LNState>(lns)); }

/**
 * @brief Determines LNS state from particle identifiers.
 *
 * Uses the sign of the product of PDG-like identifiers:
 * - positive product → LNV
 * - negative product → LNC
 *
 * @param promptPID Prompt particle ID
 * @param displacedPID Displaced particle ID
 *
 * @return LNS classification
 */
inline LNState DetermineLNS(const int promptPID, const int displacedPID) {
  return std::signbit(promptPID * displacedPID) ? HNL::Common::LNState::lnc
                                                : HNL::Common::LNState::lnv;
}
///@}

///@name Oscillation damping
///@{
/**
 * @brief Computes pseudo-Dirac mass splitting threshold Δm₀.
 *
 * Based on equation (2.26) of LN-violation at FCC-hh study.
 *
 * @param ipProtonDistance Proton distance at interaction point (nm)
 *
 * @return Mass splitting threshold Δm₀ in eV
 */
constexpr double DampingMassScale(const double ipProtonDistance) {
  return 100 / ipProtonDistance * 7.757e-9;
}

/**
 * @brief Computes effective damping parameter λ.
 *
 * Based on equation (2.26) of LN-violation at FCC-hh study.
 *
 * @param massSplitting Pseudo-Dirac HNL mass splitting (GeV)
 * @param massSplittingThreshold Characteristic mass splitting scale Δm₀ used for normalization.
 * Defaults to the FCC-hh value.
 *
 * @return Dimensionless damping parameter λ
 */
constexpr double Damping(const double massSplitting, const double massSplittingThreshold) {
  const double x{massSplitting / massSplittingThreshold};
  return x * x;
}
///@}

///@name Ratio of LNV to LNC Events
///@{
/**
 * @brief Computes LNV/LNC event ratio without damping effects.
 *
 * @param decayWidth Pseudo-Dirac HNL decay width Γ
 * @param massSplitting Pseudo-Dirac mass splitting Δm
 *
 * @return Ratio of LNV to LNC events
 */
constexpr double LNV2LNCRatio(const double decayWidth, const double massSplitting) {
  const double massSplitting2{massSplitting * massSplitting};
  const double decayWidth2{decayWidth * decayWidth};
  return massSplitting2 / (massSplitting2 + (2 * decayWidth2));
}

/**
 * @brief Computes LNV/LNC event ratio including damping effects.
 *
 * @param decayWidth Decay width Γ
 * @param massSplitting Mass splitting Δm
 * @param damping Effective damping parameter λ
 *
 * @return Ratio of LNV to LNC events
 */
constexpr double LNV2LNCRatio(const double decayWidth, const double massSplitting,
                              const double damping) {
  const double massSplitting2{massSplitting * massSplitting};
  const double decayWidth2{decayWidth * decayWidth};
  const double denominator{1 + ((1 + (massSplitting2 / decayWidth2)) * std::exp(damping))};
  return 1 - (2 / denominator);
}

/**
 * @brief Computes decay width from observed LNV/LNC ratio.
 *
 * @param rll Observed LNV/LNC ratio
 * @param massSplitting Mass splitting Δm
 *
 * @return Decay width Γ
 */
constexpr double DecayWidthFromR(const double rll, const double massSplitting) {
  return std::sqrt(2. / rll * (1 - rll)) * massSplitting;
}

/**
 * @brief Computes mass splitting from observed LNV/LNC ratio.
 *
 * @param rll Observed LNV/LNC ratio
 * @param decayWidth Decay width Γ
 *
 * @return Mass splitting Δm
 */
constexpr double MassSplittingFromR(const double rll, const double decayWidth) {
  return std::sqrt((2. * decayWidth * decayWidth * rll) / (1 - rll));
}
///@}

///@name Ratio of Dirac/Majorana distribution
///@{
/**
 * @brief Computes star ratio without damping.
 *
 * @param decayWidth Decay width Γ
 * @param massSplitting Mass splitting Δm
 *
 * @return Star ratio
 */
constexpr double StarRatio(const double decayWidth, const double massSplitting) {
  const double massSplitting2{massSplitting * massSplitting};
  const double decayWidth2{decayWidth * decayWidth};
  return massSplitting2 / (massSplitting2 + decayWidth2);
}

/**
 * @brief Computes star ratio including damping effects.
 *
 * @param decayWidth Decay width Γ
 * @param massSplitting Mass splitting Δm
 * @param damping Effective damping λ
 *
 * @return Star ratio
 */
constexpr double StarRatio(const double decayWidth, const double massSplitting,
                           const double damping) {
  const double massSplitting2{massSplitting * massSplitting};
  const double decayWidth2{decayWidth * decayWidth};
  return decayWidth2 / (decayWidth2 + massSplitting2) * std::exp(-damping);
}
///@}

///@name Simulation of Heavy Neutrino Antineutrino Oscillations
///@{
/**
 * @brief Oscillation amplitude for pseudo-Dirac HNL.
 *
 * @param massSplitting Δm
 * @param damping λ
 * @param properLifetime Proper decay time τ
 *
 * @return Oscillation amplitude
 */
constexpr double OscillationAmplitude(double massSplitting, double damping, double properLifetime) {
  return .5 * exp(-damping) * cos(massSplitting * properLifetime / Constants::hbar_GeVs);
}

/**
 * @brief Probability of LNC event.
 *
 * @param massSplitting Δm
 * @param damping λ
 * @param properLifetime τ
 *
 * @return Probability of LNC
 */
constexpr double LNCProbability(double massSplitting, double damping, double properLifetime) {
  return .5 + OscillationAmplitude(massSplitting, damping, properLifetime);
}

/**
 * @brief Probability of LNV event.
 *
 * @param massSplitting Δm
 * @param damping λ
 * @param properLifetime τ
 *
 * @return Probability of LNV
 */
constexpr double LNVProbability(double massSplitting, double damping, double properLifetime) {
  return .5 - OscillationAmplitude(massSplitting, damping, properLifetime);
}

/**
 * @brief Determines whether an event should be included in analysis.
 *
 * @param lns LNS state
 * @param properLifetime Proper decay time
 * @param massSplitting Δm
 * @param damping λ
 * @param rngLikelihood Random acceptance threshold
 *
 * @return true if event is accepted
 */
bool IncludeEvent(LNState lns, double properLifetime, double massSplitting, double damping,
                  double rngLikelihood);
/**
 * @brief Determines whether an event should be included in analysis from an integral LNS tag.
 *
 * This overload forwards to the `LNState` version after converting the input tag.
 *
 * @param lns Integral representation of the LNS state.
 * @param properLifetime Proper decay time.
 * @param massSplitting Δm.
 * @param damping λ.
 * @param rngLikelihood Random acceptance threshold.
 *
 * @return true if event is accepted.
 */
inline bool IncludeEvent(const ushort lns, const double properLifetime, const double massSplitting,
                         const double damping, const double rngLikelihood) {
  return IncludeEvent(static_cast<LNState>(lns), properLifetime, massSplitting, damping,
                      rngLikelihood);
};
///@}
}  // namespace HNL::Common

/**
 * @brief Formats `HNL::Common::LNState` values through `std::format`.
 */
template <>
struct std::formatter<enum HNL::Common::LNState> {
  /**
   * @brief Accepts the default formatter syntax.
   *
   * @param ctx Format parse context.
   *
   * @return Iterator to the current parse position.
   */
  constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

  /**
   * @brief Writes the string representation of an `LNState` value.
   *
   * @tparam FormatContext Format context type.
   * @param lns LNS value to format.
   * @param ctx Format context.
   *
   * @return Iterator to the end of the formatted output.
   */
  template <typename FormatContext>
  auto format(const HNL::Common::LNState &lns, FormatContext &ctx) {
    return std::format_to(ctx.out(), "{}", ToString(lns));
  }
};
