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
 * @brief Minimal Delphes branch reader exposing generator-level event content.
 * @tparam ReaderBase Supported reader backend providing the underlying `TTreeReader`.
 */
template <Reader Base>
class ReaderParticle : public Base {
  using Base::Base;
  using Base::treeReader;

 public:
  const TTreeReaderArray<class HepMCEvent> Event{treeReader, "Event"};
  const TTreeReaderArray<class Weight> Weight{treeReader, "Weight"};

  const TTreeReaderArray<class GenParticle> Particle{treeReader, "Particle"};

  void Refresh() const final {
    Event.GetSize();
    Weight.GetSize();

    Particle.GetSize();
  }
};

/** @brief `ReaderParticle` backed by a ROOT file opened internally. */
using FileReaderParticle = ReaderParticle<FileReader>;
/** @brief `ReaderParticle` bound to a caller-owned `TTreeReader`. */
using TreeReaderParticle = ReaderParticle<TreeReader>;
}  // namespace HNL::Delphes
