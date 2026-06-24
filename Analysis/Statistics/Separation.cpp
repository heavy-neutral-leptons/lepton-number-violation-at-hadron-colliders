// Copyright 2023-2026 Bruno M. S. Oliveira

// HNL
#include "Statistics/Separation.hpp"

#include "Common/Log.hpp"

namespace HNL::Statistics {
DistComparison Separation(const TH1* const s, const TH1* const b) {
  if (s->GetNbinsX() != b->GetNbinsX() || s->GetNbinsX() <= 0 ||
      s->GetXaxis()->GetXmin() != b->GetXaxis()->GetXmin() ||
      s->GetXaxis()->GetXmax() != b->GetXaxis()->GetXmax() ||
      s->GetXaxis()->GetXmax() <= s->GetXaxis()->GetXmin()) {
    LogError("Invalid histogram dimensions:\n S = {} ({}:{})\n B = {} ({}:{})", s->GetNbinsX(),
             s->GetXaxis()->GetXmin(), s->GetXaxis()->GetXmax(), b->GetNbinsX(),
             b->GetXaxis()->GetXmin(), b->GetXaxis()->GetXmax());

    exit(1);
  }

  const double sWeight{s->GetSumOfWeights()};
  const double bWeight{b->GetSumOfWeights()};

  if (sWeight <= 0 || bWeight <= 0) {
    LogError("Empty histograms: {} {}", sWeight, bWeight);
    exit(1);
  }

  DistComparison comparison{};
  for (int bin{1}; bin <= s->GetNbinsX(); ++bin) {
    if (s->GetBinContent(bin) == 0 && b->GetBinContent(bin) == 0) continue;

    const double sum{(s->GetBinContent(bin) / sWeight) + (b->GetBinContent(bin) / bWeight)};
    const double dif{(s->GetBinContent(bin) / sWeight) - (b->GetBinContent(bin) / bWeight)};

    comparison.separation1 += std::abs(dif) / 2;
    comparison.separation2 += dif * dif / (2 * sum);
  }

  return comparison;
}

DistComparison Separation(const std::vector<double>& s, const std::vector<double>& b) {
  if (s.size() != b.size() || s.empty()) {
    LogError("Invalid vector dimensions:\n S = {}\n B = {}", s.size(), b.size());

    exit(1);
  }

  double sWeight{0};
  double bWeight{0};
  for (size_t idx{0}; idx < s.size(); ++idx) {
    sWeight += s[idx];
    bWeight += b[idx];
  }

  if (sWeight <= 0 || bWeight <= 0) {
    LogError("Empty histograms: {} {}", sWeight, bWeight);
    exit(1);
  }

  DistComparison comparison{};
  for (size_t idx{0}; idx < s.size(); ++idx) {
    if (s[idx] == 0 && b[idx] == 0) continue;

    const double sum{(s[idx] / sWeight) + (b[idx] / bWeight)};
    const double dif{(s[idx] / sWeight) - (b[idx] / bWeight)};

    comparison.separation1 += std::abs(dif) / 2;
    comparison.separation2 += dif * dif / (2 * sum);
  }

  return comparison;
}
}  // namespace HNL::Statistics
