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
 * @brief Delphes branch reader for ATLAS detector outputs.
 * @tparam ReaderBase Supported reader backend providing the underlying `TTreeReader`.
 */
template <Reader Base>
class ReaderATLAS : public Base {
  using Base::Base;
  using Base::treeReader;

 public:
  const TTreeReaderArray<class HepMCEvent> Event{treeReader, "Event"};

  const TTreeReaderArray<class GenParticle> Particle{treeReader, "Particle"};

  const TTreeReaderArray<class Track> Track{treeReader, "Track"};
  const TTreeReaderArray<class Tower> Tower{treeReader, "Tower"};

  const TTreeReaderArray<class Track> EFlowTrack{treeReader, "EFlowTrack"};
  const TTreeReaderArray<class Tower> EFlowPhoton{treeReader, "EFlowPhoton"};
  const TTreeReaderArray<class Tower> EFlowNeutralHadron{treeReader, "EFlowNeutralHadron"};

  const TTreeReaderArray<class Jet> GenJet{treeReader, "GenJet"};
  const TTreeReaderArray<class MissingET> GenMissingET{treeReader, "GenMissingET"};

  const TTreeReaderArray<class Jet> Jet{treeReader, "Jet"};
  const TTreeReaderArray<class Electron> Electron{treeReader, "Electron"};
  const TTreeReaderArray<class Photon> Photon{treeReader, "Photon"};
  const TTreeReaderArray<class Muon> Muon{treeReader, "Muon"};
  const TTreeReaderArray<class MissingET> MissingET{treeReader, "MissingET"};
  const TTreeReaderArray<class ScalarHT> ScalarHT{treeReader, "ScalarHT"};

  void Refresh() const final {
    Event.GetSize();

    Particle.GetSize();

    Track.GetSize();
    Tower.GetSize();

    EFlowTrack.GetSize();
    EFlowPhoton.GetSize();
    EFlowNeutralHadron.GetSize();

    GenJet.GetSize();
    GenMissingET.GetSize();

    Jet.GetSize();
    Electron.GetSize();
    Photon.GetSize();
    Muon.GetSize();
    MissingET.GetSize();
    ScalarHT.GetSize();
  }
};

/** @brief `ReaderATLAS` backed by a ROOT file opened internally. */
using FileReaderATLAS = ReaderATLAS<FileReader>;
/** @brief `ReaderATLAS` bound to a caller-owned `TTreeReader`. */
using TreeReaderATLAS = ReaderATLAS<TreeReader>;
}  // namespace HNL::Delphes
