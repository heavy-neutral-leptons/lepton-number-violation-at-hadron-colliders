// Copyright 2023-2026 Bruno M. S. Oliveira

// Standard Library
#include <filesystem>
#include <ranges>
#include <string>

// CLI11
#include "CLI/CLI.hpp"

// Delphes
#include "classes/DelphesClasses.h"

// HNL
#define LOG_CUT true

#include "Common/Log.hpp"
#include "DelphesWrapper/DelphesVector4D.hpp"
#include "DelphesWrapper/ReaderParticle.hpp"
#include "DelphesWrapper/TreeNavigation.hpp"
#include "Trimuon/TreeWriter.hpp"
#include "MCPID/SM.hpp"
#include "MG5_aMC/MadgraphData.hpp"
#include "ReconstructedData/ReconstructedEventDataKeys.hpp"
#include "Reconstruction/SurvivalRate.hpp"

using DelphesReader = HNL::Delphes::FileReaderParticle;
using TreeWriter = HNL::Trimuon::TreeWriter;

namespace {
inline void ProcessEvent(DelphesReader& delphesReader, TreeWriter& treeWriter,
                         const double minMuonPt, const double /*minJetPt*/,
                         const double minDeltaR) {
  const GenParticle* muons[3]{nullptr, nullptr};
  size_t nMuons{0};

  double missingET{0};

  HNL::Delphes::ParticleTreeWalker ptw{delphesReader.Particle};
  for (const auto& [idx, particle] : delphesReader.Particle | std::views::enumerate) {
    const HNL::Delphes::ParticleFamily family{ptw.Family(idx)};

    // Logging
    Log<1>("Particle {} ({}): {}", idx, particle.PID, particle.Status);
    if (family.Produced()) Log<1>("\t{}", family.Production());
    if (family.Decayed()) Log<1>("\t{}", family.Decay());

    // Identify Particle
    if (particle.Status == -1) continue;

    if (HNL::PID::SM::IsMuon(particle.PID)) {
      Log<1>("\tIdentity  : Muon");

      if (nMuons < 3) muons[nMuons] = &particle;
      ++nMuons;
    }
  }

  if (nMuons != 3 || muons[0] == nullptr || muons[1] == nullptr || muons[2] == nullptr) {
    LogCut("Miss ID", "{}", nMuons);
    return;
  }

  std::ranges::sort(
      muons, [](const GenParticle* lhs, const GenParticle* rhs) { return lhs->PT > rhs->PT; });

  if (muons[0]->PT < minMuonPt && muons[1]->PT < minMuonPt && muons[2]->PT < minMuonPt) {
    LogCut("Muon PT", "{} {} {}", muons[0]->PT, muons[1]->PT, muons[2]->PT);
    return;
  }

  if (HNL::Delphes::DeltaR(muons[0], muons[1]) < minDeltaR ||
      HNL::Delphes::DeltaR(muons[0], muons[2]) < minDeltaR ||
      HNL::Delphes::DeltaR(muons[1], muons[2]) < minDeltaR) {
    LogCut("ΔR", "{} {} {}",  //
           HNL::Delphes::DeltaR(muons[0], muons[1]), HNL::Delphes::DeltaR(muons[0], muons[2]),
           HNL::Delphes::DeltaR(muons[1], muons[2]));
    return;
  }

  treeWriter.AddEvent(muons[0], muons[1], muons[2], missingET);

  Log<1>("");
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

  double minMuonPt{};
  app.add_option("--min-muon-pt", minMuonPt, "Minimum muon transverse momentum")->default_val(0.);

  double minJetPt{};
  app.add_option("--min-jet-pt", minJetPt, "Minimum jet transverse momentum")->default_val(0.);

  double minDeltaR{};
  app.add_option("--min-delta-r", minDeltaR, "Minimum ΔR")->default_val(0.);

  try {
    app.parse(argc, argv);
  } catch (const CLI::ParseError& e) {
    return app.exit(e);
  }

  /* ----- Event Reconstruction ----- */
  constexpr std::string_view simDir{"Trimuon@FCChh"};
  HNL::MG5::MadgraphData madgraphData{simDir, simulation, runId, tagId};

  const std::filesystem::path outPath{madgraphData.OutputTreePath(!singleRun, true)};
  if (std::filesystem::exists(outPath)) {
    LogWarn("{} already exists.", outPath.c_str());
    return 0;
  }

  DelphesReader delphesReader{madgraphData.eventsPath};
  const int64_t nEventsSimulated{delphesReader.GetEntries()};

  TreeWriter treeWriter{outPath, HNL::Reconstruct::eventTree};

  while (delphesReader.Next()) {
    ProcessEvent(delphesReader, treeWriter, minMuonPt, minJetPt, minDeltaR);
  }

  /* ----- CLI Output ----- */
  const int64_t nEventsReconstructed{treeWriter.GetEntries()};

  const double survivalRate{HNL::Reconstruct::SurvivalRate(nEventsReconstructed, nEventsSimulated)};

  if (crossSectionFactor != 1) madgraphData.crossSection *= crossSectionFactor;
  treeWriter.WriteMadgraphData(madgraphData, survivalRate);

  std::print("{},", madgraphData.CSV());
  std::print("{},{},", nEventsSimulated, nEventsReconstructed);
  std::print("{}\n", survivalRate * madgraphData.crossSection);

  return 0;
}
