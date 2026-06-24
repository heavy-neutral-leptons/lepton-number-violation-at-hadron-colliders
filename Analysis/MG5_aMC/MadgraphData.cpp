// Copyright 2023-2026 Bruno M. S. Oliveira

// Standard Library
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <string>

// ROOT
#include "TParameter.h"

// HNL
#include "Common/Log.hpp"
#include "MG5_aMC/MadgraphData.hpp"
#include "Models/Generic.hpp"
#include "Models/HeavyN_Dirac.hpp"
#include "Models/HeavyN_Majorana.hpp"
#include "Models/SM.hpp"
#include "Models/SPSS.hpp"
#include "Models/SeesawICurrents.hpp"
#include "Models/pSPSS.hpp"
#include "ReconstructedData/ReconstructedEventDataKeys.hpp"
#include "SimulationReaders/read_slha.h"

namespace HNL::MG5 {
MadgraphData::MadgraphData(const std::string_view process, const std::string_view simulation,
                           const int runId, const int tagId, const double deltam,
                           const double damping)
    : process{process}, simulation{simulation}, runId{runId}, tagId{tagId} {
  using std::filesystem::path;

  const std::string runStr{std::format("run_{:0>2}", runId)};
  const std::string tagStr{std::format("tag_{}", tagId)};

  const path simulationDir{std::format("{}/{}.run", process, simulation)};

  runDir = simulationDir / "Events" / runStr;
  if (!std::filesystem::exists(runDir)) {
    LogError("Cannot access run directory at '{}'", runDir.c_str());
    exit(1);
  }

  bannerPath = runDir / std::format("{}_{}_banner.txt", runStr, tagStr);
  if (!std::filesystem::exists(bannerPath)) {
    LogError("Cannot access banner at '{}'", bannerPath.c_str());
    exit(1);
  }

  eventsPath = runDir / (tagStr + "_delphes_events.root");
  if (!std::filesystem::exists(eventsPath)) {
    LogError("Cannot access events at '{}'", eventsPath.c_str());
    exit(1);
  }

  const Model::ModelID modelID{FetchModel()};
  FetchModelParameters(modelID, deltam, damping);

  FetchCrossSection();
  // FetchPythiaCrossSection();
}

std::string MadgraphData::CSV() const {
  if (!model || model->ID() == Model::ModelID::SM) {
    return std::format("{:0>2},{}", runId, crossSection);
  }

  return std::format("{:0>2},{},{}", runId, model->CSV(), crossSection);
}

void MadgraphData::WriteToFile(TFile& file, double survivalRate) const {
  if (model) model->WriteToFile(file);

  TParameter<double> totalCrossSection{Reconstruct::totalCrossSection, crossSection};
  file.WriteObject(&totalCrossSection, Reconstruct::totalCrossSection);

  TParameter<double> effectiveCrossSection{Reconstruct::effectiveCrossSection,
                                           survivalRate * crossSection};
  file.WriteObject(&effectiveCrossSection, Reconstruct::effectiveCrossSection);
}

std::filesystem::path MadgraphData::OutputTreePath(const bool inSubDirectory,
                                                   const bool generator) const {
  std::filesystem::path outPath{
      std::format("/tmp/Heavy-Neutrino-Antineutrino-Oscillations/{}/{}", process, simulation)};

  if (inSubDirectory) {
    outPath /= std::format("run_{:0>2}", runId);
  }

  if (generator) {
    outPath += "-generator.root";
  } else {
    outPath += ".root";
  }

  try {
    std::filesystem::create_directories(outPath.parent_path());
  } catch (const std::exception& ex) {
    LogError("Failed to create directory \"{}\"\n", ex.what());
    exit(1);
  }

  return outPath;
}

Model::ModelID MadgraphData::FetchModel() const {
  std::ifstream stream{bannerPath};
  std::string line;

  while (std::getline(stream, line)) {
    if (line.starts_with("# Begin MODEL")) {
      std::getline(stream, line);
      break;
    }
  }

  if (line.starts_with("SM_HeavyN_NLO")) {
    return Model::ModelID::HeavyN_Majorana;
  }

  if (line.starts_with("SM_HeavyN_Dirac_NLO")) {
    return Model::ModelID::HeavyN_Dirac;
  }

  if (line.starts_with("pSPSS")) {
    return Model::ModelID::pSPSS;
  }

  if (line.starts_with("SeesawICurrents")) {
    return Model::ModelID::SeesawICurrents;
  }

  if (line.starts_with("SPSS")) {
    return Model::ModelID::SPSS;
  }

  if (line.starts_with("sm")) {
    return Model::ModelID::SM;
  }

  LogError("Model could not be identified.");
  exit(1);
}

void MadgraphData::FetchModelParameters(const Model::ModelID modelID, const double deltam,
                                        const double damping) {
  switch (modelID) {
    case Model::ModelID::SM: {
      model = std::make_unique<Model::SM>();
      return;
    }

    case Model::ModelID::HeavyN_Majorana: {
      const MG5_aMC::SLHAReader slhaReader{bannerPath.c_str()};
      model = std::make_unique<Model::HeavyN::Majorana>(slhaReader);
      return;
    }

    case Model::ModelID::HeavyN_Dirac: {
      const MG5_aMC::SLHAReader slhaReader{bannerPath.c_str()};
      model = std::make_unique<Model::HeavyN::Dirac>(slhaReader);
      return;
    }

    case Model::ModelID::pSPSS: {
      const MG5_aMC::SLHAReader slhaReader{bannerPath.c_str()};
      model = std::make_unique<Model::pSPSS>(slhaReader, deltam, damping);
      return;
    }

    case Model::ModelID::SeesawICurrents: {
      const MG5_aMC::SLHAReader slhaReader{bannerPath.c_str()};
      model = std::make_unique<Model::SeesawICurrents>(slhaReader);
      return;
    }

    case Model::ModelID::SPSS: {
      const MG5_aMC::SLHAReader slhaReader{bannerPath.c_str()};
      model = std::make_unique<Model::SPSS>(slhaReader);
      return;
    }
  }
}

void MadgraphData::FetchCrossSection() {
  std::ifstream stream{bannerPath};

  char ch{};
  std::string line;

  stream.seekg(-1, std::ios_base::end);
  for (int64_t pos{stream.tellg()}; pos >= 0; --pos) {
    stream.seekg(pos);
    stream.get(ch);

    if (ch == '\n') {
      line.clear();
      continue;
    }

    line.push_back(ch);
    if (ch == '#') {
      std::ranges::reverse(line);
      if (line.starts_with("#  Integrated weight (pb)")) {
        break;
      }
    }
  }

  const size_t csStart{line.find_first_of("0123456789")};
  const size_t csEnd{line.size()};
  const auto result{std::from_chars(line.data() + csStart, line.data() + csEnd, crossSection)};

  if (!line.starts_with("#  Integrated weight (pb)") || result.ec != std::errc() ||
      crossSection <= 0) {
    LogError("Failed to read cross section:\n - {}\n - {}\n - {} ({})\n",
             std::make_error_condition(result.ec).message(), bannerPath.c_str(), line,
             line.substr(csStart, csEnd));
    exit(1);
  } else {
    Log("Read cross section {}:\n - {}\n - {} ({})\n", crossSection, bannerPath.c_str(), line,
        line.substr(csStart, csEnd));
  }
}

void MadgraphData::FetchPythiaCrossSection() {
  const auto xsecsPath{runDir / std::format("tag_{}_merged_xsecs.txt", tagId)};
  if (!std::filesystem::exists(xsecsPath)) {
    LogError("{} does not exist", xsecsPath.string());
    return;
  }

  std::ifstream stream{xsecsPath};
  std::string line;

  std::getline(stream, line);
  std::getline(stream, line);

  size_t csStart{line.find_first_of(' ')};
  csStart = line.find_first_not_of(' ', csStart);
  const size_t csEnd{line.size()};

  double crossSectionPY8{-1};
  const auto result{std::from_chars(line.data() + csStart, line.data() + csEnd, crossSectionPY8)};

  if (result.ec != std::errc() || crossSectionPY8 <= 0) {
    LogError("Failed to read cross section:\n - {}\n - {}\n - {} ({})\n",
             std::make_error_condition(result.ec).message(), bannerPath.c_str(), line,
             line.substr(csStart, csEnd));
    exit(1);
  } else {
    Log("Read cross section {}:\n - {}\n - {} ({})\n", crossSectionPY8, bannerPath.c_str(), line,
        line.substr(csStart, csEnd));
  }
}

Particle MadgraphData::FetchDecayModes(const int pid) const {
  const MG5_aMC::SLHAReader slhaReader{bannerPath.c_str()};
  const double mass{slhaReader.get_block_entry("mass", pid)};
  const double width{slhaReader.get_block_entry("decay", pid, -1)};

  if (width == -1) {
    LogError("Attempted to fetch decay modes of nonexistent particle {}", pid);
    exit(1);
  }

  Particle particle{pid, mass, width};

  std::ifstream stream{bannerPath};
  std::string line;

  // Find particle decay entry
  while (std::getline(stream, line)) {
    if (!line.starts_with("DECAY")) continue;
    std::istringstream iss(line);

    std::string tag;
    int particlePID{};
    double particleWidth{};
    iss >> tag >> particlePID >> particleWidth;

    if (particlePID != pid) continue;
    if (particleWidth != particle.Width()) {
      LogError("Inconsistent particle width {}", pid);
      exit(1);
    }

    break;
  }

  // Get decay channels
  while (std::getline(stream, line)) {
    if (line.starts_with("DECAY") || line == "</slha>") break;
    if (line.empty() || line[0] == '#') continue;

    std::istringstream iss(line);

    double br{};
    size_t nDaughters{};
    iss >> br >> nDaughters;

    std::vector<int> daughters;
    for (size_t idx{0}; idx < nDaughters; ++idx) {
      daughters.emplace_back(0);
      iss >> daughters.back();
    }

    particle.AddDecay(daughters, br);
  }

  stream.close();

  return particle;
}

[[nodiscard]] std::vector<Particle> MadgraphData::FetchDecayModes() const {
  const MG5_aMC::SLHAReader slhaReader{bannerPath.c_str()};

  std::vector<Particle> particles{};

  std::string line;
  for (std::ifstream stream{bannerPath}; std::getline(stream, line);) {
    if (line == "</slha>") break;

    if (line.starts_with("DECAY")) {
      std::istringstream iss(line);

      std::string tag;
      int pid{};
      double width{};
      iss >> tag >> pid >> width;

      const double mass{slhaReader.get_block_entry("mass", pid)};

      particles.emplace_back(pid, mass, width);
    } else if (!particles.empty()) {
      if (line.empty() || line[0] == '#') continue;
      std::istringstream iss(line);

      double br{};
      iss >> br;

      size_t nDaughters{};
      iss >> nDaughters;

      int daughter{};
      std::vector<int> daughters;
      for (size_t idx{0}; idx < nDaughters; ++idx) {
        iss >> daughter;
        daughters.emplace_back(daughter);
      }

      particles.back().AddDecay(daughters, br);
    }
  }

  return particles;
}
}  // namespace HNL::MG5
