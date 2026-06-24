// Copyright 2023-2026 Bruno M. S. Oliveira

// Standard Library
#include <filesystem>
#include <ranges>
#include <string>

// CLI11
#include "CLI/CLI.hpp"

// Delphes
#include "DelphesWrapper/TreeNavigation.hpp"
#include "classes/DelphesClasses.h"

// HNL
#define LOG_CUT true
#define VERBOSE 0

#include "Common/Log.hpp"
#include "DelphesWrapper/ReaderParticle.hpp"
#include "MG5_aMC/MadgraphData.hpp"
#include "DimuonSS-FCChh-Generator/GeneratorIdentity.hpp"
#include "DimuonSS/TreeWriter.hpp"
#include "ReconstructedData/ReconstructedEventDataKeys.hpp"
#include "Reconstruction/SurvivalRate.hpp"

using DelphesReader = HNL::Delphes::FileReaderParticle;
using TreeWriter = HNL::DimuonSS::TreeWriter;

using HNL::DimuonSS::FCChh::GeneratorID;
using HNL::DimuonSS::FCChh::IdentifyParticle;

namespace {
bool ProcessEvent(const bool afterPythia, const DelphesReader& dr, TreeWriter& tw) {
  size_t nInitQuark{0};
  size_t nHNL{0};

  const GenParticle* leptonProd{nullptr};
  size_t nLeptonProd{0};

  const GenParticle* leptonDecay{nullptr};
  size_t nLeptonDecay{0};

  const GenParticle* quarkP{nullptr};
  size_t nQuarkP{0};

  const GenParticle* quarkA{nullptr};
  size_t nQuarkM{0};

  HNL::Delphes::ParticleTreeWalker ptw{dr.Particle};
  for (const auto& [idx, particle] : dr.Particle | std::views::enumerate) {
    const GeneratorID id{IdentifyParticle(ptw, idx, afterPythia)};
    Log<1>("\t{}", ToString(id));
    Log<1>("");

    switch (id) {
      case GeneratorID::initQuark:
        ++nInitQuark;
        break;

      case GeneratorID::hnl:
        ++nHNL;
        break;

      case GeneratorID::leptonProd:
        ++nLeptonProd;
        leptonProd = &particle;
        break;

      case GeneratorID::leptonDecay:
        ++nLeptonDecay;
        leptonDecay = &particle;
        break;

      case GeneratorID::quarkP:
        ++nQuarkP;
        quarkP = &particle;
        break;

      case GeneratorID::quarkA:
        ++nQuarkM;
        quarkA = &particle;
        break;

      case GeneratorID::leptonOther:
      case GeneratorID::quarkOther:
      case GeneratorID::lightNeutrino:
      case GeneratorID::other:
      case GeneratorID::error:
        break;
    }
  }

  if (nInitQuark != 2) {
    LogCut("Initial quarks", "{}", nInitQuark);
    return false;
  }

  if (nHNL != 1) {
    LogCut("Heavy neutrinos", "{}", nHNL);
    return false;
  }

  if (nLeptonProd != 1) {
    LogCut("Production leptons", "{}", nLeptonProd);
    return false;
  }

  if (nLeptonDecay != 1) {
    LogCut("Decay leptons", "{}", nLeptonDecay);
    return false;
  }

  if (nQuarkP != 1) {
    LogCut("Decay quarks", "{}", nQuarkP);
    return false;
  }

  if (nQuarkM != 1) {
    LogCut("Decay anti-quarks", "{}", nQuarkM);
    return false;
  }

  const GenParticle* quark1{nullptr};
  const GenParticle* quark2{nullptr};
  if (quarkP->PT > quarkA->PT) {
    quark1 = quarkP;
    quark2 = quarkA;
  } else {
    quark1 = quarkA;
    quark2 = quarkP;
  }

  tw.AddEvent(leptonProd, leptonDecay, quark1, quark2);
  return true;
}

struct OutputInfo {
  OutputInfo& operator+=(const TreeWriter& treeWriter) {
    ptL1 += treeWriter.l1_pt;
    etaL1 += std::abs(treeWriter.l1_et);
    ptL2 += treeWriter.l2_pt;
    etaL2 += std::abs(treeWriter.l2_et);

    ptQ1 += treeWriter.j1_pt;
    etaQ1 += std::abs(treeWriter.j1_et);
    ptQ2 += treeWriter.j2_pt;
    etaQ2 += std::abs(treeWriter.j2_et);

    ptW += treeWriter.j12_pt;
    etaW += std::abs(treeWriter.j12_et);
    massW += treeWriter.j12_ma;
    drQuarks += treeWriter.j1_j2_dr;

    drL1L2 += treeWriter.l1_l2_dr;

    drL1Q1 += treeWriter.l1_j1_dr;
    drL1Q2 += treeWriter.l1_j2_dr;
    drL1W += treeWriter.l1_j12_dr;
    drL2Q1 += treeWriter.l2_j1_dr;
    drL2Q2 += treeWriter.l2_j2_dr;
    drL2W += treeWriter.l2_j12_dr;

    return *this;
  }

