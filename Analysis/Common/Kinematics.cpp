// Copyright 2023-2026 Bruno M. S. Oliveira

// Standard Library
#include <cmath>
#include <utility>

// ROOT
#include "Math/Vector4D.h"

// HNL
#include "Common/Kinematics.hpp"
#include "Constants/Physics.hpp"

namespace HNL::Common {
/* ----- Proper Lifetime Calculations ----- */
double ProperLifetime(const double displacement, const ROOT::Math::PtEtaPhiMVector &momentum) {
  const double beta{momentum.Beta()};

  const double gamma{momentum.Gamma()};

  const double velocity{beta * Constants::speedOfLight};

  const double lifetime{displacement / velocity};

  const double proper_lifetime{lifetime / gamma};

  return proper_lifetime;
}

std::pair<double, double> ProperLifetime(const double displacement,
                                         const ROOT::Math::PtEtaPhiMVector &momentum,
                                         const double displacement_error,
                                         const double velocity_relerror) {
  const double beta{momentum.Beta()};
  const double beta_error{velocity_relerror * beta};

  const double gamma{momentum.Gamma()};
  const double gamma_error{beta / (std::pow(1 - (beta * beta), 1.5)) * beta_error};

  const double velocity{beta * Constants::speedOfLight};
  const double velocity_error{beta_error * Constants::speedOfLight};

  const double lifetime{displacement / velocity};
  const double lifetime_error{
      std::sqrt(std::pow(displacement_error / velocity, 2) +
                std::pow(displacement / (velocity * velocity) * velocity_error, 2))};

  const double proper_lifetime{lifetime / gamma};
  const double proper_lifetime_error{std::sqrt(
      std::pow(lifetime_error / gamma, 2) + std::pow(lifetime / (gamma * gamma) * gamma_error, 2))};

  return {proper_lifetime, proper_lifetime_error / 10};
}

double ProperLifetime(const double displacement, const double gamma) {
  const double beta{Beta(gamma)};

  const double velocity{beta * Constants::speedOfLight};

  const double lifetime{displacement / velocity};

  const double proper_lifetime{lifetime / gamma};

  return proper_lifetime;
}

std::pair<double, double> ProperLifetime(const double displacement, const double gamma,
                                         const double displacement_error) {
  const double beta{Beta(gamma)};

  const double velocity{beta * Constants::speedOfLight};

  const double lifetime{displacement / velocity};
  const double lifetime_error{displacement_error / velocity};

  const double proper_lifetime{lifetime / gamma};
  const double proper_lifetime_error{lifetime_error / gamma};

  return {proper_lifetime, proper_lifetime_error / 10};
}

/* ----- PST Observables ----- */
double InvariantMass(const ROOT::Math::PtEtaPhiMVector &p1, const ROOT::Math::PtEtaPhiMVector &p2) {
  return (p1 + p2).M();
}

double InvariantMass(const ROOT::Math::PtEtaPhiMVector &p1, const ROOT::Math::PtEtaPhiMVector &p2,
                     const ROOT::Math::PtEtaPhiMVector &p3) {
  return (p1 + p2 + p3).M();
}

double InvariantMass(const ROOT::Math::PtEtaPhiMVector &p1, const ROOT::Math::PtEtaPhiMVector &p2,
                     const ROOT::Math::PtEtaPhiMVector &p3, const ROOT::Math::PtEtaPhiMVector &p4) {
  return (p1 + p2 + p3 + p4).M();
}

double PSTBigG(const double m1, const double m2, const double m12) {
  return ((m12 * m12) - (m1 * m1) - (m2 * m2)) / 2;
}

double PSTBigV(const double m1, const double m2, const double m12) {
  return 2 * std::sqrt(PSTSmallT(m1, m2, m12) * PSTSmallT(m2, m12, m1) * PSTSmallT(m12, m1, m2) *
                       PSTSmallS(m1, m2, m12));
}

double PSTSmallT(const double ma, const double mb, const double mc) {
  return ma - PSTSmallS(ma, mb, mc);
}

double PSTSmallS(const double ma, const double mb, const double mc) { return (ma + mb + mc) / 2; }
}  // namespace HNL::Common
