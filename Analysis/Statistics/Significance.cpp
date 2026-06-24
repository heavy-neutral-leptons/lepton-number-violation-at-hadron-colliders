// Copyright 2023-2026 Bruno M. S. Oliveira

// Standard Library
#include <cmath>
#include <limits>

// HNL
#include "Common/Log.hpp"
#include "Statistics/Significance.hpp"

namespace HNL::Statistics {
double PoissonSignificance(const double nMeasured, const double nExpected) {
  if (nMeasured < 0 || nExpected < 0) {
    LogError(
        "Attempting to calculate significance with negative number of events"
        "\n\t#Measured = {} events"
        "\n\t#Expected = {} events",
        nMeasured, nExpected);
    exit(1);
  }

  if (nMeasured == nExpected) return 0;
  if (nMeasured == 0) return std::sqrt(2 * nExpected);
  if (nExpected == 0) return std::numeric_limits<double>::infinity();

  const double x{nExpected - nMeasured + (nMeasured * std::log(nMeasured / nExpected))};
  return x > 0 ? std::sqrt(2 * x) : std::numeric_limits<double>::min();
}

double ExclusionSignificance(const double nSignal, const double nBkgrnd) {
  if (nBkgrnd == 0) return std::numbers::sqrt2 * std::sqrt(nSignal);
  return PoissonSignificance(nBkgrnd, nBkgrnd + nSignal);
}

double DiscoverySignificance(const double nSignal, const double nBkgrnd) {
  return PoissonSignificance(nBkgrnd + nSignal, nBkgrnd);
}
}  // namespace HNL::Statistics
