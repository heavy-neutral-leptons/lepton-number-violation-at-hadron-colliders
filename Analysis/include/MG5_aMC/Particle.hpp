// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

// Standard Library
#include <string>
#include <vector>

namespace HNL::MG5 {
/**
 * @brief One particle decay channel read from MadGraph metadata.
 *
 * Daughter ids are stored in the order used by the parsed decay table; the cleaned decay list in
 * `Particle` may merge channels after taking absolute values of daughter ids.
 */
struct Decay {
  std::vector<int> daughters;  ///< Daughter PDG ids in the decay channel.
  double br;                   ///< Branching ratio of the decay channel.

  /**
   * @brief Return the daughter particles formatted by name.
   * @return Space-separated particle names derived from the stored PDG ids.
   */
  [[nodiscard]] std::string DaughterNames() const;
};

/**
 * @brief Particle properties and decay table read from MadGraph metadata.
 *
 * The object stores both the raw decay channels and a cleaned presentation view in which daughter
 * ids are made unsigned and equal final states are merged by summing their branching ratios.
 */
class Particle {
 public:
  /**
   * @brief Construct a particle from its PDG id, mass, and width.
   * @param pid Particle PDG identifier.
   * @param mass Particle pole mass in GeV.
   * @param width Total decay width in GeV.
   */
  Particle(const int pid, const double mass, const double width)
      : pid{pid}, mass{mass}, width{width} {}

  /**
   * @brief Add a decay channel and update the cleaned decay list.
   * @param daughters Daughter PDG ids parsed from the SLHA decay table.
   * @param br Branching ratio assigned to the channel.
   *
   * The input vector is normalized in place while building the cleaned decay
   * representation.
   */
  void AddDecay(std::vector<int>& daughters, double br);

  /**
   * @brief Format the particle and its decays as text.
   * @param clean When `true`, print the merged decay table; otherwise print the raw decay channels.
   * @return Human-readable particle summary.
   */
  [[nodiscard]] std::string ToString(bool clean) const;

  /** @brief Return the PDG identifier of the particle. */
  [[nodiscard]] int PID() const { return pid; }
  /** @brief Return the particle mass in GeV. */
  [[nodiscard]] double Mass() const { return mass; }
  /** @brief Return the total decay width in GeV. */
  [[nodiscard]] double Width() const { return width; }
  /** @brief Return all parsed decay channels without merging conjugate states. */
  [[nodiscard]] const std::vector<Decay>& Decays() const { return decays; }
  /** @brief Return the cleaned decay channels used for presentation. */
  [[nodiscard]] const std::vector<Decay>& CleanDecays() const { return cleanDecays; }

 private:
  int pid;
  double mass;
  double width;

  std::vector<Decay> decays;
  std::vector<Decay> cleanDecays;
};
}  // namespace HNL::MG5
