// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

// Standard Library
#include <filesystem>

// ROOT
#include "TChain.h"
#include "TFile.h"
#include "TTree.h"

// HNL
#include "Models/Generic.hpp"

namespace HNL::Reconstruct {
/**
 * Build a compact run name from a processed ROOT file path.
 *
 * The returned value is `<parent-directory>/<stem>` and is used for plot labels and human-readable
 * source names.
 *
 * @param path Path to a processed ROOT file.
 * @return Compact run identifier.
 */
std::string SimulationName(const std::filesystem::path& path);

/**
 * @brief Common interface for reconstructed-event ROOT data.
 *
 * This abstract base class exposes the cross-section metadata and `Data` tree
 * access shared by wrappers over either one reconstructed ROOT file or a chain
 * assembled from several such files.
 */
class ReconstructedEventData {
 public:
  ReconstructedEventData() = default;

  ReconstructedEventData(const ReconstructedEventData&) = delete;
  ReconstructedEventData(ReconstructedEventData&&) = delete;
  ReconstructedEventData& operator=(const ReconstructedEventData&) = delete;
  ReconstructedEventData& operator=(ReconstructedEventData&&) = delete;

  virtual ~ReconstructedEventData() = default;

  /** @brief Return a human-readable identifier for the wrapped data source. */
  [[nodiscard]] virtual std::string Name() const = 0;

  /** @brief Return the total generated cross section, in pb, before reconstruction. */
  [[nodiscard]] virtual double TotalCrossSection() const final { return totalCrossSection; }
  /** @brief Return the effective cross section, in pb, represented by the stored events. */
  [[nodiscard]] virtual double EffectiveCrossSection() const final { return effectiveCrossSection; }

  /** @brief Return the reconstruction efficiency. */
  [[nodiscard]] virtual double ReconstructionEfficiency() const final {
    return effectiveCrossSection / totalCrossSection;
  }

  /**
   * @brief Return the expected event yield for a given integrated luminosity.
   * @param luminosity Integrated luminosity in pb^-1.
   * @return The expected number of events represented by this data source.
   */
  [[nodiscard]] virtual double NumberOfEvents(const double luminosity) const final {
    return luminosity * effectiveCrossSection;
  }

  /**
   * @brief Return the reconstructed-event `Data` tree.
   * @return Pointer to the underlying ROOT tree.
   */
  [[nodiscard]] virtual TTree* Tree() = 0;
  /** @brief Return the exact number of reconstructed events stored in the tree. */
  [[nodiscard]] int64_t GetEntries() { return Tree()->GetEntries(); }

 protected:
  double totalCrossSection{0};      ///< Cross section prior to event reconstruction (pb)
  double effectiveCrossSection{0};  ///< Cross section following event reconstruction (pb)
};

/**
 * @brief Wrapper around one reconstructed-events ROOT file.
 *
 * The wrapper owns the opened `TFile`, provides access to its `Data` tree, and
 * exposes the model metadata serialized alongside the event content.
 */
class ReconstructedEventFile final : public ReconstructedEventData {
 public:
  /**
   * @brief Open one reconstructed-events ROOT file and cache its metadata.
   * @param path Filesystem path to the ROOT file to open.
   */
  explicit ReconstructedEventFile(const std::filesystem::path& path);

  ReconstructedEventFile(const ReconstructedEventFile&) = delete;
  ReconstructedEventFile(ReconstructedEventFile&&) = delete;
  ReconstructedEventFile& operator=(const ReconstructedEventFile&) = delete;
  ReconstructedEventFile& operator=(ReconstructedEventFile&&) = delete;

  ~ReconstructedEventFile() final;

  /** @brief Return the filesystem path of the wrapped ROOT file. */
  [[nodiscard]] const std::filesystem::path& Path() const { return path; }

  /**
   * @brief Return the wrapped file path as a string.
   * @return A human-readable identifier for this file wrapper.
   */
  [[nodiscard]] std::string Name() const final;

  /**
   * @brief Return the serialized model-parameter vector stored in the file.
   * @return Pointer to the parameter vector owned by the underlying ROOT file.
   */
  [[nodiscard]] std::vector<double>* Parameters() const;

  /** @brief Return the model identifier stored in the file metadata. */
  [[nodiscard]] Model::ModelID ModelID() const;
  /**
   * @brief Materialize the physics model encoded in the file metadata.
   * @return A newly constructed model instance matching the stored metadata.
   */
  [[nodiscard]] std::unique_ptr<Model::Generic> Model() const;

  /** @brief Return the reconstructed event tree stored in the file. */
  [[nodiscard]] TTree* Tree() final { return tree; }
  /** @brief Return the exact number of reconstructed events stored in the file. */
  [[nodiscard]] int64_t GetEntries() const { return tree->GetEntries(); }

 private:
  const std::filesystem::path path;

  TFile* const file;
  TTree* const tree;
};

/**
 * @brief Chain together several reconstructed-events ROOT files.
 *
 * The chain exposes the combined `Data` tree and sums the per-file total and
 * effective cross sections stored in the input files.
 */
class ReconstructedEventChain final : public ReconstructedEventData {
 public:
  /**
   * @brief Build a chain from several reconstructed-events ROOT files.
   * @param paths Filesystem paths to the ROOT files to be chained.
   */
  explicit ReconstructedEventChain(const std::vector<std::filesystem::path>& paths);

  ReconstructedEventChain(const ReconstructedEventChain&) = delete;
  ReconstructedEventChain(ReconstructedEventChain&&) = delete;
  ReconstructedEventChain& operator=(const ReconstructedEventChain&) = delete;
  ReconstructedEventChain& operator=(ReconstructedEventChain&&) = delete;

  ~ReconstructedEventChain() final = default;

  /**
   * @brief Return an identifier derived from the first chained file name.
   *
   * This is not a full path and is intended only as a compact label for plots,
   * logs, or summaries.
   */
  [[nodiscard]] std::string Name() const final;

  /** @brief Return the chained `Data` tree spanning all input files. */
  [[nodiscard]] TTree* Tree() final { return &chain; }
  /** @brief Return the exact number of reconstructed events across the full chain. */
  [[nodiscard]] int64_t GetEntries() const;

 private:
  const std::vector<std::filesystem::path> paths;

  TChain chain;
};
}  // namespace HNL::Reconstruct
