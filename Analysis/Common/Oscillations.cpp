// Copyright 2023-2026 Bruno M. S. Oliveira

// HNL
#include "Common/Oscillations.hpp"

#include "Common/Log.hpp"

namespace HNL::Common {
bool IncludeEvent(const LNState lns, const double properLifetime, const double massSplitting,
                  const double damping, const double rngLikelihood) {
  if (lns == Common::LNState::error) {
    LogWarn("Event is neither LNC or LNV");
    return false;
  }

  if (LNCProbability(massSplitting, damping, properLifetime) >= rngLikelihood) {
    if (lns != Common::LNState::lnc) return false;
  } else {
    if (lns != Common::LNState::lnv) return false;
  }

  return true;
}
}  // namespace HNL::Common
