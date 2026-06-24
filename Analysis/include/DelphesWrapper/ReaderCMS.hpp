// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

// ROOT
#include "TTreeReaderArray.h"

// Delphes
#include "classes/DelphesClasses.h"

// HNL
#include "DelphesWrapper/ReaderBase.hpp"

namespace HNL::Delphes {
/**
 * @brief Delphes branch reader for CMS detector outputs.
 * @tparam ReaderBase Supported reader backend providing the underlying `TTreeReader`.
 */
template <Reader Base>
class ReaderCMS : public Base {
  using Base::Base;
  using Base::treeReader;

 public:
  const TTreeReaderArray<class HepMCEvent> Event{treeReader, "Event"};

  const TTreeReaderArray<class GenParticle> Particle{treeReader, "Particle"};

  const TTreeReaderArray<class Jet> GenJet{treeReader, "GenJet"};
  const TTreeReaderArray<class MissingET> GenMissingJet{treeReader, "GenMissingET"};

  const TTreeReaderArray<class Track> EFlowTrack{treeReader, "EFlowTrack"};
  const TTreeReaderArray<class Tower> EFlowPhoton{treeReader, "EFlowPhoton"};
  const TTreeReaderArray<class Tower> EFlowNeutralHadron{treeReader, "EFlowNeutralHadron"};

  const TTreeReaderArray<class Photon> Photon{treeReader, "Photon"};
  const TTreeReaderArray<class Electron> Electron{treeReader, "Electron"};
  const TTreeReaderArray<class Muon> MuonLoose{treeReader, "MuonLoose"};
  const TTreeReaderArray<class Muon> MuonTight{treeReader, "MuonTight"};

  const TTreeReaderArray<class Jet> Jet{treeReader, "Jet"};
  const TTreeReaderArray<class Jet> FatJet{treeReader, "FatJet"};

  const TTreeReaderArray<class MissingET> MissingET{treeReader, "MissingET"};
  const TTreeReaderArray<class ScalarHT> ScalarHT{treeReader, "ScalarHT"};

  void Refresh() const final {
    Event.GetSize();

    Particle.GetSize();

    GenJet.GetSize();
    GenMissingJet.GetSize();

    EFlowTrack.GetSize();
    EFlowPhoton.GetSize();
    EFlowNeutralHadron.GetSize();

    Photon.GetSize();
    Electron.GetSize();
    MuonLoose.GetSize();
    MuonTight.GetSize();

    Jet.GetSize();
    FatJet.GetSize();

    MissingET.GetSize();
    ScalarHT.GetSize();
  }
};

/** @brief `ReaderCMS` backed by a ROOT file opened internally. */
using FileReaderCMS = ReaderCMS<FileReader>;
/** @brief `ReaderCMS` bound to a caller-owned `TTreeReader`. */
using TreeReaderCMS = ReaderCMS<TreeReader>;
}  // namespace HNL::Delphes
