// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

// Standard Library
#include <cmath>

/**
 * Standard Model Monte Carlo PIDs
 *
 * Source: https://pdg.lbl.gov/2022/mcdata/mc_particle_id_contents.html
 */
namespace HNL::PID::SM {
// Quarks
inline constexpr int down{1};
inline constexpr int up{2};
inline constexpr int strange{3};
inline constexpr int charm{4};
inline constexpr int bottom{5};
inline constexpr int top{6};

inline constexpr int quarks[]{down, up, strange, charm, bottom, top};
constexpr bool IsQuark(int pid) {
  pid = std::abs(pid);
  return pid == down || pid == up || pid == strange || pid == charm || pid == bottom || pid == top;
}

inline constexpr int upTypeQuarks[]{up, charm, top};
constexpr bool IsUpTypeQuark(int pid) {
  pid = std::abs(pid);
  return pid == up || pid == charm || pid == top;
}

inline constexpr int downTypeQuarks[]{down, strange, bottom};
constexpr bool IsDownQuark(int pid) {
  pid = std::abs(pid);
  return pid == down || pid == strange || pid == bottom;
}

inline constexpr int lightQuarks[]{down, up};
constexpr bool IsLightQuark(int pid) {
  pid = std::abs(pid);
  return pid == down || pid == up;
}

// Leptons
inline constexpr int electron{11};
inline constexpr int velectron{12};
inline constexpr int muon{13};
inline constexpr int vmuon{14};
inline constexpr int tau{15};
inline constexpr int vtau{16};

inline constexpr int leptons[]{electron, velectron, muon, vmuon, tau, vtau};
constexpr bool IsLepton(int pid) {
  pid = std::abs(pid);
  return pid == electron || pid == velectron || pid == muon || pid == vmuon || pid == tau ||
         pid == vtau;
}

inline constexpr int chargedLeptons[]{electron, muon, tau};
constexpr bool IsChargedLepton(int pid) {
  pid = std::abs(pid);
  return pid == electron || pid == muon || pid == tau;
}

inline constexpr int neutralLeptons[]{velectron, vmuon, vtau};
constexpr bool IsNeutralLepton(int pid) {
  pid = std::abs(pid);
  return pid == velectron || pid == vmuon || pid == vtau;
}

constexpr bool IsElectron(const int pid) { return pid == electron || pid == -electron; }
constexpr bool IsMuon(const int pid) { return pid == muon || pid == -muon; }
constexpr bool IsTau(const int pid) { return pid == tau || pid == -tau; }

// Bosons
inline constexpr int gluon{21};
inline constexpr int gamma{22};
inline constexpr int z{23};
inline constexpr int w{24};
inline constexpr int h{25};

constexpr bool IsW(const int pid) { return pid == w || pid == -w; }
}  // namespace HNL::PID::SM
