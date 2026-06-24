// Copyright 2023-2026 Bruno M. S. Oliveira

// ROOT
#include "Math/GenVector/VectorUtil.h"

// HNL
#include "DelphesWrapper/DelphesVector4D.hpp"
#include "DimuonSS/TreeWriter.hpp"

namespace HNL::DimuonSS {
TreeWriter::TreeWriter(const std::filesystem::path &filePath, const char *const treeName)
    : Reconstruct::TreeWriterBase{filePath, treeName} {
  tree.Branch("l1_qe", &l1_qe);
  tree.Branch("l1_pt", &l1_pt);
  tree.Branch("l1_et", &l1_et);
  tree.Branch("l1_ph", &l1_ph);

  tree.Branch("l2_qe", &l2_qe);
  tree.Branch("l2_pt", &l2_pt);
  tree.Branch("l2_et", &l2_et);
  tree.Branch("l2_ph", &l2_ph);

  tree.Branch("j1_pt", &j1_pt);
  tree.Branch("j1_et", &j1_et);
  tree.Branch("j1_ph", &j1_ph);
  tree.Branch("j1_ma", &j1_ma);

  tree.Branch("j2_pt", &j2_pt);
  tree.Branch("j2_et", &j2_et);
  tree.Branch("j2_ph", &j2_ph);
  tree.Branch("j2_ma", &j2_ma);

  tree.Branch("j12_pt", &j12_pt);
  tree.Branch("j12_et", &j12_et);
  tree.Branch("j12_ph", &j12_ph);
  tree.Branch("j12_ma", &j12_ma);

  tree.Branch("ms_et", &ms_et);

  tree.Branch("l1_l2_dr", &l1_l2_dr);
  tree.Branch("l1_l2_de", &l1_l2_de);
  tree.Branch("l1_l2_dp", &l1_l2_dp);

  tree.Branch("l1_j1_dr", &l1_j1_dr);
  tree.Branch("l1_j1_de", &l1_j1_de);
  tree.Branch("l1_j1_dp", &l1_j1_dp);
  tree.Branch("l1_j2_dr", &l1_j2_dr);
  tree.Branch("l1_j2_de", &l1_j2_de);
  tree.Branch("l1_j2_dp", &l1_j2_dp);
  tree.Branch("l1_j12_dr", &l1_j12_dr);
  tree.Branch("l1_j12_de", &l1_j12_de);
  tree.Branch("l1_j12_dp", &l1_j12_dp);

  tree.Branch("l2_j1_dr", &l2_j1_dr);
  tree.Branch("l2_j1_de", &l2_j1_de);
  tree.Branch("l2_j1_dp", &l2_j1_dp);
  tree.Branch("l2_j2_dr", &l2_j2_dr);
  tree.Branch("l2_j2_de", &l2_j2_de);
  tree.Branch("l2_j2_dp", &l2_j2_dp);
  tree.Branch("l2_j12_dr", &l2_j12_dr);
  tree.Branch("l2_j12_de", &l2_j12_de);
  tree.Branch("l2_j12_dp", &l2_j12_dp);

  tree.Branch("j1_j2_dr", &j1_j2_dr);
  tree.Branch("j1_j2_de", &j1_j2_de);
  tree.Branch("j1_j2_dp", &j1_j2_dp);
}

TreeWriter::~TreeWriter() { Close(); }

void TreeWriter::AddEvent(const Muon *const muon1, const Muon *const muon2, const Jet *const jet1,
                          const Jet *const jet2, const double missingET) {
  l1_qe = muon1->Charge;
  l1_pt = muon1->PT;
  l1_et = muon1->Eta;
  l1_ph = muon1->Phi;
  const ROOT::Math::PtEtaPhiMVector pL1{Delphes::Momentum(muon1)};

  l2_qe = muon2->Charge;
  l2_pt = muon2->PT;
  l2_et = muon2->Eta;
  l2_ph = muon2->Phi;
  const ROOT::Math::PtEtaPhiMVector pL2{Delphes::Momentum(muon2)};

  j1_pt = jet1->PT;
  j1_et = jet1->Eta;
  j1_ph = jet1->Phi;
  j1_ma = jet1->Mass;
  const ROOT::Math::PtEtaPhiMVector pJ1{Delphes::Momentum(jet1)};

  j2_pt = jet2->PT;
  j2_et = jet2->Eta;
  j2_ph = jet2->Phi;
  j2_ma = jet2->Mass;
  const ROOT::Math::PtEtaPhiMVector pJ2{Delphes::Momentum(jet2)};

  ms_et = missingET;

  AddMomentumCombinations(pL1, pL2, pJ1, pJ2);

  tree.Fill();
}

void TreeWriter::AddEvent(const GenParticle *const leptonProd, const GenParticle *const leptonDecay,
                          const GenParticle *const quark1, const GenParticle *const quark2) {
  l1_qe = leptonProd->Charge;
  l1_pt = leptonProd->PT;
  l1_et = leptonProd->Eta;
  l1_ph = leptonProd->Phi;
  const ROOT::Math::PtEtaPhiMVector pL1{Delphes::Momentum(leptonProd)};

  l2_qe = leptonDecay->Charge;
  l2_pt = leptonDecay->PT;
  l2_et = leptonDecay->Eta;
  l2_ph = leptonDecay->Phi;
  const ROOT::Math::PtEtaPhiMVector pL2{Delphes::Momentum(leptonDecay)};

  j1_pt = quark1->PT;
  j1_et = quark1->Eta;
  j1_ph = quark1->Phi;
  j1_ma = quark1->Mass;
  const ROOT::Math::PtEtaPhiMVector pJ1{Delphes::Momentum(quark1)};

  j2_pt = quark2->PT;
  j2_et = quark2->Eta;
  j2_ph = quark2->Phi;
  j2_ma = quark2->Mass;
  const ROOT::Math::PtEtaPhiMVector pJ2{Delphes::Momentum(quark2)};

  ms_et = 0;

  AddMomentumCombinations(pL1, pL2, pJ1, pJ2);

  tree.Fill();
}

void TreeWriter::AddMomentumCombinations(const ROOT::Math::PtEtaPhiMVector &pL1,
                                         const ROOT::Math::PtEtaPhiMVector &pL2,
                                         const ROOT::Math::PtEtaPhiMVector &pJ1,
                                         const ROOT::Math::PtEtaPhiMVector &pJ2) {
  const ROOT::Math::PtEtaPhiMVector pJ12{pJ1 + pJ2};
  j12_pt = pJ12.Pt();
  j12_et = pJ12.Eta();
  j12_ph = pJ12.Phi();
  j12_ma = pJ12.M();

  l1_l2_dr = ROOT::Math::VectorUtil::DeltaR(pL1, pL2);
  l1_l2_de = pL1.Eta() - pL2.Eta();
  l1_l2_dp = ROOT::Math::VectorUtil::DeltaPhi(pL1, pL2);

  l1_j1_dr = ROOT::Math::VectorUtil::DeltaR(pL1, pJ1);
  l1_j1_de = pL1.Eta() - pJ1.Eta();
  l1_j1_dp = ROOT::Math::VectorUtil::DeltaPhi(pL1, pJ1);
  l1_j2_dr = ROOT::Math::VectorUtil::DeltaR(pL1, pJ2);
  l1_j2_de = pL1.Eta() - pJ2.Eta();
  l1_j2_dp = ROOT::Math::VectorUtil::DeltaPhi(pL1, pJ2);
  l1_j12_dr = ROOT::Math::VectorUtil::DeltaR(pL1, pJ12);
  l1_j12_de = pL1.Eta() - pJ12.Eta();
  l1_j12_dp = ROOT::Math::VectorUtil::DeltaPhi(pL1, pJ12);

  l2_j1_dr = ROOT::Math::VectorUtil::DeltaR(pL2, pJ1);
  l2_j1_de = pL2.Eta() - pJ1.Eta();
  l2_j1_dp = ROOT::Math::VectorUtil::DeltaPhi(pL2, pJ1);
  l2_j2_dr = ROOT::Math::VectorUtil::DeltaR(pL2, pJ2);
  l2_j2_de = pL2.Eta() - pJ2.Eta();
  l2_j2_dp = ROOT::Math::VectorUtil::DeltaPhi(pL2, pJ2);
  l2_j12_dr = ROOT::Math::VectorUtil::DeltaR(pL2, pJ12);
  l2_j12_de = pL2.Eta() - pJ12.Eta();
  l2_j12_dp = ROOT::Math::VectorUtil::DeltaPhi(pL2, pJ12);

  j1_j2_dr = ROOT::Math::VectorUtil::DeltaR(pJ1, pJ2);
  j1_j2_de = pJ1.Eta() - pJ2.Eta();
  j1_j2_dp = ROOT::Math::VectorUtil::DeltaPhi(pJ1, pJ2);
}
}  // namespace HNL::DimuonSS
