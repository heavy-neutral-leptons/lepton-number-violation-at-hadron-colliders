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
 * @brief Delphes branch reader for FCC-hh detector outputs.
 * @tparam ReaderBase Supported reader backend providing the underlying `TTreeReader`.
 */
template <Reader Base>
class ReaderFCChh : public Base {
  using Base::Base;
  using Base::treeReader;

 public:
  const TTreeReaderArray<class HepMCEvent> Event{treeReader, "Event"};
  // const TTreeReaderArray<class Weight> Weight{treeReader, "Weight"};

  const TTreeReaderArray<class GenParticle> Particle{treeReader, "Particle"};

  const TTreeReaderArray<class MissingET> GenMissingET{treeReader, "GenMissingET"};
  const TTreeReaderArray<class Jet> GenJet{treeReader, "GenJet"};

  const TTreeReaderArray<class Track> Track{treeReader, "Track"};
  const TTreeReaderArray<class Tower> Tower{treeReader, "Tower"};

  const TTreeReaderArray<class Track> EFlowTrack{treeReader, "EFlowTrack"};
  const TTreeReaderArray<class Tower> EFlowPhoton{treeReader, "EFlowPhoton"};
  const TTreeReaderArray<class Tower> EFlowNeutralHadron{treeReader, "EFlowNeutralHadron"};

  const TTreeReaderArray<class Photon> Photon{treeReader, "Photon"};
  const TTreeReaderArray<class Electron> Electron{treeReader, "Electron"};
  const TTreeReaderArray<class Muon> Muon{treeReader, "Muon"};
  const TTreeReaderArray<class Jet> Jet{treeReader, "Jet"};

  const TTreeReaderArray<class Jet> GenJet02{treeReader, "GenJet02"};
  const TTreeReaderArray<class Jet> GenJet04{treeReader, "GenJet04"};
  const TTreeReaderArray<class Jet> GenJet08{treeReader, "GenJet08"};
  const TTreeReaderArray<class Jet> GenJet15{treeReader, "GenJet15"};

  const TTreeReaderArray<class Jet> ParticleFlowJet02{treeReader, "ParticleFlowJet02"};
  const TTreeReaderArray<class Jet> ParticleFlowJet04{treeReader, "ParticleFlowJet04"};
  const TTreeReaderArray<class Jet> ParticleFlowJet08{treeReader, "ParticleFlowJet08"};
  const TTreeReaderArray<class Jet> ParticleFlowJet15{treeReader, "ParticleFlowJet15"};

  const TTreeReaderArray<class Jet> CaloJet02{treeReader, "CaloJet02"};
  const TTreeReaderArray<class Jet> CaloJet04{treeReader, "CaloJet04"};
  const TTreeReaderArray<class Jet> CaloJet08{treeReader, "CaloJet08"};
  const TTreeReaderArray<class Jet> CaloJet15{treeReader, "CaloJet15"};

  const TTreeReaderArray<class Jet> TrackJet02{treeReader, "TrackJet02"};
  const TTreeReaderArray<class Jet> TrackJet04{treeReader, "TrackJet04"};
  const TTreeReaderArray<class Jet> TrackJet08{treeReader, "TrackJet08"};
  const TTreeReaderArray<class Jet> TrackJet15{treeReader, "TrackJet15"};

  const TTreeReaderArray<class MissingET> MissingET{treeReader, "MissingET"};
  const TTreeReaderArray<class ScalarHT> ScalarHT{treeReader, "ScalarHT"};

  void Refresh() const final {
    Event.GetSize();
    // Weight.GetSize();

    Particle.GetSize();

    GenMissingET.GetSize();
    GenJet.GetSize();

    Track.GetSize();
    Tower.GetSize();

    EFlowTrack.GetSize();
    EFlowPhoton.GetSize();
    EFlowNeutralHadron.GetSize();

    Photon.GetSize();
    Electron.GetSize();
    Muon.GetSize();
    Jet.GetSize();

    GenJet02.GetSize();
    GenJet04.GetSize();
    GenJet08.GetSize();
    GenJet15.GetSize();

    ParticleFlowJet02.GetSize();
    ParticleFlowJet04.GetSize();
    ParticleFlowJet08.GetSize();
    ParticleFlowJet15.GetSize();

    CaloJet02.GetSize();
    CaloJet04.GetSize();
    CaloJet08.GetSize();
    CaloJet15.GetSize();

    TrackJet02.GetSize();
    TrackJet04.GetSize();
    TrackJet08.GetSize();
    TrackJet15.GetSize();

    MissingET.GetSize();
    ScalarHT.GetSize();
  }
};

/** @brief `ReaderFCChh` backed by a ROOT file opened internally. */
using FileReaderFCChh = ReaderFCChh<FileReader>;
/** @brief `ReaderFCChh` bound to a caller-owned `TTreeReader`. */
using TreeReaderFCChh = ReaderFCChh<TreeReader>;
}  // namespace HNL::Delphes
