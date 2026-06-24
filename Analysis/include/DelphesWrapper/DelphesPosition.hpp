// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

// HNL
#include "Constants/Experiments.hpp"
#include "DelphesWrapper/DelphesConcepts.hpp"
#include "DelphesWrapper/DelphesVector3D.hpp"
#include "DelphesWrapper/DelphesVector4D.hpp"

namespace HNL {
/** @brief Tracker acceptance category assigned to a Delphes or truth-level object. */
enum class TrackerPosition : uint8_t {
  prompt,
  displaced,
  outside_tracker,
  neither_prompt_nor_displaced,
};

// CMS (LHC)
namespace CMS {
/** @brief Classify a vertex and momentum according to the CMS tracker acceptance. */
inline TrackerPosition Tracker(const ROOT::Math::XYZPoint &vertex,
                               const ROOT::Math::PtEtaPhiMVector &momentum) {
  const double displacement_t{vertex.Rho()};
  if (displacement_t > maxDisplacementTransverse || std::abs(vertex.Z()) > maxDisplacementAxis) {
    return TrackerPosition::outside_tracker;
  }

  const double impact_parameter_t{Delphes::TransverseImpactParameter(vertex, momentum)};
  if (impact_parameter_t <= impactParameterPromptThreshold) {
    return TrackerPosition::prompt;
  }

  if (impact_parameter_t >= impactParameterDisplacedThreshold) {
    return TrackerPosition::displaced;
  }

  return TrackerPosition::neither_prompt_nor_displaced;
}

/** @brief CMS tracker classification for compatible Delphes objects. */
template <Delphes::DelphesClassWithParticleOrXYZWithPtEtaPhi T>
inline TrackerPosition Tracker(const T &t) {
  return Tracker(Delphes::Origin(t), Delphes::Momentum(t));
}

/** @brief CMS tracker classification for pointers to compatible Delphes objects. */
template <Delphes::DelphesClassWithParticleOrXYZWithPtEtaPhi T>
inline TrackerPosition Tracker(const T *const t) {
  return Tracker(Delphes::Origin(t), Delphes::Momentum(t));
}
}  // namespace CMS

// IDEA (FCC-ee)
namespace IDEA {
/** @brief IDEA tracker classification for objects with explicit decay coordinates. */
template <Delphes::DelphesClassWithXYZ T>
TrackerPosition Tracker(const T &t) {
  if (Delphes::Displacement(t) < minDisplacementThreshold) {
    return TrackerPosition::prompt;
  }

  if (std::pow(t.X, 2) + std::pow(t.Y, 2) > std::pow(maxDisplacementTransverse, 2) ||
      std::abs(t.Z) > maxDisplacementLongitudinal

  ) {
    return TrackerPosition::outside_tracker;
  }

  return TrackerPosition::displaced;
}

/** @brief IDEA tracker classification via the referenced generator-level particle. */
template <Delphes::DelphesClassWithParticleNoXYZ T>
TrackerPosition Tracker(const T &t) {
  return Tracker(*Delphes::Particle(t));
}
}  // namespace IDEA

// FCC-hh
namespace FCChh {
/** @brief Classify a vertex and momentum according to the FCC-hh tracker acceptance. */
inline TrackerPosition Tracker(const ROOT::Math::XYZPoint &vertex,
                               const ROOT::Math::PtEtaPhiMVector &momentum) {
  const double displacement_t{vertex.Rho()};
  if (displacement_t > maxDisplacementTransverse || std::abs(vertex.Z()) > maxDisplacementAxis) {
    return TrackerPosition::outside_tracker;
  }

  const double impact_parameter_t{Delphes::TransverseImpactParameter(vertex, momentum)};
  if (impact_parameter_t <= impactParameterPromptThreshold) {
    return TrackerPosition::prompt;
  }

  if (impact_parameter_t >= impactParameterDisplacedThreshold) {
    return TrackerPosition::displaced;
  }

  return TrackerPosition::neither_prompt_nor_displaced;
}

/** @brief FCC-hh tracker classification for compatible Delphes objects. */
template <Delphes::DelphesClassWithParticleOrXYZWithPtEtaPhi T>
inline TrackerPosition Tracker(const T &t) {
  return Tracker(Delphes::Origin(t), Delphes::Momentum(t));
}

/** @brief FCC-hh tracker classification for pointers to compatible Delphes objects. */
template <Delphes::DelphesClassWithParticleOrXYZWithPtEtaPhi T>
inline TrackerPosition Tracker(const T *const t) {
  return Tracker(Delphes::Origin(t), Delphes::Momentum(t));
}
}  // namespace FCChh
}  // namespace HNL
