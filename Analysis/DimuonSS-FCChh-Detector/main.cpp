// Copyright 2023-2026 Bruno M. S. Oliveira

// Standard Library
#include <array>
#include <filesystem>
#include <string>

// CLI11
#include "CLI/CLI.hpp"

// Delphes
#include "classes/DelphesClasses.h"

// HNL
#define LOG_CUT false

#include "Common/Log.hpp"
#include "DelphesWrapper/DelphesPosition.hpp"
#include "DelphesWrapper/ReaderFCChh.hpp"
#include "MG5_aMC/MadgraphData.hpp"
#include "DimuonSS/TreeWriter.hpp"
#include "ReconstructedData/ReconstructedEventDataKeys.hpp"
#include "Reconstruction/SurvivalRate.hpp"

using DelphesReader = HNL::Delphes::FileReaderFCChh;
using TreeWriter = HNL::DimuonSS::TreeWriter;

namespace {
enum class Cut : uint8_t {
  // Reconstructed
  generatorLNV,
  reconstructed,
  // Cuts
  jetTopology,
  muonCharge,
  muonTopology,
  muonTopologyDelphes,
  missingEnergy,
  electronTopology,
  // Simulated
  simulated,
};

inline constexpr size_t nCuts{std::to_underlying(Cut::simulated)};

constexpr std::string_view ToString(const Cut cut) {
  switch (cut) {
    using enum Cut;
    // Reconstructed
    case generatorLNV:
      return "Generator level LNV";
    case reconstructed:
      return "Reconstructed      ";
    // Cuts
    case Cut::jetTopology:
      return "Jet topology       ";
    case muonCharge:
      return "Same sign muons    ";
    case muonTopology:
      return "Muon topology      ";
    case muonTopologyDelphes:
      return "Muon topology (Delphes)";
    case missingEnergy:
      return "Missing energy     ";
    case electronTopology:
      return "Electron topology  ";
    // Simulated
    case simulated:
      return "Simulated          ";
  }

  return "Error";
}

inline Cut ProcessEvent(const DelphesReader& delphesReader, TreeWriter& treeWriter) {
  delphesReader.Refresh();

  /* ----- Electrons ----- */
  size_t nElectrons{0};
  for (const Electron& electron : delphesReader.Electron) {
    if (electron.PT >= 20) ++nElectrons;
  }

  if (nElectrons != 0) {
    LogCut(ToString(Cut::electronTopology), "{}/{} electrons", nElectrons,
           delphesReader.Electron.GetSize());

    return Cut::electronTopology;
  }

  /* ----- Missing transverse energy ----- */
  double missingET{0};
  for (const MissingET& entry : delphesReader.MissingET) {
    missingET += entry.MET;
  }

  // if (missingET > 40) {
  //   LogCut(ToString(Cut::missingEnergy),
  //          "Missing transverse energy = {} GeV ({})", missingET,
  //          delphesReader.MissingET.GetSize());
  //
  //   return Cut::missingEnergy;
  // }

  /* ----- Muons ----- */
  if (delphesReader.Muon.GetSize() < 2) {
    LogCut(ToString(Cut::muonTopology), "{} muons", delphesReader.Muon.GetSize());

    return Cut::muonTopologyDelphes;
  }

  size_t nMuons{0};
  const Muon* muons[2]{nullptr, nullptr};  // Two highest PT muons

  for (const Muon& muon : delphesReader.Muon) {
    if (muon.PT < 20) continue;
    if (HNL::FCChh::Tracker(muon) != HNL::TrackerPosition::prompt) continue;

    if (muons[0] == nullptr || muon.PT > muons[0]->PT) {
      muons[1] = muons[0];
      muons[0] = &muon;
    } else if (muons[1] == nullptr || muon.PT > muons[1]->PT) {
      muons[1] = &muon;
    }

    ++nMuons;
  }

  // Topology
  if (nMuons != 2) {
    LogCut(ToString(Cut::muonTopology), "{}/{} muons", nMuons, delphesReader.Muon.GetSize());

    return Cut::muonTopology;
  }

  // Same sign
  if (muons[0]->Charge != muons[1]->Charge) {
    LogCut(ToString(Cut::muonCharge));

    return Cut::muonCharge;
  }

  /* ----- Jets ----- */
  if (delphesReader.Jet.GetSize() < 2) {
    LogCut(ToString(Cut::jetTopology), "{} jets", delphesReader.Jet.GetSize());

    return Cut::jetTopology;
  }

  const Jet* jets[2]{nullptr, nullptr};  // Two highest PT jets
  for (const Jet& jet : delphesReader.Jet) {
    if (jets[0] == nullptr || jet.PT > jets[0]->PT) {
      jets[1] = jets[0];
      jets[0] = &jet;
    } else if (jets[1] == nullptr || jet.PT > jets[1]->PT) {
      jets[1] = &jet;
    }
  }

  /* ----- Fill Tree ----- */
  treeWriter.AddEvent(muons[0], muons[1], jets[0], jets[1], missingET);
  return Cut::reconstructed;
}
}  // namespace

