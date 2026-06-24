// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

// Standard Library
#include <filesystem>
#include <string>
#include <vector>

// ROOT
#include "TFile.h"

// HNL
#include "MG5_aMC/Particle.hpp"
#include "Models/Generic.hpp"
#include "Particle.hpp"

namespace HNL::MG5 {
/**
 * @brief Metadata and helper paths for one MadGraph production run.
 *
 * The object resolves the run directory layout, locates the generator banner and Delphes output,
 * reconstructs the associated benchmark model, and exposes the cross section metadata later written
 * into reconstructed ROOT files.
 */
class MadgraphData {
 public:
  /**
   * @brief Build the run description from the process, simulation tag, and ids.
   * @param process Study or process directory containing the `.run` folder.
   * @param simulation Simulation key inside `process`.
   * @param runId Numeric MadGraph run identifier used in `run_##`.
   * @param tagId Numeric reconstruction tag used in banner and ROOT filenames.
   * @param deltam Optional pseudo-Dirac mass splitting forwarded to model reconstruction when
   * required.
   * @param damping Optional damping parameter forwarded to model reconstruction when required.
   */
  MadgraphData(std::string_view process, std::string_view simulation, int runId, int tagId,
               double deltam = 0, double damping = 0);

  std::filesystem::path runDir;      ///< Path to the resolved `run_##` directory.
  std::filesystem::path bannerPath;  ///< Path to the MadGraph banner file with SLHA metadata.
  std::filesystem::path eventsPath;  ///< Path to the Delphes event ROOT file for this run.

  /**
   * @brief Build the output ROOT path for this run and ensure its parent directory exists.
   * @param inSubDirectory Whether to place the file inside a `run_##` subdirectory.
   * @param generator Whether to use the generator-level `-generator` suffix
   * @return Output path under `/tmp/Heavy-Neutrino-Antineutrino-Oscillations/...`.
   */
  [[nodiscard]] std::filesystem::path OutputTreePath(bool inSubDirectory,
                                                     bool generator = false) const;

  std::unique_ptr<Model::Generic> model{nullptr};  ///< Physics model reconstructed from metadata.

  double crossSection{-1};  ///< Total generated cross section of the run in pb.

  /** @brief Serialize the run metadata as a CSV row. */
  [[nodiscard]] std::string CSV() const;
  /**
   * @brief Persist the run metadata plus total and effective cross sections into a ROOT file.
   * @param file ROOT file that receives the metadata objects.
   * @param survivalRate Event survival fraction used to derive the effective cross section.
   */
  void WriteToFile(TFile &file, double survivalRate) const;

  /**
   * @brief Fetch the decay table of one particle from the banner.
   * @param pid PDG id of the particle whose `DECAY` block should be parsed.
   * @return Particle summary including mass, width, and parsed decay channels.
   */
  [[nodiscard]] Particle FetchDecayModes(int pid) const;
  /**
   * @brief Fetch decay tables for all particles declared in the banner.
   * @return Parsed particle summaries in banner order.
   */
  [[nodiscard]] std::vector<Particle> FetchDecayModes() const;

 private:
  [[nodiscard]] Model::ModelID FetchModel() const;
  void FetchModelParameters(Model::ModelID modelID, double deltam, double damping);

  void FetchCrossSection();
  void FetchPythiaCrossSection();

  std::string process;
  std::string simulation;
  int runId, tagId;
};
}  // namespace HNL::MG5
