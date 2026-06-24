// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

// Standard Library
#include <filesystem>

// ROOT
#include "Math/Vector4Dfwd.h"

// Delphes
#include "classes/DelphesClasses.h"

// HNL
#include "Reconstruction/TreeWriterBase.hpp"

namespace HNL::DimuonSS {
/**
 * @brief Writes DimuonSS observables to a ROOT tree.
 *
 * The writer is shared by generator-level and detector-level FCC-hh analyses.
 * It stores the leading leptons, the dijet system, missing transverse energy, and the angular
 * combinations used by downstream summary scripts.
 */
class TreeWriter : public Reconstruct::TreeWriterBase {
 public:
  TreeWriter(const TreeWriter &) = delete;
  TreeWriter(TreeWriter &&) = delete;
  TreeWriter &operator=(const TreeWriter &) = delete;
  TreeWriter &operator=(TreeWriter &&) = delete;

  /**
   * @brief Create an output tree.
   * @param filePath ROOT file written on destruction.
   * @param treeName Name used for the in-file TTree.
   */
  explicit TreeWriter(const std::filesystem::path &filePath, const char *treeName);
  ~TreeWriter();

  /**
   * @brief Append one detector-level event.
   * @param muon1 Hardest reconstructed muon.
   * @param muon2 Second hardest reconstructed muon.
   * @param jet1 Hardest reconstructed jet.
   * @param jet2 Second hardest reconstructed jet.
   * @param missingET Event missing transverse energy.
   */
  void AddEvent(const Muon *muon1, const Muon *muon2, const Jet *jet1, const Jet *jet2,
                double missingET);

  /**
   * @brief Append one generator-level event.
   * @param leptonProd Charged lepton produced with the HNL.
   * @param leptonDecay Charged lepton produced in the HNL decay.
   * @param quark1 Leading quark from the HNL decay.
   * @param quark2 Subleading quark from the HNL decay.
   */
  void AddEvent(const GenParticle *leptonProd, const GenParticle *leptonDecay,
                const GenParticle *quark1, const GenParticle *quark2);

 private:
  /**
   * @brief Fill the pairwise angular observables and combined dijet kinematics.
   */
  void AddMomentumCombinations(const ROOT::Math::PtEtaPhiMVector &pL1,
                               const ROOT::Math::PtEtaPhiMVector &pL2,
                               const ROOT::Math::PtEtaPhiMVector &pJ1,
                               const ROOT::Math::PtEtaPhiMVector &pJ2);

 public:
  ///@name Single charged lepton
  ///@{
  int l1_qe{0};     ///<  Charge of the prompt lepton.
  double l1_pt{0};  ///<  Transverse momentum of the prompt lepton.
  double l1_et{0};  ///<  Pseudorapidity of the prompt lepton.
  double l1_ph{0};  ///<  Azimuthal angle of the prompt lepton.

  int l2_qe{0};     ///<  Charge of the displaced lepton.
  double l2_pt{0};  ///<  Transverse momentum of the displaced lepton.
  double l2_et{0};  ///<  Pseudorapidity of the displaced lepton.
  double l2_ph{0};  ///<  Azimuthal angle of the displaced lepton.
  ///@}

  ///@name Single jet
  ///@{
  double j1_pt{0};  ///<  Transverse momentum of the leading jet.
  double j1_et{0};  ///<  Pseudorapidity of the leading jet.
  double j1_ph{0};  ///<  Azimuthal angle of the leading jet.
  double j1_ma{0};  ///<  Invariant mass of the leading jet.

  double j2_pt{0};  ///<  Transverse momentum of the subleading jet.
  double j2_et{0};  ///<  Pseudorapidity of the subleading jet.
  double j2_ph{0};  ///<  Azimuthal angle of the subleading jet.
  double j2_ma{0};  ///<  Invariant mass of the subleading jet.
  ///@}

  ///@name Combined jets
  ///@{
  double j12_pt{0};  ///<  Transverse momentum of the dijet system.
  double j12_et{0};  ///<  Pseudorapidity of the dijet system.
  double j12_ph{0};  ///<  Azimuthal angle of the dijet system.
  double j12_ma{0};  ///<  Invariant mass of the dijet system.
  ///@}

  ///@name Missing momentum
  ///@{
  double ms_et{0};  ///<  Missing transverse energy.
  ///@}

  ///@name Angular observables
  ///@{
  double l1_l2_dr{0};  ///<  Angular distance between leptons 1 and 2.
  double l1_l2_de{0};  ///<  Pseudorapidity separation between leptons 1 and 2.
  double l1_l2_dp{0};  ///<  Azimuthal separation between leptons 1 and 2.

  double l1_j1_dr{0};   ///<  Angular distance between lepton 1 and jet 1.
  double l1_j1_de{0};   ///<  Pseudorapidity separation between lepton 1 and jet 1.
  double l1_j1_dp{0};   ///<  Azimuthal separation between lepton 1 and jet 1.
  double l1_j2_dr{0};   ///<  Angular distance between lepton 1 and jet 2.
  double l1_j2_de{0};   ///<  Pseudorapidity separation between lepton 1 and jet 2.
  double l1_j2_dp{0};   ///<  Azimuthal separation between lepton 1 and jet 2.
  double l1_j12_dr{0};  ///<  Angular distance between lepton 1 and the dijet system.
  double l1_j12_de{0};  ///<  Pseudorapidity separation between lepton 1 and the dijet system.
  double l1_j12_dp{0};  ///<  Azimuthal separation between lepton 1 and the dijet system.

  double l2_j1_dr{0};   ///<  Angular distance between lepton 2 and jet 1.
  double l2_j1_de{0};   ///<  Pseudorapidity separation between lepton 2 and jet 1.
  double l2_j1_dp{0};   ///<  Azimuthal separation between lepton 2 and jet 1.
  double l2_j2_dr{0};   ///<  Angular distance between lepton 2 and jet 2.
  double l2_j2_de{0};   ///<  Pseudorapidity separation between lepton 2 and jet 2.
  double l2_j2_dp{0};   ///<  Azimuthal separation between lepton 2 and jet 2.
  double l2_j12_dr{0};  ///<  Angular distance between lepton 2 and the dijet system.
  double l2_j12_de{0};  ///<  Pseudorapidity separation between lepton 2 and the dijet system.
  double l2_j12_dp{0};  ///<  Azimuthal separation between lepton 2 and the dijet system.

  double j1_j2_dr{0};  ///<  Angular distance between the two jets.
  double j1_j2_de{0};  ///<  Pseudorapidity separation between the two jets.
  double j1_j2_dp{0};  ///<  Azimuthal separation between the two jets.
  ///@}
};
}  // namespace HNL::DimuonSS