int main(const int argc, const char* const argv[]) {
  /* ----- CLI Arguments ----- */
  CLI::App app{};

  std::string simulation{};
  app.add_option("--name", simulation, "Simulation name")->required();

  bool singleRun{};
  app.add_flag("--singlerun", singleRun, "Assume single run simulation");

  int runId{};
  app.add_option("--run", runId, "run")->required();

  int tagId{};
  app.add_option("--tag", tagId, "tag")->required();

  double crossSectionFactor{1.};
  app.add_option("--cs-factor", crossSectionFactor, "Cross section factor")->default_val(1.);

  try {
    app.parse(argc, argv);
  } catch (const CLI::ParseError& e) {
    return app.exit(e);
  }

  /* ----- Event Reconstruction ----- */
  constexpr std::string_view simDir{"DimuonSS@FCChh"};
  HNL::MG5::MadgraphData madgraphData{simDir, simulation, runId, tagId};

  const std::filesystem::path outPath{madgraphData.OutputTreePath(!singleRun)};
  if (std::filesystem::exists(outPath)) {
    LogWarn("{} already exists.", outPath.c_str());
    return 0;
  }

  DelphesReader delphesReader{madgraphData.eventsPath};
  const int64_t nEventsSimulated{delphesReader.GetEntries()};

  TreeWriter treeWriter{outPath, HNL::Reconstruct::eventTree};

  std::array<size_t, nCuts> nEventsCut{};
  while (delphesReader.Next()) {
    const Cut cut{ProcessEvent(delphesReader, treeWriter)};
    ++nEventsCut[std::to_underlying(cut)];
  }

  /* ----- CLI Output ----- */
  const int64_t nEventsReconstructed{treeWriter.GetEntries()};

  const double survivalRate{HNL::Reconstruct::SurvivalRate(nEventsReconstructed, nEventsSimulated)};

  if (crossSectionFactor != 1) madgraphData.crossSection *= crossSectionFactor;
  treeWriter.WriteMadgraphData(madgraphData, survivalRate);

  std::print("{},", madgraphData.CSV());
  std::print("{},", nEventsSimulated);
  std::print("{},", nEventsCut[std::to_underlying(Cut::electronTopology)]);
  std::print("{},", nEventsCut[std::to_underlying(Cut::muonTopologyDelphes)]);
  std::print("{},", nEventsCut[std::to_underlying(Cut::muonTopology)]);
  std::print("{},", nEventsCut[std::to_underlying(Cut::muonCharge)]);
  std::print("{},", nEventsCut[std::to_underlying(Cut::jetTopology)]);
  std::print("{},", nEventsCut[std::to_underlying(Cut::reconstructed)]);
  std::print("{},", nEventsReconstructed);
  std::print("{}\n", survivalRate * madgraphData.crossSection);

  return 0;
}
