// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

// HNL
#include "DelphesWrapper/TreeNavigation.hpp"

namespace HNL::DimuonSS::FCChh {
/** @brief Generator-level particle categories used in SS dimuon FCC-hh studies. */
enum class GeneratorID : uint8_t {
  initQuark,
  hnl,
  leptonProd,
  leptonDecay,
  quarkP,
  quarkA,
  leptonOther,
  quarkOther,
  lightNeutrino,
  other,
  error
};

/** @brief Convert a SS dimuon generator category into a display label. */
constexpr std::string_view ToString(const GeneratorID identity) {
  switch (identity) {
    case GeneratorID::initQuark:
      return "Quark (Initial)";
    case GeneratorID::hnl:
      return "HNL";
    case GeneratorID::leptonProd:
      return "Lepton (Production)";
    case GeneratorID::leptonDecay:
      return "Lepton (Decay)";
    case GeneratorID::quarkP:
      return "Quark (Decay)";
    case GeneratorID::quarkA:
      return "Anti-Quark (Decay)";
    case GeneratorID::leptonOther:
      return "Lepton (Unknown source)";
    case GeneratorID::quarkOther:
      return "Quark (Unknown source)";
    case GeneratorID::lightNeutrino:
      return "Light Neuntrino (Unknown source)";
    case GeneratorID::other:
      return "Other";
    case GeneratorID::error:
      return "Error";
  }

  return "Error";
}

/** @brief Classify a particle in the Delphes truth tree for SS dimuon studies. */
GeneratorID IdentifyParticle(const HNL::Delphes::ParticleTreeWalker& ptw, size_t idx,
                             bool afterPythia);
};  // namespace HNL::DimuonSS::FCChh
