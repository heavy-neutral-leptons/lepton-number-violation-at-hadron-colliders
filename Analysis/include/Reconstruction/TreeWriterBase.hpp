// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

// Standard Library
#include <cstdint>
#include <filesystem>

// ROOT
#include "TFile.h"
#include "TTree.h"

// HNL
#include "MG5_aMC/MadgraphData.hpp"

namespace HNL::Reconstruct {
/**
 * @brief Common ROOT file and tree writer used by analysis-specific tree writers.
 *
 * Derived classes define analysis branches and fill event-level observables, while this base class
 * manages the output file lifetime and the shared MadGraph metadata payload.
 */
class TreeWriterBase {
 public:
  /**
   * @brief Create a ROOT output file and tree.
   * @param filePath Path to the ROOT file to create.
   * @param treeName Name of the TTree stored in the file.
   * @param treeTitle Optional TTree title.
   */
  explicit TreeWriterBase(const std::filesystem::path &filePath, const char *treeName,
                          const char *treeTitle = nullptr);

  /** @brief Destroy the writer; derived classes are responsible for closing the file. */
  ~TreeWriterBase();

  TreeWriterBase(const TreeWriterBase &) = delete;
  TreeWriterBase(TreeWriterBase &&) = delete;
  TreeWriterBase &operator=(const TreeWriterBase &) = delete;
  TreeWriterBase &operator=(TreeWriterBase &&) = delete;

  /** @brief Return the number of entries currently stored in the tree. */
  [[nodiscard]] int64_t GetEntries() const;

  /**
   * @brief Persist MadGraph-level metadata alongside the reconstructed tree.
   * @param madgraphData Generated sample metadata.
   * @param survivalRate Fraction of events that survive reconstruction or selection.
   */
  void WriteMadgraphData(const MG5::MadgraphData &madgraphData, double survivalRate);

 protected:
  /** @brief Explicitly write and close the ROOT file. */
  void Close();

 private:
  TFile file;          ///< Output ROOT file owned by the writer.
  bool closed{false};  ///< Tracks whether the output file has already been closed.

 protected:
  TTree tree;  ///< Event tree filled by the derived analysis writer.
};
}  // namespace HNL::Reconstruct
