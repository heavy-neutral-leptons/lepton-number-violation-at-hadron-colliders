// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

// Standard Library
#include <filesystem>

// ROOT
#include "TTreeReaderValue.h"

// HNL
#include "ReconstructedData/TreeReaderBase.hpp"

namespace HNL::Trimuon {
/**
 * @brief ROOT tree reader for the trimuon reconstructed-event schema.
 *
 * The reader exposes the branch layout written by `Trimuon::TreeWriter` for both generator-level
 * and detector-level studies.
 */
class TreeReader final : Reconstruct::TreeReaderBase {
 public:
  /**
   * @brief Open a trimuon reconstructed ROOT file.
   * @param filePath Path to the ROOT file containing the shared `Data` tree.
   */
  explicit TreeReader(const std::filesystem::path& filePath) : TreeReaderBase{filePath.c_str()} {}

  TreeReader(const TreeReader&) = delete;
  TreeReader(TreeReader&&) = delete;
  TreeReader& operator=(const TreeReader&) = delete;
  TreeReader& operator=(TreeReader&&) = delete;

  /** @brief Destroy the reader and close the underlying ROOT file. */
  ~TreeReader() final { Close(); }

  ///@name Single charged lepton
  ///@{
  TTreeReaderValue<int> l1_qe{tr, "l1_qe"};     ///<  Charge of the leading lepton.
  TTreeReaderValue<double> l1_pt{tr, "l1_pt"};  ///<  Transverse momentum of the leading lepton.
  TTreeReaderValue<double> l1_et{tr, "l1_et"};  ///<  Pseudorapidity of the leading lepton.
  TTreeReaderValue<double> l1_ph{tr, "l1_ph"};  ///<  Azimuthal angle of the leading lepton.

  TTreeReaderValue<int> l2_qe{tr, "l2_qe"};     ///<  Charge of the second lepton.
  TTreeReaderValue<double> l2_pt{tr, "l2_pt"};  ///<  Transverse momentum of the second lepton.
  TTreeReaderValue<double> l2_et{tr, "l2_et"};  ///<  Pseudorapidity of the second lepton.
  TTreeReaderValue<double> l2_ph{tr, "l2_ph"};  ///<  Azimuthal angle of the second lepton.

  TTreeReaderValue<int> l3_qe{tr, "l3_qe"};     ///<  Charge of the third lepton.
  TTreeReaderValue<double> l3_pt{tr, "l3_pt"};  ///<  Transverse momentum of the third lepton.
  TTreeReaderValue<double> l3_et{tr, "l3_et"};  ///<  Pseudorapidity of the third lepton.
  TTreeReaderValue<double> l3_ph{tr, "l3_ph"};  ///<  Azimuthal angle of the third lepton.
  ///@}

  ///@name Missing momentum
  ///@{
  TTreeReaderValue<double> missingET{tr, "ms_et"};  ///<  Missing transverse energy.
  ///@}

  ///@name Angular observables
  ///@{
  TTreeReaderValue<double> l1_l2_dr{tr,
                                    "l1_l2_dr"};  ///<  Angular distance between leptons 1 and 2.
  TTreeReaderValue<double> l1_l2_de{
      tr, "l1_l2_de"};  ///<  Pseudorapidity separation between leptons 1 and 2.
  TTreeReaderValue<double> l1_l2_dp{
      tr, "l1_l2_dp"};  ///<  Azimuthal separation between leptons 1 and 2.

  TTreeReaderValue<double> l1_l3_dr{tr,
                                    "l1_l3_dr"};  ///<  Angular distance between leptons 1 and 3.
  TTreeReaderValue<double> l1_l3_de{
      tr, "l1_l3_de"};  ///<  Pseudorapidity separation between leptons 1 and 3.
  TTreeReaderValue<double> l1_l3_dp{
      tr, "l1_l3_dp"};  ///<  Azimuthal separation between leptons 1 and 3.

  TTreeReaderValue<double> l2_l3_dr{tr,
                                    "l2_l3_dr"};  ///<  Angular distance between leptons 2 and 3.
  TTreeReaderValue<double> l2_l3_de{
      tr, "l2_l3_de"};  ///<  Pseudorapidity separation between leptons 2 and 3.
  TTreeReaderValue<double> l2_l3_dp{
      tr, "l2_l3_dp"};  ///<  Azimuthal separation between leptons 2 and 3.
  ///@}
};
}  // namespace HNL::Trimuon
