// Copyright 2023-2026 Bruno M. S. Oliveira

// Standard Library
#include <algorithm>
#include <format>
#include <ranges>

// HNLs
#include "MCPID/ParticleName.hpp"
#include "MG5_aMC/Particle.hpp"

namespace HNL::MG5 {
[[nodiscard]] std::string Decay::DaughterNames() const {
  std::string daughterNames{};

  for (const auto [idx, pid] : daughters | std::ranges::views::enumerate) {
    if (idx != 0) daughterNames += " ";
    daughterNames += PID::ParticleName(pid);
  }

  return daughterNames;
}

void Particle::AddDecay(std::vector<int>& daughters, const double br) {
  std::ranges::sort(daughters, std::less{}, [](const int n) { return std::abs(n); });
  decays.emplace_back(daughters, br);

  for (int& daughterPID : daughters) {
    daughterPID = std::abs(daughterPID);
  }

  bool inCleanDecays{false};
  for (Decay& decay : cleanDecays) {
    if (decay.daughters == daughters) {
      inCleanDecays = true;
      decay.br += br;
    }
  }

  if (!inCleanDecays) {
    cleanDecays.emplace_back(daughters, br);
  }
}

std::string Particle::ToString(const bool clean) const {
  std::string out{std::format("Particle {} (M = {} GeV & Γ = {} GeV)", pid, mass, width)};

  const std::vector<Decay>* decaysToPrint{nullptr};
  if (clean) {
    decaysToPrint = &cleanDecays;
  } else {
    decaysToPrint = &decays;
  }

  for (const Decay& decay : *decaysToPrint) {
    out = std::format("{}\n BR = {:.3f} [{}]", out, decay.br, decay.DaughterNames());
  }

  return out;
}
}  // namespace HNL::MG5
