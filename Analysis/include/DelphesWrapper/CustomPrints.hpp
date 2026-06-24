// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

// Standard Library
#include <format>
#include <string>

// ROOT
#include "Math/Vector4Dfwd.h"

// Delphes
#include "classes/DelphesClasses.h"

// HNL
#include "DelphesWrapper/DelphesConcepts.hpp"

/* ----- GenParticle ----- */
/** @brief CSV header matching the `std::formatter<GenParticle>` output. */
inline constexpr std::string_view header_genparticle{
    "PID,Status,IsPU,M1,M2,D1,D2,Charge,Mass,E,Px,Py,Pz,P,PT,Eta,Phi,Rapidity,"
    "T,X,Y,Z"};
/** @brief Stream a generator particle using the custom Delphes print format. */
std::ostream &operator<<(std::ostream &out, GenParticle const &particle);

/** @brief Formats `GenParticle` as a single CSV row. */
template <>
struct std::formatter<GenParticle> {
  constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const GenParticle &particle, FormatContext &ctx) {
    return std::format_to(ctx.out(),
                          "{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},",
                          particle.PID,     // particle HEP ID number | hepevt.idhep[number]
                          particle.Status,  // particle status | hepevt.isthep[number]
                          particle.IsPU,    // 0 or 1 for particles from pile-up interactions
                          particle.M1,      // particle 1st mother | hepevt.jmohep[number][0] - 1
                          particle.M2,      // particle 2nd mother | hepevt.jmohep[number][1] - 1
                          particle.D1,      // particle 1st daughter | hepevt.jdahep[number][0] - 1
                          particle.D2,      // particle last daughter | hepevt.jdahep[number][1] - 1
                          particle.Charge,  // particle charge
                          particle.Mass,    // particle mass
                          particle.E,       // particle energy | hepevt.phep[number][3]
                          particle.Px,      // particle momentum vector (x component) |
                                            // hepevt.phep[number][0]
                          particle.Py,      // particle momentum vector (y component) |
                                            // hepevt.phep[number][1]
                          particle.Pz,      // particle momentum vector (z component) |
                                            // hepevt.phep[number][2]
                          particle.P,       // particle momentum
                          particle.PT,      // particle transverse momentum
                          particle.Eta,     // particle pseudorapidity
                          particle.Phi,     // particle azimuthal angle
                          particle.Rapidity,  // particle rapidity
                          particle.T,         // particle vertex position (t component) |
                                              // hepevt.vhep[number][3]
                          particle.X,         // particle vertex position (x component) |
                                              // hepevt.vhep[number][0]
                          particle.Y,         // particle vertex position (y component) |
                                              // hepevt.vhep[number][1]
                          particle.Z          // particle vertex position (z component) |
                                              // hepevt.vhep[number][2]
    );
  }
};

/* ----- Muon ----- */
/** @brief CSV header matching the `std::formatter<Muon>` output. */
inline constexpr std::string_view header_muon{
    "PT,Eta,Phi,T,Charge,IsolationVar,IsolationVarRhoCorr,SumPtCharged,"
    "SumPtNeutral,SumPtChargedPU,SumPt,D0,DZ,ErrorD0,"
    "ErrorDZ,PID,Status,IsPU,M1,M2,D1,D2,Charge,Mass,E,Px,Py,Pz,P,PT,Eta,Phi,"
    "Rapidity,T,X,Y,Z"};

/** @brief Formats `Muon` as a CSV row followed by its linked generator particle. */
template <>
struct std::formatter<Muon> {
  constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const Muon &muon, FormatContext &ctx) {
    return std::format_to(ctx.out(), "{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{}",
                          muon.PT,                      // muon transverse momentum
                          muon.Eta,                     // muon pseudorapidity
                          muon.Phi,                     // muon azimuthal angle
                          muon.T,                       // particle arrival time of flight
                          muon.Charge,                  // muon charge
                          muon.IsolationVar,            // isolation variable
                          muon.IsolationVarRhoCorr,     // isolation variable
                          muon.SumPtCharged,            // isolation variable
                          muon.SumPtNeutral,            // isolation variable
                          muon.SumPtChargedPU,          // isolation variable
                          muon.SumPt,                   // isolation variable
                          muon.D0,                      // track transverse impact parameter
                          muon.DZ,                      // track longitudinal impact parameter
                          muon.ErrorD0,                 // track transverse impact parameter error
                          muon.ErrorDZ,                 // track longitudinal impact parameter error
                          HNL::Delphes::Particle(muon)  // reference to generated particle
    );
  }
};

/* ----- Jet ----- */
/** @brief CSV header matching the `std::formatter<Jet>` output. */
inline constexpr std::string_view header_jet{"PT,Eta,Phi,Mass,T,DeltaEta,DeltaPhi,MeanSqDeltaR"};

/** @brief Formats `Jet` as a single CSV row. */
template <>
struct std::formatter<Jet> {
  constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const Jet &jet, FormatContext &ctx) {
    return std::format_to(ctx.out(), "{},{},{},{},{},{},{},{},",
                          jet.PT,           //
                          jet.Eta,          //
                          jet.Phi,          //
                          jet.Mass,         //
                          jet.T,            //
                          jet.DeltaEta,     //
                          jet.DeltaPhi,     //
                          jet.MeanSqDeltaR  //
    );
  }
};

/* ----- PtEtaPhiMVector ----- */
/** @brief CSV header matching the `std::formatter<ROOT::Math::PtEtaPhiMVector>` output. */
inline constexpr std::string_view header_momentum{"Pt,Pz,M"};

/** @brief Formats `ROOT::Math::PtEtaPhiMVector` as a compact CSV row. */
template <>
struct std::formatter<ROOT::Math::PtEtaPhiMVector> {
  constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const ROOT::Math::PtEtaPhiMVector &momentum, FormatContext &ctx) {
    return std::format_to(ctx.out(), "{},{},{}",
                          momentum.Pt(),  //
                          momentum.Pz(),  //
                          momentum.M()    //
    );
  }
};

/* ----- Other ----- */
namespace HNL::Delphes {
/** @brief Compute the event fraction surviving a cut. */
double CutFraction(size_t n_after_cut, size_t n_before_cut);
/** @brief Format a cut-flow transition between two event counts. */
std::string PrintCut(size_t i, size_t j);

/** @brief Format the component-wise difference between two four-momenta. */
std::string PrintDelta(ROOT::Math::PtEtaPhiMVector const &momentum1,
                       ROOT::Math::PtEtaPhiMVector const &momentum2);
}  // namespace HNL::Delphes
