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
 * @brief Delphes branch reader for IDEA/FCC-ee detector outputs.
 * @tparam ReaderBase Supported reader backend providing the underlying `TTreeReader`.
 */
template <Reader Base>
class ReaderIDEA : public Base {
  using Base::Base;
  using Base::treeReader;

 public:
  const TTreeReaderArray<class HepMCEvent> Event{treeReader, "Event"};

  const TTreeReaderArray<class GenParticle> Particle{treeReader, "Particle"};
  const TTreeReaderArray<class Vertex> GenVertex{treeReader, "GenVertex"};

  const TTreeReaderArray<class Track> Track{treeReader, "Track"};
  const TTreeReaderArray<class Tower> Tower{treeReader, "Tower"};

  const TTreeReaderArray<class Track> EFlowTrack{treeReader, "EFlowTrack"};
  const TTreeReaderArray<class Tower> EFlowPhoton{treeReader, "EFlowPhoton"};
  const TTreeReaderArray<class Tower> EFlowNeutralHadron{treeReader, "EFlowNeutralHadron"};

  const TTreeReaderArray<class ParticleFlowCandidate> ParticleFlowCandidate{
      treeReader, "ParticleFlowCandidate"};

  const TTreeReaderArray<class Photon> CaloPhoton{treeReader, "CaloPhoton"};
  const TTreeReaderArray<class Photon> PhotonEff{treeReader, "PhotonEff"};
  const TTreeReaderArray<class Photon> PhotonIso{treeReader, "PhotonIso"};

  const TTreeReaderArray<class Jet> GenJet{treeReader, "GenJet"};
  const TTreeReaderArray<class MissingET> GenMissingET{treeReader, "GenMissingET"};

  const TTreeReaderArray<class Jet> Jet{treeReader, "Jet"};
  const TTreeReaderArray<class Electron> Electron{treeReader, "Electron"};
  const TTreeReaderArray<class Photon> Photon{treeReader, "Photon"};
  const TTreeReaderArray<class Muon> Muon{treeReader, "Muon"};

  const TTreeReaderArray<class MissingET> MissingET{treeReader, "MissingET"};

  void Refresh() const final {
    Event.GetSize();

    Particle.GetSize();
    GenVertex.GetSize();

    Track.GetSize();
    Tower.GetSize();

    EFlowTrack.GetSize();
    EFlowPhoton.GetSize();
    EFlowNeutralHadron.GetSize();

    ParticleFlowCandidate.GetSize();

    CaloPhoton.GetSize();
    PhotonEff.GetSize();
    PhotonIso.GetSize();

    GenJet.GetSize();
    GenMissingET.GetSize();

    Jet.GetSize();
    Electron.GetSize();
    Photon.GetSize();
    Muon.GetSize();

    MissingET.GetSize();
  }
};

/** @brief `ReaderIDEA` backed by a ROOT file opened internally. */
using FileReaderIDEA = ReaderIDEA<FileReader>;
/** @brief `ReaderIDEA` bound to a caller-owned `TTreeReader`. */
using TreeReaderIDEA = ReaderIDEA<TreeReader>;
}  // namespace HNL::Delphes
