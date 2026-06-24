// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

// Standard Library
#include <cmath>
#include <utility>

// ROOT
#include "Constants/Physics.hpp"
#include "Math/Vector4Dfwd.h"

namespace HNL::Common {
///@name Relativistic kinematics
///@{
/**
 * @brief Computes the relativistic velocity fraction \f$\beta = v/c\f$ from \f$\gamma\f$.
 *
 * @param gamma Lorentz factor.
 *
 * @return Dimensionless velocity fraction.
 */
constexpr double Beta(const double gamma) { return std::sqrt(1 - (1 / (gamma * gamma))); }

/**
 * @brief Computes \f$\beta^2\f$ from the Lorentz factor.
 *
 * @param gamma Lorentz factor.
 *
 * @return Squared velocity fraction.
 */
constexpr double BetaSquared(const double gamma) { return 1 - (1 / (gamma * gamma)); }

/**
 * @brief Computes the Lorentz factor \f$\gamma\f$ from \f$\beta\f$.
 *
 * @param beta Dimensionless velocity fraction.
 *
 * @return Lorentz factor.
 */
constexpr double Gamma(const double beta) { return std::sqrt(1 / (1 - (beta * beta))); }

/**
 * @brief Computes \f$\gamma^2\f$ from \f$\beta\f$.
 *
 * @param beta Dimensionless velocity fraction.
 *
 * @return Squared Lorentz factor.
 */
constexpr double GammaSquared(const double beta) { return 1 / (1 - (beta * beta)); }
///@}

///@name Proper lifetime calculations
///@{
/**
 * @brief Computes the proper lifetime from a displacement and reconstructed four-momentum.
 *
 * The displacement is converted to the lab-frame lifetime using the particle velocity, then divided
 * by the Lorentz factor to obtain the proper lifetime.
 *
 * @param displacement Decay displacement in meters.
 * @param momentum Particle four-momentum.
 *
 * @return Proper lifetime in seconds.
 */
double ProperLifetime(double displacement, const ROOT::Math::PtEtaPhiMVector &momentum);

/**
 * @brief Computes the proper lifetime and propagated uncertainty from displacement and momentum.
 *
 * @param displacement Decay displacement in meters.
 * @param momentum Particle four-momentum.
 * @param displacement_error Absolute displacement uncertainty in meters.
 * @param velocity_relerror Relative uncertainty on the velocity fraction.
 *
 * @return Pair containing the proper lifetime and its uncertainty, both in seconds.
 */
std::pair<double, double> ProperLifetime(double displacement,
                                         const ROOT::Math::PtEtaPhiMVector &momentum,
                                         double displacement_error, double velocity_relerror);

/**
 * @brief Computes the proper lifetime from a displacement and Lorentz factor.
 *
 * @param displacement Decay displacement in meters.
 * @param gamma Lorentz factor.
 *
 * @return Proper lifetime in seconds.
 */
double ProperLifetime(double displacement, double gamma);

/**
 * @brief Computes the proper lifetime and propagated uncertainty from displacement and Lorentz
 * factor.
 *
 * @param displacement Decay displacement in meters.
 * @param gamma Lorentz factor.
 * @param displacement_error Absolute displacement uncertainty in meters.
 *
 * @return Pair containing the proper lifetime and its uncertainty, both in seconds.
 */
std::pair<double, double> ProperLifetime(double displacement, double gamma,
                                         double displacement_error);

/**
 * @brief Converts a proper lifetime into a proper decay length.
 *
 * @param proper_lifetime Proper lifetime in seconds.
 *
 * @return Proper decay length in meters.
 */
constexpr double ProperLength(const double proper_lifetime) {
  return Constants::speedOfLight * proper_lifetime;
}
///@}

///@name Two-body kinematics
///@{
/**
 * @brief Computes the energy of the first daughter in a two-body system.
 *
 * @param energy Total center-of-mass energy.
 * @param mass1 Mass of the first daughter.
 * @param mass2 Mass of the second daughter.
 *
 * @return Energy of the first daughter.
 */
constexpr double TwoBodyEnergy(const double energy, const double mass1, const double mass2) {
  return ((energy * energy) + (mass1 * mass1) - (mass2 * mass2)) / (2. * energy);
}

/**
 * @brief Computes the squared momentum of the first daughter in a two-body system.
 *
 * @param energy Total center-of-mass energy.
 * @param mass1 Mass of the first daughter.
 * @param mass2 Mass of the second daughter.
 *
 * @return Squared three-momentum of the first daughter.
 */
constexpr double TwoBodyMomentumSquared(const double energy, const double mass1,
                                        const double mass2) {
  const double energy1{TwoBodyEnergy(energy, mass1, mass2)};

  return (energy1 * energy1) - (mass1 * mass1);
}

/**
 * @brief Computes the momentum magnitude of the first daughter in a two-body system.
 *
 * @param energy Total center-of-mass energy.
 * @param mass1 Mass of the first daughter.
 * @param mass2 Mass of the second daughter.
 *
 * @return Three-momentum magnitude of the first daughter.
 */
constexpr double TwoBodyMomentum(const double energy, const double mass1, const double mass2) {
  return std::sqrt(TwoBodyMomentumSquared(energy, mass1, mass2));
}

/**
 * @brief Computes the Lorentz factor of the first daughter in a two-body system.
 *
 * @param energy Total center-of-mass energy.
 * @param mass1 Mass of the first daughter.
 * @param mass2 Mass of the second daughter.
 *
 * @return Lorentz factor of the first daughter.
 */
constexpr double TwoBodyGamma(const double energy, const double mass1, const double mass2) {
  return TwoBodyEnergy(energy, mass1, mass2) / mass1;
}

/**
 * @brief Computes \f$\beta^2\f$ for the first daughter in a two-body system.
 *
 * @param energy Total center-of-mass energy.
 * @param mass1 Mass of the first daughter.
 * @param mass2 Mass of the second daughter.
 *
 * @return Squared velocity fraction of the first daughter.
 */
constexpr double TwoBodyBetaSquared(const double energy, const double mass1, const double mass2) {
  return BetaSquared(TwoBodyGamma(energy, mass1, mass2));
}

/**
 * @brief Computes \f$\beta\f$ for the first daughter in a two-body system.
 *
 * @param energy Total center-of-mass energy.
 * @param mass1 Mass of the first daughter.
 * @param mass2 Mass of the second daughter.
 *
 * @return Velocity fraction of the first daughter.
 */
constexpr double TwoBodyBeta(const double energy, const double mass1, const double mass2) {
  return Beta(TwoBodyGamma(energy, mass1, mass2));
}
///@}

///@name Invariant masses
///@{
/**
 * @brief Computes the invariant mass of two four-momenta.
 *
 * @param p1 First four-momentum.
 * @param p2 Second four-momentum.
 *
 * @return Invariant mass of the combined system.
 */
double InvariantMass(const ROOT::Math::PtEtaPhiMVector &p1, const ROOT::Math::PtEtaPhiMVector &p2);
/**
 * @brief Computes the invariant mass of three four-momenta.
 *
 * @param p1 First four-momentum.
 * @param p2 Second four-momentum.
 * @param p3 Third four-momentum.
 *
 * @return Invariant mass of the combined system.
 */
double InvariantMass(const ROOT::Math::PtEtaPhiMVector &p1, const ROOT::Math::PtEtaPhiMVector &p2,
                     const ROOT::Math::PtEtaPhiMVector &p3);
/**
 * @brief Computes the invariant mass of four four-momenta.
 *
 * @param p1 First four-momentum.
 * @param p2 Second four-momentum.
 * @param p3 Third four-momentum.
 * @param p4 Fourth four-momentum.
 *
 * @return Invariant mass of the combined system.
 */
double InvariantMass(const ROOT::Math::PtEtaPhiMVector &p1, const ROOT::Math::PtEtaPhiMVector &p2,
                     const ROOT::Math::PtEtaPhiMVector &p3, const ROOT::Math::PtEtaPhiMVector &p4);

///@}

///@name PST observables
///@brief Auxiliary observables defined in arXiv:2505.17903.
///@{
/**
 * @brief Computes the auxiliary observable \f$G\f$ from three masses.
 *
 * @param m1 First mass.
 * @param m2 Second mass.
 * @param m12 Combined invariant mass.
 *
 * @return Value of the \f$G\f$ observable.
 */
double PSTBigG(double m1, double m2, double m12);
/**
 * @brief Computes the auxiliary observable \f$V\f$ from three masses.
 *
 * @param m1 First mass.
 * @param m2 Second mass.
 * @param m12 Combined invariant mass.
 *
 * @return Value of the \f$V\f$ observable.
 */
double PSTBigV(double m1, double m2, double m12);
/**
 * @brief Computes the auxiliary observable \f$T\f$ from three masses.
 *
 * @param ma First mass.
 * @param mb Second mass.
 * @param mc Third mass.
 *
 * @return Value of the \f$T\f$ observable.
 */
double PSTSmallT(double ma, double mb, double mc);
/**
 * @brief Computes the auxiliary observable \f$S\f$ from three masses.
 *
 * @param ma First mass.
 * @param mb Second mass.
 * @param mc Third mass.
 *
 * @return Value of the \f$S\f$ observable.
 */
double PSTSmallS(double ma, double mb, double mc);
///@}
}  // namespace HNL::Common
