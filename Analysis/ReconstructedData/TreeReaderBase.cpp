// Copyright 2023-2026 Bruno M. S. Oliveira

// HNL
#include "ReconstructedData/TreeReaderBase.hpp"

namespace HNL::Reconstruct {
void TreeReaderBase::Close() {
  if (closed) return;

  file.cd();
  file.Close();
  closed = true;
}
}  // namespace HNL::Reconstruct
