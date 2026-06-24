// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

// ROOT
#include "Math/Vector4Dfwd.h"

// Delphes
#include "classes/DelphesClasses.h"

// HNL
#include "Constants/Maths.hpp"
#include "Constants/SMMasses.hpp"
#include "DelphesWrapper/DelphesConcepts.hpp"

namespace HNL::Delphes {
///@name Four-momentum helpers
///@{
/** @brief Build a Cartesian four-momentum from Delphes momentum components and mass. */
template <DelphesClassWithPxPyPzM T>
inline ROOT::Math::PxPyPzMVector MomentumPxPyPzM(const T &t) {
  return {t.Px, t.Py, t.Pz, t.Mass};
}

/** @brief Build a cylindrical four-momentum from Delphes transverse kinematics and mass. */
template <DelphesClassWithPtEtaPhiMass T>
inline ROOT::Math::PtEtaPhiMVector Momentum(const T &t) {
  return {t.PT, t.Eta, t.Phi, t.Mass};
}

/** @brief Build a photon four-momentum using zero mass. */
inline ROOT::Math::PtEtaPhiMVector Momentum(const Photon &photon) {
  return {photon.PT, photon.Eta, photon.Phi, 0};
}

/** @brief Build an electron four-momentum using the PDG electron mass. */
inline ROOT::Math::PtEtaPhiMVector Momentum(const Electron &electron) {
  return {electron.PT, electron.Eta, electron.Phi, Constants::massElectron};
}

/** @brief Build a muon four-momentum using the PDG muon mass. */
inline ROOT::Math::PtEtaPhiMVector Momentum(const Muon &muon) {
  return {muon.PT, muon.Eta, muon.Phi, Constants::massMuon};
}

/** @brief Build a cylindrical four-momentum from a Delphes object pointer. */
template <DelphesClassWithPtEtaPhiMass T>
inline ROOT::Math::PtEtaPhiMVector Momentum(const T *const t) {
  return {t->PT, t->Eta, t->Phi, t->Mass};
}

/** @brief Build a Cartesian four-momentum from a Delphes object pointer. */
template <DelphesClassWithPxPyPzM T>
inline ROOT::Math::PxPyPzMVector MomentumPxPyPzM(const T *const t) {
  return {t->Px, t->Py, t->Pz, t->Mass};
}

/** @brief Build a photon four-momentum from a pointer using zero mass. */
inline ROOT::Math::PtEtaPhiMVector Momentum(const Photon *const photon) {
  return {photon->PT, photon->Eta, photon->Phi, 0};
}

/** @brief Build an electron four-momentum from a pointer using the PDG electron mass. */
inline ROOT::Math::PtEtaPhiMVector Momentum(const Electron *const electron) {
  return {electron->PT, electron->Eta, electron->Phi, Constants::massElectron};
}

/** @brief Build a muon four-momentum from a pointer using the PDG muon mass. */
inline ROOT::Math::PtEtaPhiMVector Momentum(const Muon *const muon) {
  return {muon->PT, muon->Eta, muon->Phi, Constants::massMuon};
}

/** @brief Convert pseudorapidity into the polar angle. */
template <DelphesClassWithPtEtaPhi T>
inline double PolarAngle(const T &t) {
  return 2 * std::atan(std::exp(-t.Eta));
}

/** @brief Convert pseudorapidity into the polar angle. */
template <DelphesClassWithPtEtaPhi T>
inline double PolarAngle(const T *const t) {
  return 2 * std::atan(std::exp(-t->Eta));
}

/** @brief Compute the angular separation `DeltaR` between two Delphes objects. */
template <DelphesClassWithPtEtaPhi T1, DelphesClassWithPtEtaPhi T2>
inline double DeltaR(const T1 &t1, const T2 &t2) {
  return std::sqrt(std::pow(t1.Eta - t2.Eta, 2) +
                   std::pow(std::remainder(t1.Phi - t2.Phi, Constants::pi), 2));
}

/** @brief Compute the angular separation `DeltaR` between two Delphes object pointers. */
template <DelphesClassWithPtEtaPhi T1, DelphesClassWithPtEtaPhi T2>
inline double DeltaR(const T1 *const t1, const T2 *const t2) {
  return std::sqrt(std::pow(t1->Eta - t2->Eta, 2) +
                   std::remainder(std::pow(t1->Phi - t2->Phi, 2), Constants::pi));
}

/** @brief Compute the angular separation `DeltaR` to a ROOT four-vector. */
template <DelphesClassWithPtEtaPhi T1>
inline double DeltaR(const T1 &t1, const ROOT::Math::PtEtaPhiMVector &t2) {
  return std::sqrt(std::pow(t1.Eta - t2.Eta(), 2) +
                   std::pow(std::remainder(t1.Phi - t2.Phi(), Constants::pi), 2));
}

/** @brief Compute the angular separation `DeltaR` from a Delphes object pointer to a ROOT
 * four-vector. */
template <DelphesClassWithPtEtaPhi T1>
inline double DeltaR(const T1 *t1, const ROOT::Math::PtEtaPhiMVector &t2) {
  return std::sqrt(std::pow(t1->Eta - t2.Eta(), 2) +
                   std::remainder(std::pow(t1->Phi - t2.Phi(), 2), Constants::pi));
}
///@}
}  // namespace HNL::Delphes