  OutputInfo& operator/=(const size_t nEvents) {
    const double nEventsDbl{static_cast<double>(nEvents)};

    ptL1 /= nEventsDbl;
    etaL1 /= nEventsDbl;
    ptL2 /= nEventsDbl;
    etaL2 /= nEventsDbl;

    ptQ1 /= nEventsDbl;
    etaQ1 /= nEventsDbl;
    ptQ2 /= nEventsDbl;
    etaQ2 /= nEventsDbl;

    ptW /= nEventsDbl;
    etaW /= nEventsDbl;
    massW /= nEventsDbl;
    drQuarks /= nEventsDbl;

    drL1L2 /= nEventsDbl;

    drL1Q1 /= nEventsDbl;
    drL1Q2 /= nEventsDbl;
    drL1W /= nEventsDbl;
    drL2Q1 /= nEventsDbl;
    drL2Q2 /= nEventsDbl;
    drL2W /= nEventsDbl;

    return *this;
  }

  void Print() const {
    std::print("{},{},{},{},", ptL1, etaL1, ptL2, etaL2);
    std::print("{},{},{},{},", ptQ1, etaQ1, ptQ2, etaQ2);
    std::print("{},{},{},{},", ptW, etaW, massW, drQuarks);
    std::print("{},", drL1L2);
    std::print("{},{},{},{},{},{}", drL1Q1, drL2Q1, drL1W, drL2Q1, drL2Q2, drL2W);
  }

  double ptL1{0};
  double etaL1{0};
  double ptL2{0};
  double etaL2{0};

  double ptQ1{0};
  double etaQ1{0};
  double ptQ2{0};
  double etaQ2{0};

  double ptW{0};
  double etaW{0};
  double massW{0};
  double drQuarks{0};

  double drL1L2{0};

  double drL1Q1{0};
  double drL1Q2{0};
  double drL1W{0};
  double drL2Q1{0};
  double drL2Q2{0};
  double drL2W{0};
};
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

  const bool afterPythia{tagId != 0};

  const std::filesystem::path outPath{madgraphData.OutputTreePath(!singleRun, true)};
  if (std::filesystem::exists(outPath)) {
    LogWarn("{} already exists.", outPath.c_str());
    return 0;
  }

  DelphesReader delphesReader{madgraphData.eventsPath};
  const int64_t nEventsSimulated{delphesReader.GetEntries()};

  TreeWriter treeWriter{outPath, HNL::Reconstruct::eventTree};

  OutputInfo info{};
  while (delphesReader.Next()) {
    if (ProcessEvent(afterPythia, delphesReader, treeWriter)) {
      info += treeWriter;
    } else {
      exit(1);
    }
  }

  /* ----- CLI Output ----- */
  const int64_t nEventsReconstructed{treeWriter.GetEntries()};
  const double survivalRate{HNL::Reconstruct::SurvivalRate(nEventsReconstructed, nEventsSimulated)};

  if (crossSectionFactor != 1) madgraphData.crossSection *= crossSectionFactor;
  treeWriter.WriteMadgraphData(madgraphData, survivalRate);

  info /= nEventsReconstructed;

  std::print("{},", madgraphData.CSV());
  std::print("{},{},", nEventsSimulated, nEventsReconstructed);
  std::print("{},", survivalRate * madgraphData.crossSection);
  info.Print();
  std::println();

  return 0;
}
