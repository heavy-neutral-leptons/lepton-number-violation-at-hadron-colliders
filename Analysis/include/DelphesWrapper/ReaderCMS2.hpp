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
 * @brief Delphes branch reader for the CMS Phase-2 style detector card.
 * @tparam ReaderBase Supported reader backend providing the underlying `TTreeReader`.
 */
template <Reader Base>
class ReaderCMS2 : public Base {
  using Base::Base;
  using Base::treeReader;

 public:
  const TTreeReaderArray<class HepMCEvent> Event{treeReader, "Event"};
  const TTreeReaderArray<class Weight> Weight{treeReader, "Weight"};

  const TTreeReaderArray<class GenParticle> Particle{treeReader, "Particle"};
  const TTreeReaderArray<class Vertex> Vertex{treeReader, "Vertex"};

  const TTreeReaderArray<class Jet> GenJet{treeReader, "GenJet"};
  const TTreeReaderArray<class Jet> GenJetAK8{treeReader, "GenJetAK8"};
  const TTreeReaderArray<class MissingET> GenMissingET{treeReader, "GenMissingET"};

  const TTreeReaderArray<class Track> EFlowTrack{treeReader, "EFlowTrack"};

  const TTreeReaderArray<class Photon> Photon{treeReader, "Photon"};
  const TTreeReaderArray<class Electron> Electron{treeReader, "Electron"};
  const TTreeReaderArray<class Muon> MuonLoose{treeReader, "MuonLoose"};
  const TTreeReaderArray<class Muon> MuonTight{treeReader, "MuonTight"};

  const TTreeReaderArray<class Photon> PhotonCHS{treeReader, "PhotonCHS"};
  const TTreeReaderArray<class Electron> ElectronCHS{treeReader, "ElectronCHS"};
  const TTreeReaderArray<class Muon> MuonLooseCHS{treeReader, "MuonLooseCHS"};
  const TTreeReaderArray<class Muon> MuonTightCHS{treeReader, "MuonTightCHS"};

  const TTreeReaderArray<class Jet> Jet{treeReader, "Jet"};
  const TTreeReaderArray<class Jet> JetPUPPI{treeReader, "JetPUPPI"};
  const TTreeReaderArray<class Jet> JetAK8{treeReader, "JetAK8"};
  const TTreeReaderArray<class Jet> JetPUPPIAK8{treeReader, "JetPUPPIAK8"};

  const TTreeReaderArray<class Rho> Rho{treeReader, "Rho"};

  const TTreeReaderArray<class MissingET> MissingET{treeReader, "MissingET"};
  const TTreeReaderArray<class MissingET> PuppiMissingET{treeReader, "PuppiMissingET"};
  const TTreeReaderArray<class MissingET> GenPileUpMissingET{treeReader, "GenPileUpMissingET"};
  const TTreeReaderArray<class ScalarHT> ScalarHT{treeReader, "ScalarHT"};

  void GetSize() const final {
    Event.GetSize();
    Weight.GetSize();

    Particle.GetSize();
    Vertex.GetSize();

    GenJet.GetSize();
    GenJetAK8.GetSize();
    GenMissingET.GetSize();

    EFlowTrack.GetSize();

    Photon.GetSize();
    Electron.GetSize();
    MuonLoose.GetSize();
    MuonTight.GetSize();

    PhotonCHS.GetSize();
    ElectronCHS.GetSize();
    MuonLooseCHS.GetSize();
    MuonTightCHS.GetSize();

    Jet.GetSize();
    JetPUPPI.GetSize();
    JetAK8.GetSize();
    JetPUPPIAK8.GetSize();

    Rho.GetSize();

    MissingET.GetSize();
    PuppiMissingET.GetSize();
    GenPileUpMissingET.GetSize();
    ScalarHT.GetSize();
  }
};

/** @brief `ReaderCMS2` backed by a ROOT file opened internally. */
using FileReaderCMS2 = ReaderCMS2<FileReader>;
/** @brief `ReaderCMS2` bound to a caller-owned `TTreeReader`. */
using TreeReaderCMS2 = ReaderCMS2<TreeReader>;
}  // namespace HNL::Delphes
