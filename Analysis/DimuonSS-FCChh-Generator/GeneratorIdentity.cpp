// Copyright 2023-2026 Bruno M. S. Oliveira

// Delphes
#include "classes/DelphesClasses.h"

// HNL
#include "Common/Log.hpp"
#include "Common/PythiaStatus.hpp"
#include "MCPID/SM.hpp"
#include "MCPID/SPSS.hpp"
#include "DimuonSS-FCChh-Generator/GeneratorIdentity.hpp"

namespace HNL::DimuonSS::FCChh {
GeneratorID IdentifyParticle(const HNL::Delphes::ParticleTreeWalker& ptw, const size_t idx,
                             const bool afterPythia) {
  using enum GeneratorID;

  const GenParticle* const particle{ptw.At(idx)};

  Log<1>("Particle {} ({}): {}", idx, particle->PID, particle->Status);

  const HNL::Delphes::ParticleFamily family{ptw.Family(idx)};
  Log<1>("\tMothers   : {} {}", particle->M1, particle->M2);
  if (family.Produced()) Log<1>("\t{}", family.Production());
  Log<1>("\tDaughters : {} {}", particle->D1, particle->D2);
  if (family.Decayed()) Log<1>("\t{}", family.Decay());

  if (HNL::Pythia::IsHardIncoming(particle->Status, afterPythia)) {
    return initQuark;
  }

  switch (std::abs(particle->PID)) {
    // HNLs
    case HNL::PID::SPSS::n1:
    case HNL::PID::SPSS::n2: {
      if (HNL::Pythia::IsHardIntermediate(particle->Status, afterPythia)) return hnl;

      return other;
    }

    // SM Neutrinos
    case HNL::PID::SM::velectron:
    case HNL::PID::SM::vmuon:
    case HNL::PID::SM::tau: {
      return lightNeutrino;
    }

    // SM Charged Leptons
    case HNL::PID::SM::muon: {
      if (family.NotProduced()) return leptonOther;

      // Look into mother
      const GenParticle* const mother{family.Mothers().front()};

      if (HNL::Pythia::IsHardIncoming(mother->Status, afterPythia)) return leptonProd;
      if (HNL::PID::SPSS::IsHNL(mother->PID)) return leptonDecay;

      // If mother is W, look into gradmother
      if (!HNL::PID::SM::IsW(mother->PID)) return leptonOther;

      Log<1>("\tMothers²  : {} {}", mother->M1, mother->M2);
      const auto mothers2{ptw.Mother(*mother)};

      if (mothers2.empty()) return leptonOther;
      const GenParticle* const mother2{mothers2.front()};

      if (HNL::Pythia::IsHardIncoming(mother2->Status, afterPythia)) return leptonProd;
      if (HNL::PID::SPSS::IsHNL(mother2->PID)) return leptonDecay;

      // As a last resort, look into sister
      if (family.SisterIdxs().size() == 1) {
        if (HNL::PID::SPSS::IsHNL(family.SisterPIDs().front())) return leptonProd;
      }

      return leptonOther;
    }

    case HNL::PID::SM::up:
    case HNL::PID::SM::down:
    case HNL::PID::SM::strange:
    case HNL::PID::SM::charm:
    case HNL::PID::SM::bottom: {
      if (family.NotProduced()) return quarkOther;
      const GenParticle* const mother{family.Mothers().front()};

      // Look into mother
      if (HNL::PID::SPSS::IsHNL(mother->PID)) {
        if (particle->PID > 0) return quarkP;
        return quarkA;
      }

      // If mother is W, look into gradmother
      // Sometimes it's necessary to clean-up W clones made by Pythia
      if (!HNL::PID::SM::IsW(mother->PID)) return quarkOther;

      const size_t motherIdx{family.MotherIdxs().front()};
      const size_t motherIdxClean{ptw.CleanupSelfAndPhotons(motherIdx)};

      Log<1>("\tMothers²  : {} {}", mother->M1, mother->M2);
      std::vector<size_t> idxMother2{ptw.MotherIndex(motherIdxClean)};

      if (idxMother2.empty()) return quarkOther;
      const GenParticle* const mother2{ptw.At(idxMother2.front())};

      if (HNL::PID::SPSS::IsHNL(mother2->PID)) {
        if (particle->PID > 0) return quarkP;
        return quarkA;
      }

      return quarkOther;
    }

    default:
      return other;
  }
}
}  // namespace HNL::DimuonSS::FCChh
