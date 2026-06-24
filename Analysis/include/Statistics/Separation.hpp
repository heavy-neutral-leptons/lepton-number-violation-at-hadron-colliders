// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

// ROOT
#include "TH1.h"

// HNL
namespace HNL::Statistics {
/**
 * @brief Separation metrics comparing two normalized distributions.
 *
 * `separation1` stores the total-variation-like metric used in some summaries,
 * while `separation2` stores the TMVA-style separation integral.
 */
struct DistComparison {
  double separation1{0};  ///< S1 separation (absolute differences).
  double separation2{0};  ///< S2 separation (TMVA-style quadratic separation).

  bool operator==(const DistComparison& rhs) const {
    return separation1 == rhs.separation1 && separation2 == rhs.separation2;
  }
};

/**
 * @brief Compute separation metrics from two histograms.
 *
 * Adapted from https://root.cern/doc/v634/classTMVA_1_1Tools.html#a1ac374fc4d7cd300117b8553274b30b9
 *
 * @param s Signal-like distribution histogram.
 * @param b Background-like distribution histogram.
 * @return Pair of separation metrics accumulated over the histogram bins.
 *
 * @warning The function terminates the program if the histogram ranges or normalizations are
 * invalid.
 */
DistComparison Separation(const TH1* s, const TH1* b);

/**
 * @brief Compute separation metrics from two binned distributions.
 *
 * @param s Signal-like bin contents.
 * @param b Background-like bin contents.
 * @return Pair of separation metrics accumulated over the bins.
 *
 * @warning The function terminates the program if the vectors are empty, mismatched, or have zero
 * total weight.
 */
DistComparison Separation(const std::vector<double>& s, const std::vector<double>& b);
}  // namespace HNL::Statistics
