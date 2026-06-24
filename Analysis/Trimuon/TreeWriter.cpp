// Copyright 2023-2026 Bruno M. S. Oliveira

// ROOT
#include "Math/GenVector/VectorUtil.h"

// HNL
#include "DelphesWrapper/DelphesVector4D.hpp"
#include "Trimuon/TreeWriter.hpp"

namespace HNL::Trimuon {
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

  tree.Branch("l3_qe", &l3_qe);
  tree.Branch("l3_pt", &l3_pt);
  tree.Branch("l3_et", &l3_et);
  tree.Branch("l3_ph", &l3_ph);

  tree.Branch("ms_et", &ms_et);

  tree.Branch("l1_l2_dr", &l1_l2_dr);
  tree.Branch("l1_l2_de", &l1_l2_de);
  tree.Branch("l1_l2_dp", &l1_l2_dp);

  tree.Branch("l1_l3_dr", &l1_l3_dr);
  tree.Branch("l1_l3_de", &l1_l3_de);
  tree.Branch("l1_l3_dp", &l1_l3_dp);

  tree.Branch("l2_l3_dr", &l2_l3_dr);
  tree.Branch("l2_l3_de", &l2_l3_de);
  tree.Branch("l2_l3_dp", &l2_l3_dp);
}

void TreeWriter::AddEvent(const Muon *const muon1, const Muon *const muon2, const Muon *const muon3,
                          const double missingET) {
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

  l3_qe = muon3->Charge;
  l3_pt = muon3->PT;
  l3_et = muon3->Eta;
  l3_ph = muon3->Phi;
  const ROOT::Math::PtEtaPhiMVector pL3{Delphes::Momentum(muon3)};

  ms_et = missingET;

  AddMomentumCombinations(pL1, pL2, pL3);

  tree.Fill();
}

void TreeWriter::AddEvent(const GenParticle *const lepton1, const GenParticle *const lepton2,
                          const GenParticle *const lepton3, const double missingET) {
  l1_qe = lepton1->Charge;
  l1_pt = lepton1->PT;
  l1_et = lepton1->Eta;
  l1_ph = lepton1->Phi;
  const auto pL1{Delphes::Momentum(lepton1)};

  l2_qe = lepton2->Charge;
  l2_pt = lepton2->PT;
  l2_et = lepton2->Eta;
  l2_ph = lepton2->Phi;
  const auto pL2{Delphes::Momentum(lepton2)};

  l3_qe = lepton3->Charge;
  l3_pt = lepton3->PT;
  l3_et = lepton3->Eta;
  l3_ph = lepton3->Phi;
  const auto pL3{Delphes::Momentum(lepton3)};

  ms_et = missingET;

  AddMomentumCombinations(pL1, pL2, pL3);

  tree.Fill();
}

void TreeWriter::AddMomentumCombinations(const ROOT::Math::PtEtaPhiMVector &pL1,
                                         const ROOT::Math::PtEtaPhiMVector &pL2,
                                         const ROOT::Math::PtEtaPhiMVector &pL3) {
  l1_l2_dr = ROOT::Math::VectorUtil::DeltaR(pL1, pL2);
  l1_l2_de = pL1.Eta() - pL2.Eta();
  l1_l2_dp = ROOT::Math::VectorUtil::DeltaPhi(pL1, pL2);

  l1_l3_dr = ROOT::Math::VectorUtil::DeltaR(pL1, pL3);
  l1_l3_de = pL1.Eta() - pL3.Eta();
  l1_l3_dp = ROOT::Math::VectorUtil::DeltaPhi(pL1, pL3);

  l2_l3_dr = ROOT::Math::VectorUtil::DeltaR(pL2, pL3);
  l2_l3_de = pL2.Eta() - pL3.Eta();
  l2_l3_dp = ROOT::Math::VectorUtil::DeltaPhi(pL2, pL3);
}
}  // namespace HNL::Trimuon
