// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

// Standard Library
#include <filesystem>

// Delphes
#include "classes/DelphesClasses.h"

// HNL
#include "Reconstruction/TreeWriterBase.hpp"

namespace HNL::Trimuon {
/**
 * @brief Writes trimuon three-muon observables to a ROOT tree.
 *
 * The same schema is used by the generator-level and detector-level studies.
 */
class TreeWriter : public Reconstruct::TreeWriterBase {
 public:
  /**
   * @brief Create an output tree.
   * @param filePath ROOT file written on destruction.
   * @param treeName Name used for the in-file TTree.
   */
  explicit TreeWriter(const std::filesystem::path &filePath, const char *treeName);

  TreeWriter(const TreeWriter &) = delete;
  TreeWriter(TreeWriter &&) = delete;
  TreeWriter &operator=(const TreeWriter &) = delete;
  TreeWriter &operator=(TreeWriter &&) = delete;

  ~TreeWriter() { Close(); }

  /**
   * @brief Append one detector-level three-muon event.
   * @param muon1 Leading reconstructed muon.
   * @param muon2 Subleading reconstructed muon.
   * @param muon3 Third reconstructed muon.
   * @param missingET Event missing transverse energy.
   */
  void AddEvent(const Muon *muon1, const Muon *muon2, const Muon *muon3, double missingET);

  /**
   * @brief Append one generator-level three-muon event.
   * @param lepton1 Leading generator-level charged lepton.
   * @param lepton2 Subleading generator-level charged lepton.
   * @param lepton3 Third generator-level charged lepton.
   * @param missingET Event missing transverse energy.
   */
  void AddEvent(const GenParticle *lepton1, const GenParticle *lepton2, const GenParticle *lepton3,
                double missingET);

 private:
  /**
   * @brief Fill the pairwise angular observables between the three leptons.
   */
  void AddMomentumCombinations(const ROOT::Math::PtEtaPhiMVector &pL1,
                               const ROOT::Math::PtEtaPhiMVector &pL2,
                               const ROOT::Math::PtEtaPhiMVector &pL3);

 public:
  ///@name Single charged lepton
  ///@{
  int l1_qe{0};     ///<  Charge of the leading lepton.
  double l1_pt{0};  ///<  Transverse momentum of the leading lepton.
  double l1_et{0};  ///<  Pseudorapidity of the leading lepton.
  double l1_ph{0};  ///<  Azimuthal angle of the leading lepton.

  int l2_qe{0};     ///<  Charge of the second lepton.
  double l2_pt{0};  ///<  Transverse momentum of the second lepton.
  double l2_et{0};  ///<  Pseudorapidity of the second lepton.
  double l2_ph{0};  ///<  Azimuthal angle of the second lepton.

  int l3_qe{0};     ///<  Charge of the third lepton.
  double l3_pt{0};  ///<  Transverse momentum of the third lepton.
  double l3_et{0};  ///<  Pseudorapidity of the third lepton.
  double l3_ph{0};  ///<  Azimuthal angle of the third lepton.
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

  double l1_l3_dr{0};  ///<  Angular distance between leptons 1 and 3.
  double l1_l3_de{0};  ///<  Pseudorapidity separation between leptons 1 and 3.
  double l1_l3_dp{0};  ///<  Azimuthal separation between leptons 1 and 3.

  double l2_l3_dr{0};  ///<  Angular distance between leptons 2 and 3.
  double l2_l3_de{0};  ///<  Pseudorapidity separation between leptons 2 and 3.
  double l2_l3_dp{0};  ///<  Azimuthal separation between leptons 2 and 3.
  ///@}
};
}  // namespace HNL::Trimuon
