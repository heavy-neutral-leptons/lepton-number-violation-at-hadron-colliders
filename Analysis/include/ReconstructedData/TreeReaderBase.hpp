// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

// Standard Library
#include <cstdint>
#include <filesystem>

// ROOT
#include "TFile.h"
#include "TTree.h"
#include "TTreeReader.h"

// HNL
#include "ReconstructedData/ReconstructedEventDataKeys.hpp"

namespace HNL::Reconstruct {
/**
 * @brief Common ROOT file and tree reader used by analysis-specific tree readers.
 *
 * Derived classes define analysis branches, while this base class manages the output file lifetime.
 */
class TreeReaderBase {
 public:
  /**
   * @brief Open the `Data` tree from the provided ROOT file.
   * @param filePath Path to a reconstructed ROOT file containing the shared event tree.
   */
  explicit TreeReaderBase(const std::filesystem::path &filePath) : file{filePath.c_str()} {}

  TreeReaderBase(const TreeReaderBase &) = delete;
  TreeReaderBase(TreeReaderBase &&) = delete;
  TreeReaderBase &operator=(const TreeReaderBase &) = delete;
  TreeReaderBase &operator=(TreeReaderBase &&) = delete;

  /** @brief Destroy the reader and close the underlying ROOT file. */
  virtual ~TreeReaderBase() { Close(); }

  /** @brief Return the total number of entries in the wrapped tree. */
  [[nodiscard]] virtual int64_t GetEntries() const final { return tr.GetEntries(); }
  /** @brief Return the current entry index. */
  [[nodiscard]] virtual int64_t GetCurrentEntry() const final { return tr.GetCurrentEntry(); }
  /** @brief Advance to the next tree entry. */
  virtual bool Next() final { return tr.Next(); }

  /** @brief Restart iteration from the first tree entry. */
  virtual void Restart() final { tr.Restart(); }

 protected:
  /** @brief Close the ROOT file. */
  void Close();

 private:
  TFile file;          ///< Input ROOT file owned by the reader.
  bool closed{false};  ///< Tracks whether the input file has already been closed.

 protected:
  TTreeReader tr{HNL::Reconstruct::eventTree, &file};
};
}  // namespace HNL::Reconstruct
