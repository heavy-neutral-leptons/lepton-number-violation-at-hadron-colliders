// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

// ROOT
#include "Math/Point3D.h"

// Delphes
#include "classes/DelphesClasses.h"

// HNL
#include "DelphesWrapper/DelphesConcepts.hpp"

namespace HNL::Delphes {
///@name 3D geometry helpers
///@{

/** @brief Compute the Euclidean norm of the provided floating-point components. */
template <std::floating_point... T>
constexpr auto Amplitude(T... args) {
  double sum_of_squares{0};
  for (const auto x : {args...}) sum_of_squares += x * x;
  return std::sqrt(sum_of_squares);
}

/** @brief Compute the 3D displacement magnitude from explicit Delphes coordinates. */
template <DelphesClassWithXYZ T>
inline double Displacement(const T &t) {
  return Amplitude(t.X, t.Y, t.Z);
}

/** @brief Compute the 3D displacement magnitude from explicit Delphes coordinates. */
template <DelphesClassWithXYZ T>
inline double Displacement(const T *const t) {
  return Amplitude(t->X, t->Y, t->Z);
}

/** @brief Compute the 3D displacement through the referenced generator-level particle. */
template <DelphesClassWithParticleNoXYZ T>
inline double Displacement(const T &t) {
  return Displacement(Particle(t));
}

/** @brief Compute the 3D displacement through the referenced generator-level particle. */
template <DelphesClassWithParticleNoXYZ T>
inline double Displacement(const T *const t) {
  return Displacement(Particle(t));
}

/** @brief Compute the transverse displacement magnitude from explicit coordinates. */
template <DelphesClassWithXYZ T>
inline double TransverseDisplacement(const T &t) {
  return Amplitude(t.X, t.Y);
}

/** @brief Compute the transverse displacement magnitude from explicit coordinates. */
template <DelphesClassWithXYZ T>
inline double TransverseDisplacement(const T *const t) {
  return Amplitude(t->X, t->Y);
}

/** @brief Compute the transverse displacement through the referenced generator-level particle. */
template <DelphesClassWithParticleNoXYZ T>
inline double TransverseDisplacement(const T &t) {
  return TransverseDisplacement(Particle(t));
}

/** @brief Compute the transverse displacement through the referenced generator-level particle. */
template <DelphesClassWithParticleNoXYZ T>
inline double TransverseDisplacement(const T *const t) {
  return TransverseDisplacement(Particle(t));
}

/** @brief Return the longitudinal displacement through the referenced generator-level particle. */
template <DelphesClassWithParticleNoXYZ T>
inline double LongitudinalDisplacement(const T &t) {
  return Particle(t)->Z;
}

/** @brief Return the longitudinal displacement through the referenced generator-level particle. */
template <DelphesClassWithParticleNoXYZ T>
inline double LongitudinalDisplacement(const T *const t) {
  return Particle(t)->Z;
}

/** @brief Compute the signed transverse impact parameter from a vertex and momentum. */
inline double TransverseImpactParameter(const ROOT::Math::XYZPoint &vertex,
                                        const ROOT::Math::PtEtaPhiMVector &momentum) {
  return ((vertex.X() * momentum.Y()) - (vertex.Y() * momentum.X())) / momentum.Pt();
}

/** @brief Compute the signed transverse impact parameter from 2D coordinate arrays. */
inline double TransverseImpactParameter(const double vertex[2], const double momentum[2]) {
  return ((vertex[0] * momentum[1]) - (vertex[1] * momentum[0])) /
         Amplitude(momentum[0], momentum[1]);
}

/** @brief Convert explicit Delphes coordinates into a ROOT 3D point. */
template <DelphesClassWithXYZ T>
inline ROOT::Math::XYZPoint Origin(const T &t) {
  return {t.X, t.Y, t.Z};
}

/** @brief Convert explicit Delphes coordinates into a ROOT 3D point. */
template <DelphesClassWithXYZ T>
inline ROOT::Math::XYZPoint Origin(const T *const t) {
  return {t->X, t->Y, t->Z};
}

/** @brief Return the origin of the referenced generator-level particle. */
template <DelphesClassWithParticleNoXYZ T>
inline ROOT::Math::XYZPoint Origin(const T &t) {
  return Origin(Particle(t));
}

/** @brief Return the origin of the referenced generator-level particle. */
template <DelphesClassWithParticleNoXYZ T>
inline ROOT::Math::XYZPoint Origin(const T *const t) {
  return Origin(Particle(t));
}

/** @brief Compute the 3D distance between two explicit Delphes vertices. */
template <DelphesClassWithXYZ T1, DelphesClassWithXYZ T2>
inline double VertexDistance(const T1 &t1, const T2 &t2) {
  return Amplitude(t1.X - t2.X, 2, t1.Y - t2.Y, t1.Z - t2.Z);
}

/** @brief Compute the 3D distance between two explicit Delphes vertices. */
template <DelphesClassWithXYZ T1, DelphesClassWithXYZ T2>
inline double VertexDistance(const T1 *const t1, const T2 *const t2) {
  return Amplitude(t1->X - t2->X, 2, t1->Y - t2->Y, t1->Z - t2->Z);
}
///@}
}  // namespace HNL::Delphes
