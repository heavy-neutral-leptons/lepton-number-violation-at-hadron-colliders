// Copyright 2023-2026 Bruno M. S. Oliveira

// ROOT
#include "Math/GenVector/VectorUtil.h"

// HNL
#include "DelphesWrapper/CustomPrints.hpp"

/* ----- Other ----- */
namespace HNL::Delphes {
double CutFraction(size_t n_after_cut, size_t n_before_cut) {
  return static_cast<double>(n_after_cut) / static_cast<double>(n_before_cut);
}

std::string PrintCut(const size_t i, const size_t j) {
  return std::format("{}/{} events ({}%)", i, j, 100. * CutFraction(i, j));
}

std::string PrintDelta(ROOT::Math::PtEtaPhiMVector const &momentum1,
                       ROOT::Math::PtEtaPhiMVector const &momentum2) {
  return std::format("{},{},{},{},{}", momentum1.Pt() - momentum2.Pt(),
                     momentum1.Pz() - momentum2.Pz(), momentum1.M() - momentum2.M(),
                     ROOT::Math::VectorUtil::Angle(momentum1, momentum2),
                     ROOT::Math::VectorUtil::DeltaR(momentum1, momentum2));
}
}  // namespace HNL::Delphes
