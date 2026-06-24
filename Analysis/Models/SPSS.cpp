// Copyright 2023-2026 Bruno M. S. Oliveira

// Standard Library
#include <string>

// HNL
#include "Common/Log.hpp"
#include "Common/Oscillations.hpp"
#include "MCPID/pSPSS.hpp"
#include "Models/SPSS.hpp"

namespace HNL::Model {
SPSS::SPSS(const MG5_aMC::SLHAReader &slhaReader, const double massSplitting, const double damping)
    : Generic{ModelID::SPSS} {
  const double mn4{slhaReader.get_block_entry("mass", PID::pSPSS::n4)};
  const double mn5{slhaReader.get_block_entry("mass", PID::pSPSS::n5)};
  if (mn4 != mn5) {
    LogError(
        "SPSS HNLs have inconsistent masses:"
        "\n\tm(n1) = {} GeV"
        "\n\tm(n2) = {} GeV",
        mn4, mn5);
    exit(1);
  }

  meanMass = mn4;

  ve = slhaReader.get_block_entry("neutrinos", 1);
  vm = slhaReader.get_block_entry("neutrinos", 2);
  vt = slhaReader.get_block_entry("neutrinos", 3);
  thetaSquared = BaselineThetaSquared();

  fe = slhaReader.get_block_entry("neutrinos", 4);
  fm = slhaReader.get_block_entry("neutrinos", 5);
  ft = slhaReader.get_block_entry("neutrinos", 6);

  if (ThetaSquared() == 0) {
    ve = slhaReader.get_block_entry("pmns", 1);
    vm = slhaReader.get_block_entry("pmns", 2);
    vt = slhaReader.get_block_entry("pmns", 3);

    fe = slhaReader.get_block_entry("pmns", 4);
    fm = slhaReader.get_block_entry("pmns", 5);
    ft = slhaReader.get_block_entry("pmns", 6);
  }

  n4Width = slhaReader.get_block_entry("decay", PID::pSPSS::n4);
  n5Width = slhaReader.get_block_entry("decay", PID::pSPSS::n5);
  meanWidth = BaselineMeanWidth();

  this->massSplitting = massSplitting;
  this->damping = damping;
  FixInverseSeesawMass();
};

SPSS::SPSS(const std::vector<double> &parametersVector) : Generic{ModelID::SPSS} {
  meanMass = parametersVector.at(0);

  ve = parametersVector.at(1);
  vm = parametersVector.at(2);
  vt = parametersVector.at(3);
  thetaSquared = BaselineThetaSquared();

  fe = parametersVector.at(4);
  fm = parametersVector.at(5);
  ft = parametersVector.at(6);

  n4Width = parametersVector.at(7);
  n5Width = parametersVector.at(8);
  meanWidth = BaselineMeanWidth();

  massSplitting = parametersVector.at(9);
  damping = parametersVector.at(10);
  FixInverseSeesawMass();
};

SPSS &SPSS::operator=(const std::vector<double> &parametersVector) {
  meanMass = parametersVector.at(0);

  ve = parametersVector.at(1);
  vm = parametersVector.at(2);
  vt = parametersVector.at(3);
  thetaSquared = BaselineThetaSquared();

  fe = parametersVector.at(4);
  fm = parametersVector.at(5);
  ft = parametersVector.at(6);

  n4Width = parametersVector.at(7);
  n5Width = parametersVector.at(8);
  meanWidth = BaselineMeanWidth();

  massSplitting = parametersVector.at(9);
  damping = parametersVector.at(10);
  FixInverseSeesawMass();

  return *this;
};

SPSS &SPSS::operator=(const std::vector<double> &&parametersVector) {
  meanMass = parametersVector.at(0);

  ve = parametersVector.at(1);
  vm = parametersVector.at(2);
  vt = parametersVector.at(3);
  thetaSquared = BaselineThetaSquared();

  fe = parametersVector.at(4);
  fm = parametersVector.at(5);
  ft = parametersVector.at(6);

  n4Width = parametersVector.at(7);
  n5Width = parametersVector.at(8);
  meanWidth = BaselineMeanWidth();

  massSplitting = parametersVector.at(9);
  damping = parametersVector.at(10);

  FixInverseSeesawMass();

  return *this;
};

std::string SPSS::CSV() const {
  return std::format("{},{},{},{},{},{},{}", meanMass, ThetaSquared(), MeanWidth(), massSplitting,
                     damping, LNV2LNCRatio(), inverseSeesawMass);
}

std::vector<double> SPSS::Vector() const {
  return {meanMass, ve, fe, vm, fm, vt, ft, n4Width, n5Width, massSplitting, damping};
}

void SPSS::SetThetaSquared(const double thetaSquared, const double crossSectionEnhancement) {
  if (thetaSquared == ThetaSquared()) return;

  const double thetaSquared0{BaselineThetaSquared()};
  const double thRatio{thetaSquared / thetaSquared0};

  const double sv0{std::sqrt(1 + BaselineThetaSquared())};
  const double sv1{std::sqrt(1 + thetaSquared)};
  const double svRatio{sv1 / sv0};

  const double widthFactor{thRatio / std::pow(svRatio, 2.5)};

  this->thetaSquared = thetaSquared;
  this->meanWidth = widthFactor * BaselineMeanWidth();
  this->xsectionEnhancement = crossSectionEnhancement;
}

void SPSS::SetMassSplitting(const double massSplitting, const double massSplittingThreshold) {
  this->massSplitting = massSplitting;
  if (massSplittingThreshold != 0) FixDamping(massSplittingThreshold);

  FixInverseSeesawMass();
}

void SPSS::SetInverseSeesawMass(const double inverseSeesawMass,
                                const double massSplittingThreshold) {
  this->inverseSeesawMass = inverseSeesawMass;

  FixMassSplitting();
  if (massSplittingThreshold != 0) FixDamping(massSplittingThreshold);
}

double SPSS::OscillationLength() const {
  return massSplitting / (HNL::Constants::hbar_GeVs * HNL::Constants::speedOfLight);
}

double SPSS::DampingLength() const {
  return damping / (HNL::Constants::hbar_GeVs * HNL::Constants::speedOfLight);
}

double SPSS::MeanDecayConstant() const {
  return MeanWidth() / (HNL::Constants::hbar_GeVs * HNL::Constants::speedOfLight);
}

double SPSS::LNV2LNCRatio() const {
  if (massSplitting <= 0 && damping <= 0) return 1;

  if (damping == 0) {
    return HNL::Common::LNV2LNCRatio(MeanWidth(), massSplitting);
  }

  return HNL::Common::LNV2LNCRatio(MeanWidth(), massSplitting, damping);
}

double SPSS::LNCFraction() const {
  const double lnv2lncRatio{LNV2LNCRatio()};
  return 1 / (1 + lnv2lncRatio);
}

double SPSS::LNVFraction() const {
  const double lnv2lncRatio{LNV2LNCRatio()};
  return lnv2lncRatio / (1 + lnv2lncRatio);
}

[[nodiscard]] double SPSS::BaselineThetaSquared() const {
  return (ve * ve) + (vm * vm) + (vt * vt);
}

[[nodiscard]] double SPSS::BaselineMeanWidth() const { return (n4Width + n5Width) / 2; }

void SPSS::FixMassSplitting() { massSplitting = inverseSeesawMass / ThetaSquared(); }

void SPSS::FixInverseSeesawMass() { inverseSeesawMass = ThetaSquared() * massSplitting; }

void SPSS::FixDamping(const double massSplittingThreshold) {
  damping = Common::Damping(massSplitting, massSplittingThreshold);
}

std::istream &operator>>(std::istream &in, SPSS::Scaling &tss) {
  char sep{};
  in >> tss.thetaSquared >> sep >> tss.scalingFactor;
  if (!in || sep != ':' || tss.thetaSquared < 0 || tss.scalingFactor < 0) {
    in.setstate(std::ios::failbit);
  }

  return in;
}
}  // namespace HNL::Model
