// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

// Standard Library
#include <optional>
#include <string>
#include <vector>

// ROOT
#include "TTreeReaderArray.h"

// Delphes
#include "classes/DelphesClasses.h"

// HNL
#include "DelphesWrapper/DelphesConcepts.hpp"

namespace HNL::Delphes {
// https://pythia.org/latest-manual/ParticleProperties.html

/** @brief Convenience alias for the Delphes generator-particle branch reader. */
using ParticleReader = TTreeReaderArray<GenParticle>;

/** @brief Cached family relationships extracted from a Delphes generator tree. */
class ParticleFamily;

///@name Particle ancestry helpers
///@{
/** @brief Return the valid mother indices stored by a Delphes generator particle. */
std::vector<size_t> MotherIndex(const GenParticle &particle);
/** @brief Return the valid daughter indices stored by a Delphes generator particle. */
std::vector<size_t> DaughterIndex(const GenParticle &particle, size_t idxRm = -1);

/** @brief Navigates mother, daughter, and sister relations inside a Delphes particle branch. */
class ParticleTreeWalker {
 public:
  explicit ParticleTreeWalker(const ParticleReader &particles) : particles{particles} {}

  /** @brief Find the index of a particle within the wrapped branch, if present. */
  [[nodiscard]] std::optional<size_t> Find(const GenParticle &particle) const;

  /** @brief Return the particle stored at a given index, or `nullptr` when invalid. */
  [[nodiscard]] const GenParticle *At(size_t idx) const;
  /** @brief Return the particles stored at the provided indices. */
  [[nodiscard]] std::vector<const GenParticle *> At(const std::vector<size_t> &idx) const;

  /** @brief Return the PDG identifier of the particle at a given index. */
  [[nodiscard]] int PID(size_t idx) const;
  /** @brief Return the PDG identifiers for the particles at the provided indices. */
  [[nodiscard]] std::vector<int> PID(const std::vector<size_t> &idx) const;

  /** @brief Format a particle and its nearby family context as a compact string. */
  [[nodiscard]] std::string AsString(size_t idx) const;
  /** @brief Format multiple particles and their nearby family context as compact strings. */
  [[nodiscard]] std::string AsString(const std::vector<size_t> &idx) const;

  /** @brief Return the mother indices of the particle at a given index. */
  [[nodiscard]] std::vector<size_t> MotherIndex(size_t idx) const;

  /** @brief Return the mothers of a particle instance. */
  [[nodiscard]] std::vector<const GenParticle *> Mother(const GenParticle &particle) const;
  /** @brief Return the mothers of the particle at a given index. */
  [[nodiscard]] std::vector<const GenParticle *> Mother(size_t idx) const;

  /** @brief Return the daughter indices of the particle at a given index. */
  [[nodiscard]] std::vector<size_t> DaughterIndex(size_t idx, size_t idxRm = -1) const;

  /** @brief Return the daughters of a particle instance. */
  [[nodiscard]] std::vector<const GenParticle *> Daughter(const GenParticle &particle,
                                                          size_t idxRm = -1) const;
  /** @brief Return the daughters of the particle at a given index. */
  [[nodiscard]] std::vector<const GenParticle *> Daughter(size_t idx, size_t idxRm = -1) const;

  /** @brief Return the sibling indices of the particle at a given index. */
  [[nodiscard]] std::vector<size_t> SisterIndex(size_t idx) const;
  /** @brief Return the sibling indices of a particle instance. */
  [[nodiscard]] std::vector<size_t> SisterIndex(const GenParticle &particle) const;

  /** @brief Return the siblings of the particle at a given index. */
  [[nodiscard]] std::vector<const GenParticle *> Sister(size_t idx) const;
  /** @brief Return the siblings of a particle instance. */
  [[nodiscard]] std::vector<const GenParticle *> Sister(const GenParticle &particle) const;

  /** @brief Remove self-references from a traversal result. */
  [[nodiscard]] size_t CleanupSelf(size_t idx) const;
  /** @brief Remove intermediate photons from a traversal result. */
  [[nodiscard]] size_t CleanupPhotons(size_t idx) const;
  /** @brief Remove self-references and intermediate photons from a traversal result. */
  [[nodiscard]] size_t CleanupSelfAndPhotons(size_t idx) const;

  /** @brief Build a cached family view around a given particle index. */
  [[nodiscard]] ParticleFamily Family(size_t idx, bool cleanupSelf = false,
                                      bool cleanupPhotons = false) const;

  /** @brief Determine whether a particle descends from a given ancestor index. */
  [[nodiscard]] std::optional<size_t> DescendsFrom(size_t idxDescendent, size_t idxAscendent) const;

 private:
  const ParticleReader &particles;
};

/** @brief Cached family relationships for one particle in a Delphes generator tree. */
class ParticleFamily {
  ParticleFamily(const ParticleTreeWalker &ptw, size_t idx);

 public:
  /** @brief Return `true` when the particle has at least one recorded mother. */
  [[nodiscard]] constexpr bool Produced() const { return !idxMothers.empty(); };
  /** @brief Return `true` when the particle has no recorded mothers. */
  [[nodiscard]] constexpr bool NotProduced() const { return idxMothers.empty(); };

  /** @brief Return the cached mother indices. */
  [[nodiscard]] std::vector<size_t> MotherIdxs() const;
  /** @brief Return the cached mother particles. */
  [[nodiscard]] std::vector<const GenParticle *> Mothers() const;
  /** @brief Return the cached mother PDG identifiers. */
  [[nodiscard]] std::vector<int> MotherPIDs() const;

  /** @brief Return `true` when the particle has at least one recorded daughter. */
  [[nodiscard]] constexpr bool Decayed() const { return !idxDaughters.empty(); };
  /** @brief Return `true` when the particle has no recorded daughters. */
  [[nodiscard]] constexpr bool NotDecayed() const { return idxDaughters.empty(); };

  /** @brief Return the cached sibling indices. */
  [[nodiscard]] std::vector<size_t> SisterIdxs() const;
  /** @brief Return the cached sibling particles. */
  [[nodiscard]] std::vector<const GenParticle *> Sisters() const;
  /** @brief Return the cached sibling PDG identifiers. */
  [[nodiscard]] std::vector<int> SisterPIDs() const;

  /** @brief Return `true` when the particle has at least one recorded sibling. */
  [[nodiscard]] constexpr bool HasSisters() const { return !idxSisters.empty(); };
  /** @brief Return `true` when the particle has no recorded siblings. */
  [[nodiscard]] constexpr bool HasNoSisters() const { return idxSisters.empty(); };

  /** @brief Return the cached daughter indices. */
  [[nodiscard]] std::vector<size_t> DaughterIdxs() const;
  /** @brief Return the cached daughter particles. */
  [[nodiscard]] std::vector<const GenParticle *> Daughters() const;
  /** @brief Return the cached daughter PDG identifiers. */
  [[nodiscard]] std::vector<int> DaughterPIDs() const;

  /** @brief Format the production topology of the cached family. */
  [[nodiscard]] std::string Production() const;
  /** @brief Format the decay topology of the cached family. */
  [[nodiscard]] std::string Decay() const;

 private:
  const ParticleTreeWalker &ptw;

  size_t idxSelf;

  std::vector<size_t> idxMothers;
  std::vector<size_t> idxSisters;
  std::vector<size_t> idxDaughters;

  friend ParticleFamily ParticleTreeWalker::Family(size_t idx, bool cleanupSelf,
                                                   bool cleanupPhotons) const;
};
///@}
}  // namespace HNL::Delphes
