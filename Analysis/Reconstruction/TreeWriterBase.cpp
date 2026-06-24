// Copyright 2023-2026 Bruno M. S. Oliveira

// HNL
#include "Reconstruction/TreeWriterBase.hpp"

#include "MG5_aMC/MadgraphData.hpp"

namespace HNL::Reconstruct {
TreeWriterBase::TreeWriterBase(const std::filesystem::path &filePath, const char *const treeName,
                               const char *const treeTitle)
    : file{filePath.c_str(), "CREATE"},
      tree{treeName, treeTitle == nullptr ? treeName : treeTitle} {}

TreeWriterBase::~TreeWriterBase() = default;

void TreeWriterBase::Close() {
  if (closed) return;

  file.cd();
  tree.Write();
  file.Close();
  closed = true;
}

int64_t TreeWriterBase::GetEntries() const { return tree.GetEntries(); }

void TreeWriterBase::WriteMadgraphData(const MG5::MadgraphData &madgraphData,
                                       const double survivalRate) {
  madgraphData.WriteToFile(file, survivalRate);
}
}  // namespace HNL::Reconstruct
