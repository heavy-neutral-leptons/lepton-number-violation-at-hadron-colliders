// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

// Standard Library
#include <filesystem>

// ROOT
#include "TFile.h"
#include "TTreeReader.h"

namespace HNL::Delphes {
/**
 * @brief Lightweight wrapper around an in-memory Delphes `TTreeReader`.
 *
 * The wrapper is non-owning: the caller retains ownership of the underlying `TTreeReader` and must
 * keep it alive for the lifetime of this object.
 */
class BaseReader {
 public:
  BaseReader() = default;

  BaseReader(const BaseReader &) = delete;
  BaseReader(BaseReader &&) = delete;
  BaseReader &operator=(const BaseReader &) = delete;
  BaseReader &operator=(BaseReader &&) = delete;

  virtual ~BaseReader() = default;

  /** @brief Return the total number of entries in the wrapped tree. */
  [[nodiscard]] virtual int64_t GetEntries() const = 0;
  /** @brief Return the current entry index. */
  [[nodiscard]] virtual int64_t GetCurrentEntry() const = 0;
  /** @brief Advance iteration to the next tree entry. */
  virtual bool Next() = 0;
  /** @brief Restart iteration from the first tree entry. */
  virtual void Restart() = 0;

  /**
   * @brief Materialize all bound branch arrays for the current entry.
   * @note Call after advancing the underlying reader when branch payloads are needed eagerly.
   */
  virtual void Refresh() const = 0;
};

/**
 * @brief Lightweight wrapper around an in-memory Delphes `TTreeReader`.
 *
 * The wrapper is non-owning: the caller retains ownership of the underlying `TTreeReader` and must
 * keep it alive for the lifetime of this object.
 */
class TreeReader : public BaseReader {
 public:
  /**
   * @brief Bind to an existing Delphes tree reader.
   * @param treeReader Reader already configured for a Delphes tree.
   */
  explicit TreeReader(TTreeReader &treeReader) : treeReader{treeReader} {}

  TreeReader(const TreeReader &) = delete;
  TreeReader(TreeReader &&) = delete;
  TreeReader &operator=(const TreeReader &) = delete;
  TreeReader &operator=(TreeReader &&) = delete;

  ~TreeReader() override = default;

  [[nodiscard]] int64_t GetEntries() const final { return treeReader.GetEntries(); }
  [[nodiscard]] int64_t GetCurrentEntry() const final { return treeReader.GetCurrentEntry(); }
  bool Next() final { return treeReader.Next(); }
  void Restart() final { treeReader.Restart(); }

 protected:
  TTreeReader &treeReader;
};

/**
 * @brief File-backed Delphes tree reader opening the `Delphes` tree from a ROOT file.
 *
 * The reader owns the opened `TFile` and closes it on destruction.
 */
class FileReader : public BaseReader {
 public:
  /**
   * @brief Open a Delphes ROOT file and bind a `TTreeReader` to its `Delphes` tree.
   * @param filePath Filesystem path to the Delphes ROOT file.
   */
  explicit FileReader(const std::filesystem::path &filePath) : delphesOutput{filePath.c_str()} {}

  FileReader(const FileReader &) = delete;
  FileReader(FileReader &&) = delete;
  FileReader &operator=(const FileReader &) = delete;
  FileReader &operator=(FileReader &&) = delete;

  ~FileReader() override { delphesOutput.Close(); }

  [[nodiscard]] int64_t GetEntries() const final { return treeReader.GetEntries(); }
  [[nodiscard]] int64_t GetCurrentEntry() const final { return treeReader.GetCurrentEntry(); }
  bool Next() final { return treeReader.Next(); }
  void Restart() final { treeReader.Restart(); }

 private:
  TFile delphesOutput;

 protected:
  TTreeReader treeReader{"Delphes", &delphesOutput};
};

/**
 * @brief Accepts the supported Delphes tree reader wrapper types.
 * @tparam T Candidate wrapper type.
 */
template <typename T>
concept Reader = std::derived_from<T, BaseReader>;
}  // namespace HNL::Delphes
