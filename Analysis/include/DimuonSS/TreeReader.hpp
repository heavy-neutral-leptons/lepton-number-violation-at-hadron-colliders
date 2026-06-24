// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

// Standard Library
#include <filesystem>

// ROOT
#include "TTreeReaderValue.h"

// HNL
#include "ReconstructedData/TreeReaderBase.hpp"

namespace HNL::DimuonSS {
/**
 * @brief ROOT tree reader for the DimuonSS reconstructed-event schema.
 *
 * The reader exposes the branch layout written by `DimuonSS::TreeWriter` for both generator-level
 * and detector-level studies.
 */
class TreeReader final : public Reconstruct::TreeReaderBase {
 public:
  /**
   * @brief Open a DimuonSS reconstructed ROOT file.
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
  TTreeReaderValue<int> l1_qe{tr, "l1_qe"};     ///<  Charge of the prompt lepton.
  TTreeReaderValue<double> l1_pt{tr, "l1_pt"};  ///<  Transverse momentum of the prompt lepton.
  TTreeReaderValue<double> l1_et{tr, "l1_et"};  ///<  Pseudorapidity of the prompt lepton.
  TTreeReaderValue<double> l1_ph{tr, "l1_ph"};  ///<  Azimuthal angle of the prompt lepton.

  TTreeReaderValue<int> l2_qe{tr, "l2_qe"};     ///<  Charge of the displaced lepton.
  TTreeReaderValue<double> l2_pt{tr, "l2_pt"};  ///<  Transverse momentum of the displaced lepton.
  TTreeReaderValue<double> l2_et{tr, "l2_et"};  ///<  Pseudorapidity of the displaced lepton.
  TTreeReaderValue<double> l2_ph{tr, "l2_ph"};  ///<  Azimuthal angle of the displaced lepton.
  ///@}

  ///@name Single jet
  ///@{
  TTreeReaderValue<double> j1_pt{tr, "j1_pt"};  ///<  Transverse momentum of the leading jet.
  TTreeReaderValue<double> j1_et{tr, "j1_et"};  ///<  Pseudorapidity of the leading jet.
  TTreeReaderValue<double> j1_ph{tr, "j1_ph"};  ///<  Azimuthal angle of the leading jet.
  TTreeReaderValue<double> j1_ma{tr, "j1_ma"};  ///<  Invariant mass of the leading jet.

  TTreeReaderValue<double> j2_pt{tr, "j2_pt"};  ///<  Transverse momentum of the subleading jet.
  TTreeReaderValue<double> j2_et{tr, "j2_et"};  ///<  Pseudorapidity of the subleading jet.
  TTreeReaderValue<double> j2_ph{tr, "j2_ph"};  ///<  Azimuthal angle of the subleading jet.
  TTreeReaderValue<double> j2_ma{tr, "j2_ma"};  ///<  Invariant mass of the subleading jet.
  ///@}

  ///@name Combined jets
  ///@{
  TTreeReaderValue<double> j12_pt{tr, "j12_pt"};  ///<  Transverse momentum of the dijet system.
  TTreeReaderValue<double> j12_et{tr, "j12_et"};  ///<  Pseudorapidity of the dijet system.
  TTreeReaderValue<double> j12_ph{tr, "j12_ph"};  ///<  Azimuthal angle of the dijet system.
  TTreeReaderValue<double> j12_ma{tr, "j12_ma"};  ///<  Invariant mass of the dijet system.
  ///@}

  ///@name Missing momentum
  ///@{
  TTreeReaderValue<double> ms_et{tr, "ms_et"};  ///<  Missing transverse energy.
  ///@}

  ///@name Angular observables
  ///@{
  TTreeReaderValue<double> l1_l2_dr{tr,
                                    "l1_l2_dr"};  ///<  Angular distance between leptons 1 and 2.
  TTreeReaderValue<double> l1_l2_de{
      tr, "l1_l2_de"};  ///<  Pseudorapidity separation between leptons 1 and 2.
  TTreeReaderValue<double> l1_l2_dp{
      tr, "l1_l2_dp"};  ///<  Azimuthal separation between leptons 1 and 2.

  TTreeReaderValue<double> l1_j1_dr{tr,
                                    "l1_j1_dr"};  ///<  Angular distance between lepton 1 and jet 1.
  TTreeReaderValue<double> l1_j1_de{
      tr, "l1_j1_de"};  ///<  Pseudorapidity separation between lepton 1 and jet 1.
  TTreeReaderValue<double> l1_j1_dp{
      tr, "l1_j1_dp"};  ///<  Azimuthal separation between lepton 1 and jet 1.
  TTreeReaderValue<double> l1_j2_dr{tr,
                                    "l1_j2_dr"};  ///<  Angular distance between lepton 1 and jet 2.
  TTreeReaderValue<double> l1_j2_de{
      tr, "l1_j2_de"};  ///<  Pseudorapidity separation between lepton 1 and jet 2.
  TTreeReaderValue<double> l1_j2_dp{
      tr, "l1_j2_dp"};  ///<  Azimuthal separation between lepton 1 and jet 2.
  TTreeReaderValue<double> l1_j12_dr{
      tr, "l1_j12_dr"};  ///<  Angular distance between lepton 1 and the dijet system.
  TTreeReaderValue<double> l1_j12_de{
      tr, "l1_j12_de"};  ///<  Pseudorapidity separation between lepton 1 and the dijet system.
  TTreeReaderValue<double> l1_j12_dp{
      tr, "l1_j12_dp"};  ///<  Azimuthal separation between lepton 1 and the dijet system.

  TTreeReaderValue<double> l2_j1_dr{tr,
                                    "l2_j1_dr"};  ///<  Angular distance between lepton 2 and jet 1.
  TTreeReaderValue<double> l2_j1_de{
      tr, "l2_j1_de"};  ///<  Pseudorapidity separation between lepton 2 and jet 1.
  TTreeReaderValue<double> l2_j1_dp{
      tr, "l2_j1_dp"};  ///<  Azimuthal separation between lepton 2 and jet 1.
  TTreeReaderValue<double> l2_j2_dr{tr,
                                    "l2_j2_dr"};  ///<  Angular distance between lepton 2 and jet 2.
  TTreeReaderValue<double> l2_j2_de{
      tr, "l2_j2_de"};  ///<  Pseudorapidity separation between lepton 2 and jet 2.
  TTreeReaderValue<double> l2_j2_dp{
      tr, "l2_j2_dp"};  ///<  Azimuthal separation between lepton 2 and jet 2.
  TTreeReaderValue<double> l2_j12_dr{
      tr, "l2_j12_dr"};  ///<  Angular distance between lepton 2 and the dijet system.
  TTreeReaderValue<double> l2_j12_de{
      tr, "l2_j12_de"};  ///<  Pseudorapidity separation between lepton 2 and the dijet system.
  TTreeReaderValue<double> l2_j12_dp{
      tr, "l2_j12_dp"};  ///<  Azimuthal separation between lepton 2 and the dijet system.

  TTreeReaderValue<double> j1_j2_dr{tr, "j1_j2_dr"};  ///<  Angular distance between the two jets.
  TTreeReaderValue<double> j1_j2_de{
      tr, "j1_j2_de"};  ///<  Pseudorapidity separation between the two jets.
  TTreeReaderValue<double> j1_j2_dp{tr,
                                    "j1_j2_dp"};  ///<  Azimuthal separation between the two jets.
  ///@}
};
}  // namespace HNL::DimuonSS
