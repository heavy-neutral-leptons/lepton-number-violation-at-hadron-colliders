// Copyright 2023-2026 Bruno M. S. Oliveira

// Standard Library
#include <numeric>
#include <ranges>
#include <vector>

// HNL
#include "Common/Log.hpp"
#include "DelphesWrapper/TreeNavigation.hpp"
#include "MCPID/SM.hpp"
#include "classes/DelphesClasses.h"

namespace HNL::Delphes {
/* ----- ParticleFamily ----- */
ParticleFamily::ParticleFamily(const ParticleTreeWalker &ptw, const size_t idx)
    : ptw{ptw}, idxSelf{idx} {}

[[nodiscard]] std::vector<size_t> ParticleFamily::MotherIdxs() const { return idxMothers; };

[[nodiscard]] std::vector<const GenParticle *> ParticleFamily::Mothers() const {
  return ptw.At(idxMothers);
}

[[nodiscard]] std::vector<int> ParticleFamily::MotherPIDs() const { return ptw.PID(idxMothers); }

[[nodiscard]] std::vector<size_t> ParticleFamily::SisterIdxs() const { return idxSisters; };

[[nodiscard]] std::vector<const GenParticle *> ParticleFamily::Sisters() const {
  return ptw.At(idxSisters);
}

[[nodiscard]] std::vector<int> ParticleFamily::SisterPIDs() const { return ptw.PID(idxSisters); }

[[nodiscard]] std::vector<size_t> ParticleFamily::DaughterIdxs() const { return idxDaughters; };

[[nodiscard]] std::vector<const GenParticle *> ParticleFamily::Daughters() const {
  return ptw.At(idxDaughters);
}

[[nodiscard]] std::vector<int> ParticleFamily::DaughterPIDs() const {
  return ptw.PID(idxDaughters);
}

[[nodiscard]] std::string ParticleFamily::Production() const {
  if (NotProduced()) return "";

  if (HasNoSisters()) {
    return std::format("Production: {} -> {}", ptw.AsString(idxMothers), ptw.AsString(idxSelf));
  }

  return std::format("Production: {} -> {} + {}", ptw.AsString(idxMothers), ptw.AsString(idxSelf),
                     ptw.AsString(idxSisters));
}

[[nodiscard]] std::string ParticleFamily::Decay() const {
  if (NotDecayed()) return "";

  return std::format("Decay     : {} -> {}", ptw.AsString(idxSelf), ptw.AsString(idxDaughters));
}

/* ----- ParticleTreeWalker ----- */
std::vector<size_t> MotherIndex(const GenParticle &particle) {
  if (particle.M1 < 0) return {};

  if (particle.M2 < 0) {
    return std::vector{static_cast<size_t>(particle.M1)};
  }

  if (particle.M1 == particle.M2) {
    return std::vector{static_cast<size_t>(particle.M1)};
  }

  if (particle.M1 < particle.M2) {
    const int particleStatus{std::abs(particle.Status)};
    if ((particleStatus >= 81 && particleStatus <= 86) ||
        (particleStatus >= 101 && particleStatus <= 106)) {
      std::vector<size_t> idxMothers(particle.M2 - particle.M1 + 1);
      std::ranges::iota(idxMothers.begin(), idxMothers.end(), particle.M1);
      return idxMothers;
    }

    return std::vector{static_cast<size_t>(particle.M1), static_cast<size_t>(particle.M2)};
  }

  if (particle.M2 < particle.M1) {
    return std::vector{static_cast<size_t>(particle.M1), static_cast<size_t>(particle.M2)};
  }

  return {};
}

std::vector<size_t> DaughterIndex(const GenParticle &particle, size_t idxRm) {
  // There are no daughters
  if (particle.D1 <= 0 || particle.D2 < 0) return {};

  // The particle has a "carbon copy" as its sole daughter
  if (particle.D1 == particle.D2) return {static_cast<size_t>(particle.D1)};

  // Each of the incoming beams has only (at most) one daughter
  if (particle.D1 > 0 && particle.D2 == 0) return {static_cast<size_t>(particle.D1)};

  // The particle has two separately stored decay products
  if (particle.D2 < particle.D1) {
    return {static_cast<size_t>(particle.D2), static_cast<size_t>(particle.D1)};
  }

  // The particle has a range of decay products
  std::vector<size_t> idxDaughters;
  idxDaughters.reserve(particle.D2 - particle.D1);

  for (int i{particle.D1}; i <= particle.D2; ++i) {
    if (static_cast<size_t>(i) != idxRm) {
      idxDaughters.emplace_back(i);
    }
  }

  return idxDaughters;
}

namespace {
bool operator==(const GenParticle &lhs, const GenParticle &rhs) {
  return lhs.PID == rhs.PID && lhs.PT == rhs.PT && lhs.Eta == rhs.Eta && lhs.Phi == rhs.Phi;
}
}  // namespace

std::optional<size_t> ParticleTreeWalker::Find(const GenParticle &particle) const {
  for (const auto &[idx, iter_particle] : particles | std::views::enumerate) {
    if (particle == iter_particle) return idx;
  }

  LogError("Could not find requested particle in TTreeReaderArray.");
  return std::nullopt;
}

const GenParticle *ParticleTreeWalker::At(const size_t idx) const { return &particles.At(idx); }

std::vector<const GenParticle *> ParticleTreeWalker::At(const std::vector<size_t> &idx) const {
  if (idx.size() == 0) return {};

  std::vector<const GenParticle *> out;
  out.reserve(idx.size());

  for (const size_t idx : idx) out.emplace_back(&particles.At(idx));

  return out;
}

int ParticleTreeWalker::PID(const size_t idx) const { return particles.At(idx).PID; }

std::vector<int> ParticleTreeWalker::PID(const std::vector<size_t> &idx) const {
  if (idx.size() == 0) return {};

  std::vector<int> out;
  out.reserve(idx.size());

  for (const size_t idx : idx) out.emplace_back(particles.At(idx).PID);

  return out;
}

std::string ParticleTreeWalker::AsString(const size_t idx) const {
  return std::format("{}({})", idx, particles.At(idx).PID);
}

std::string ParticleTreeWalker::AsString(const std::vector<size_t> &idxList) const {
  std::string out{};

  for (const size_t idx : idxList) {
    if (out.empty()) {
      out = AsString(idx);
    } else {
      out.append(std::format(" {}", AsString(idx)));
    }
  }

  return out;
}

std::vector<size_t> ParticleTreeWalker::MotherIndex(const size_t idx) const {
  const GenParticle &particle{particles.At(idx)};
  return Delphes::MotherIndex(particle);
}

std::vector<const GenParticle *> ParticleTreeWalker::Mother(const GenParticle &particle) const {
  return At(Delphes::MotherIndex(particle));
}

std::vector<const GenParticle *> ParticleTreeWalker::Mother(const size_t idx) const {
  const GenParticle &particle{particles.At(idx)};
  return At(Delphes::MotherIndex(particle));
}

std::vector<size_t> ParticleTreeWalker::DaughterIndex(const size_t idx, size_t idxRm) const {
  const GenParticle &particle{particles.At(idx)};
  return Delphes::DaughterIndex(particle, idxRm);
}

std::vector<const GenParticle *> ParticleTreeWalker::Daughter(const GenParticle &particle,
                                                              size_t idxRm) const {
  return At(Delphes::DaughterIndex(particle, idxRm));
}

std::vector<const GenParticle *> ParticleTreeWalker::Daughter(size_t idx, size_t idxRm) const {
  const GenParticle &particle{particles.At(idx)};
  return At(Delphes::DaughterIndex(particle, idxRm));
}

std::vector<size_t> ParticleTreeWalker::SisterIndex(const size_t idx) const {
  const std::vector<size_t> idxMothers{MotherIndex(idx)};
  if (idxMothers.size() == 0) return {};

  return DaughterIndex(idxMothers.front(), idx);
}

std::vector<size_t> ParticleTreeWalker::SisterIndex(const GenParticle &particle) const {
  const std::vector<size_t> idxMothers{Delphes::MotherIndex(particle)};
  if (idxMothers.size() == 0) return {};

  return DaughterIndex(idxMothers.front(), *Find(particle));
}

std::vector<const GenParticle *> ParticleTreeWalker::Sister(const size_t idx) const {
  return At(SisterIndex(idx));
}

std::vector<const GenParticle *> ParticleTreeWalker::Sister(const GenParticle &particle) const {
  return At(SisterIndex(particle));
}

size_t ParticleTreeWalker::CleanupSelf(size_t idx) const {
  std::vector<size_t> idxMothers{idx};
  std::vector<size_t> idxSisters{};

  while (true) {
    idx = idxMothers[0];

    idxMothers = MotherIndex(idx);
    if (idxMothers.size() != 1) break;

    const GenParticle &mother{particles.At(idxMothers[0])};
    if (mother.D1 != mother.D2) break;
  }

  return idx;
}

size_t ParticleTreeWalker::CleanupPhotons(size_t idx) const {
  const int particlePID{particles.At(idx).PID};

  std::vector<size_t> idxMothers{idx};
  std::vector<size_t> idxSisters{};

  int sisterPID{};

  while (true) {
    idx = idxMothers[0];

    idxMothers = MotherIndex(idx);
    if (idxMothers.size() != 1) break;
    if (particles.At(idxMothers[0]).PID != particlePID) break;

    idxSisters = DaughterIndex(idxMothers[0]);
    if (idxSisters.size() != 2) break;

    sisterPID = (idx == idxSisters[0]) ? PID(idxSisters[1]) : PID(idxSisters[0]);
    if (sisterPID != PID::SM::gamma) break;
  }

  return idx;
}

size_t ParticleTreeWalker::CleanupSelfAndPhotons(size_t idx) const {
  const int particlePID{particles.At(idx).PID};

  std::vector<size_t> idxMothers{idx};
  std::vector<size_t> idxSisters{};

  int sisterPID{};

  while (true) {
    idx = idxMothers[0];

    idxMothers = MotherIndex(idx);
    if (idxMothers.size() != 1) break;

    const GenParticle &mother{particles.At(idxMothers[0])};
    if (mother.PID != particlePID) break;

    if (mother.D1 == mother.D2) continue;

    idxSisters = DaughterIndex(idxMothers[0]);
    if (idxSisters.size() != 2) break;

    sisterPID = (idx == idxSisters[0]) ? PID(idxSisters[1]) : PID(idxSisters[0]);
    if (sisterPID != PID::SM::gamma) break;
  }

  return idx;
}

std::optional<size_t> ParticleTreeWalker::DescendsFrom(const size_t idxDescendent,
                                                       const size_t idxAscendent) const {
  size_t n_generations{0};
  std::optional<size_t> idx{idxDescendent};

  do {
    if (*idx == idxAscendent) return n_generations;
    ++n_generations;
  } while ((idx = MotherIndex(*idx).front()));

  return std::nullopt;
}

ParticleFamily ParticleTreeWalker::Family(size_t idx, const bool cleanupSelf,
                                          const bool cleanupPhotons) const {
  ParticleFamily family{*this, idx};

  if (cleanupSelf && cleanupPhotons) {
    idx = CleanupSelfAndPhotons(idx);
  } else if (cleanupSelf) {
    idx = CleanupSelf(idx);
  } else if (cleanupPhotons) {
    idx = CleanupPhotons(idx);
  }

  family.idxMothers = MotherIndex(idx);

  if (!family.idxMothers.empty()) {
    family.idxSisters = DaughterIndex(family.idxMothers[0], idx);
  }

  family.idxDaughters = DaughterIndex(family.idxSelf, family.idxSelf);

  return family;
}
}  // namespace HNL::Delphes
