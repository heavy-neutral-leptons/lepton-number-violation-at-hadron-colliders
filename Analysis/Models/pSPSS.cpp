// Copyright 2023-2026 Bruno M. S. Oliveira

// Standard Library
#include <string>

// HNL
#include "Common/Log.hpp"
#include "Common/Oscillations.hpp"
#include "MCPID/pSPSS.hpp"
#include "Models/pSPSS.hpp"

namespace HNL::Model {
pSPSS::pSPSS(const double deltam, const double damping)
    : Generic{ModelID::pSPSS}, imposeOscillations{deltam > 0 && damping >= 0} {}

pSPSS::pSPSS(const MG5_aMC::SLHAReader &slhaReader, const double deltam, const double damping)
    : pSPSS{deltam, damping} {
  meanMass = slhaReader.get_block_entry("pspss", 1);
  theta1 = slhaReader.get_block_entry("pspss", 3);
  theta2 = slhaReader.get_block_entry("pspss", 4);
  theta3 = slhaReader.get_block_entry("pspss", 5);

  n4Width = slhaReader.get_block_entry("decay", PID::pSPSS::n4, -1);
  n5Width = slhaReader.get_block_entry("decay", PID::pSPSS::n5, -1);

  const double slahMassSplitting{slhaReader.get_block_entry("pspss", 2)};
  if (!imposeOscillations) {
    this->deltam = slahMassSplitting;
    this->damping = slhaReader.get_block_entry("pspss", 6);
  }

  const double mn4{slhaReader.get_block_entry("mass", PID::pSPSS::n4)};
  const double mn5{slhaReader.get_block_entry("mass", PID::pSPSS::n5)};
  if (mn4 != meanMass - (slahMassSplitting / 2) || mn5 != meanMass + (slahMassSplitting / 2)) {
    LogError(
        "pSPSS HNLs have inconsistent masses:"
        "\n\tmN = {} GeV, Δm = {} GeV"
        "\n\tm(n4) = {} GeV"
        "\n\tm(n5) = {} GeV",
        meanMass, deltam, mn4, mn5);
    exit(1);
  }

  FixInverseSeesawMass();
};

pSPSS::pSPSS(const std::vector<double> &parametersVector, const double deltam, const double damping)
    : pSPSS{deltam, damping} {
  meanMass = parametersVector.at(0);
  theta1 = parametersVector.at(2);
  theta2 = parametersVector.at(3);
  theta3 = parametersVector.at(4);

  n4Width = parametersVector.at(6);
  n5Width = parametersVector.at(7);

  if (imposeOscillations) {
    this->deltam = deltam;
    this->damping = damping;
  } else {
    this->deltam = parametersVector.at(1);
    this->damping = parametersVector.at(5);
  }

  FixInverseSeesawMass();
};

pSPSS &pSPSS::operator=(const std::vector<double> &parametersVector) {
  meanMass = parametersVector.at(0);
  theta1 = parametersVector.at(2);
  theta2 = parametersVector.at(3);
  theta3 = parametersVector.at(4);

  n4Width = parametersVector.at(6);
  n5Width = parametersVector.at(7);

  if (!imposeOscillations) {
    this->deltam = parametersVector.at(1);
    this->damping = parametersVector.at(5);
  }

  FixInverseSeesawMass();

  return *this;
};

pSPSS &pSPSS::operator=(const std::vector<double> &&parametersVector) {
  meanMass = parametersVector.at(0);
  theta1 = parametersVector.at(2);
  theta2 = parametersVector.at(3);
  theta3 = parametersVector.at(4);

  n4Width = parametersVector.at(6);
  n5Width = parametersVector.at(7);

  if (!imposeOscillations) {
    this->deltam = parametersVector.at(1);
    this->damping = parametersVector.at(5);
  }

  FixInverseSeesawMass();

  return *this;
};

std::string pSPSS::CSV() const {
  return std::format("{},{},{},{},{},{},{}", meanMass, ThetaSquared(), MeanWidth(), deltam, damping,
                     LNV2LNCRatio(), inverseSeesawMass);
}

std::vector<double> pSPSS::Vector() const {
  return {meanMass, deltam, theta1, theta2, theta3, damping, n4Width, n5Width};
}

void pSPSS::SetMassSplitting(const double massSplitting, const bool fixDamping) {
  this->deltam = massSplitting;
  if (fixDamping) FixDamping();

  FixInverseSeesawMass();
}

void pSPSS::SetInverseSeesawMass(const double inverseSeesawMass, const bool fixDamping) {
  this->inverseSeesawMass = inverseSeesawMass;

  FixMassSplitting();
  if (fixDamping) FixDamping();
}

double pSPSS::ThetaSquared() const {
  return (theta1 * theta1) + (theta2 * theta2) + (theta3 * theta3);
}

double pSPSS::OscillationLength() const {
  return deltam / (HNL::Constants::hbar_GeVs * HNL::Constants::speedOfLight);
}

double pSPSS::DampingLength() const {
  return damping / (HNL::Constants::hbar_GeVs * HNL::Constants::speedOfLight);
}

double pSPSS::MeanWidth() const { return (n4Width + n5Width) / 2; }

double pSPSS::MeanDecayConstant() const {
  return MeanWidth() / (HNL::Constants::hbar_GeVs * HNL::Constants::speedOfLight);
}

double pSPSS::LNV2LNCRatio() const {
  if (deltam <= 0 && damping <= 0) return 1;

  if (damping == 0) return HNL::Common::LNV2LNCRatio(MeanWidth(), deltam);

  return HNL::Common::LNV2LNCRatio(MeanWidth(), deltam, damping);
}

double pSPSS::LNCFraction() const {
  const double lnv2lncRatio{LNV2LNCRatio()};
  return 1 / (1 + lnv2lncRatio);
}

double pSPSS::LNVFraction() const {
  const double lnv2lncRatio{LNV2LNCRatio()};
  return lnv2lncRatio / (1 + lnv2lncRatio);
}

void pSPSS::FixMassSplitting() { deltam = inverseSeesawMass / ThetaSquared(); }

void pSPSS::FixInverseSeesawMass() { inverseSeesawMass = ThetaSquared() * deltam; }

void pSPSS::FixDamping() { exit(1); }
}  // namespace HNL::Model
