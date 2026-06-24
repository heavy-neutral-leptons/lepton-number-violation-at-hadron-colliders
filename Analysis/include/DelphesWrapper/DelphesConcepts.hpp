// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

// Standard Library
#include <concepts>

// Delphes
#include "classes/DelphesClasses.h"
#include "classes/SortableObject.h"

namespace HNL::Delphes {
/** @brief Matches Delphes objects deriving from `SortableObject`. */
template <typename T>
concept DelphesClass = std::derived_from<T, SortableObject>;

/** @brief Matches Delphes objects exposing a generator-particle `TRef`. */
template <typename T>
concept DelphesClassWithParticle =
    DelphesClass<T> && requires(T t) { requires std::same_as<decltype(t.Particle), TRef>; };

/**
 * @brief Resolve the generator-level particle referenced by a Delphes object.
 * @param t Delphes object carrying a `Particle` reference.
 * @return Pointer to the referenced `GenParticle`, or `nullptr` if the cast fails.
 */
template <DelphesClassWithParticle T>
GenParticle* Particle(const T& t) {
  return dynamic_cast<GenParticle*>(t.Particle.GetObject());
}

/**
 * @brief Resolve the generator-level particle referenced by a Delphes object pointer.
 * @param t Pointer to a Delphes object carrying a `Particle` reference.
 * @return Pointer to the referenced `GenParticle`, or `nullptr` if the cast fails.
 */
template <DelphesClassWithParticle T>
GenParticle* Particle(const T* const t) {
  return dynamic_cast<GenParticle*>(t->Particle.GetObject());
}

/** @brief Matches Delphes objects exposing Cartesian vertex coordinates. */
template <typename T>
concept DelphesClassWithXYZ = DelphesClass<T> && requires(T t) {
  requires std::same_as<decltype(t.X), Float_t>;
  requires std::same_as<decltype(t.Y), Float_t>;
  requires std::same_as<decltype(t.Z), Float_t>;
};

/** @brief Matches Delphes objects with a particle reference but no direct vertex coordinates. */
template <typename T>
concept DelphesClassWithParticleNoXYZ = DelphesClassWithParticle<T> && !DelphesClassWithXYZ<T>;

/** @brief Matches Delphes objects with direct coordinates or a particle reference. */
template <typename T>
concept DelphesClassWithParticleOrXYZ = DelphesClassWithParticle<T> || DelphesClassWithXYZ<T>;

/** @brief Matches Delphes objects exposing Cartesian momentum components and mass. */
template <typename T>
concept DelphesClassWithPxPyPzM = DelphesClass<T> && requires(T t) {
  requires std::same_as<decltype(t.Px), Float_t>;
  requires std::same_as<decltype(t.Py), Float_t>;
  requires std::same_as<decltype(t.Pz), Float_t>;
  requires std::same_as<decltype(t.Mass), Float_t>;
};

/** @brief Matches Delphes objects exposing transverse momentum, pseudorapidity, and azimuth. */
template <typename T>
concept DelphesClassWithPtEtaPhi = DelphesClass<T> && requires(T t) {
  requires std::same_as<decltype(t.PT), Float_t>;
  requires std::same_as<decltype(t.Eta), Float_t>;
  requires std::same_as<decltype(t.Phi), Float_t>;
};

/** @brief Matches Delphes objects exposing a `Mass` data member. */
template <typename T>
concept DelphesClassWithMass =
    DelphesClass<T> && requires(T t) { requires std::same_as<decltype(t.Mass), Float_t>; };

/** @brief Matches Delphes objects exposing `PT`, `Eta`, `Phi`, and `Mass`. */
template <typename T>
concept DelphesClassWithPtEtaPhiMass = DelphesClassWithPtEtaPhi<T> && DelphesClassWithMass<T>;

/** @brief Matches Delphes objects exposing `PT`, `Eta`, and `Phi` but not `Mass`. */
template <typename T>
concept DelphesClassWithPtEtaPhiNoMass = DelphesClassWithPtEtaPhi<T> && !DelphesClassWithMass<T>;

/** @brief Matches Delphes objects with usable vertex and cylindrical momentum information. */
template <typename T>
concept DelphesClassWithParticleOrXYZWithPtEtaPhi =
    DelphesClassWithParticleOrXYZ<T> && DelphesClassWithPtEtaPhi<T>;
}  // namespace HNL::Delphes
