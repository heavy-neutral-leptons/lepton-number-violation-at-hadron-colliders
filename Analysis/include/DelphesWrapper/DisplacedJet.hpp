// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

// ROOT
#include "Math/Point3D.h"
#include "TClass.h"
#include "TObject.h"

// Delphes
#include "classes/DelphesClasses.h"

// HNL
#include "Common/Log.hpp"
#include "DelphesWrapper/DelphesPosition.hpp"
#include "DelphesWrapper/DelphesVector3D.hpp"

namespace HNL::Delphes {
/** @brief Count jet tracks whose origin is compatible with a reference vertex. */
inline size_t CountCompatibleTracks(const ROOT::Math::XYZPoint& origin,
                                    const double tolerance_radius, const Jet& jet) {
  size_t n_displaced_tracks{0};

  for (const TObject* const constituent : jet.Constituents) {
    Log<4>("\tConstituent {}", constituent->IsA()->GetName());

    if (constituent == nullptr || constituent->IsA() != Track::Class()) {
      continue;
    }

    Track const* const track{dynamic_cast<Track const*>(constituent)};

    const ROOT::Math::XYZPoint vertex{Origin(*track)};
    const double displacement{std::sqrt((origin - vertex).Mag2())};

    Log<4>("\t ({}): {}/{} ({} {} {})", track->PID, displacement, tolerance_radius, track->X,
           track->Y, track->Z);

    if (displacement <= tolerance_radius) {
      ++n_displaced_tracks;
    }
  }

  Log<4>("\tJet has {} displaced tracks", n_displaced_tracks);

  return n_displaced_tracks;
}

/** @brief Count jet tracks classified as displaced by a tracker model. */
inline size_t CountDisplacedTracks(TrackerPosition tracker_function(const Track&), const Jet& jet) {
  size_t n_displaced_tracks{0};

  for (const TObject* const constituent : jet.Constituents) {
    if (constituent == nullptr || constituent->IsA() != Track::Class()) {
      continue;
    }

    Track const* const track{dynamic_cast<Track const*>(constituent)};
    if (tracker_function(*track) == TrackerPosition::displaced) {
      ++n_displaced_tracks;
    }
  }

  return n_displaced_tracks;
}

/** @brief Count jet truth particles whose origin is compatible with a reference vertex. */
inline size_t CountCompatibleParticles(const ROOT::Math::XYZPoint& origin,
                                       const double tolerance_radius, const Jet& jet) {
  size_t n_displaced_particles{0};

  for (const TObject* const constituent : jet.Particles) {
    Log<4>("\tParticle {}", constituent->IsA()->GetName());

    if (constituent == nullptr || constituent->IsA() != GenParticle::Class()) {
      continue;
    }

    GenParticle const* const particle{dynamic_cast<GenParticle const*>(constituent)};

    const ROOT::Math::XYZPoint vertex{Origin(*particle)};
    const double displacement{std::sqrt((origin - vertex).Mag2())};

    Log<4>("\t ({}): {}/{} ({} {} {})", particle->PID, displacement, tolerance_radius, particle->X,
           particle->Y, particle->Z);

    if (displacement <= tolerance_radius) {
      ++n_displaced_particles;
    }
  }

  Log<4>("\tJet has {} displaced particles", n_displaced_particles);

  return n_displaced_particles;
}

/** @brief Count jet truth particles classified as displaced by a tracker model. */
inline size_t CountDisplacedParticles(TrackerPosition tracker_function(const GenParticle&),
                                      const Jet& jet) {
  size_t n_displaced_tracks{0};

  for (const TObject* const constituent : jet.Particles) {
    if (constituent == nullptr || constituent->IsA() != GenParticle::Class()) {
      continue;
    }

    GenParticle const* const particle{dynamic_cast<GenParticle const*>(constituent)};
    if (tracker_function(*particle) == TrackerPosition::displaced) {
      ++n_displaced_tracks;
    }
  }

  return n_displaced_tracks;
}

/** @brief Compute the fraction of jet-track transverse momentum compatible with a vertex. */
inline double JetVertexFraction(const ROOT::Math::XYZPoint& origin, const double tolerance_radius,
                                const Jet& jet) {
  double vertex_pt{0};
  double total_pt{0};
  for (const TObject* const constituent : jet.Constituents) {
    Log<4>("\tConstituent {}", constituent->IsA()->GetName());

    if (constituent == nullptr || constituent->IsA() != Track::Class()) {
      continue;
    }

    Track const* const track{dynamic_cast<Track const*>(constituent)};

    const ROOT::Math::XYZPoint vertex{Origin(*track)};
    const double displacement{std::sqrt((origin - vertex).Mag2())};

    total_pt += track->PT;
    if (displacement <= tolerance_radius) vertex_pt += track->PT;
  }

  return vertex_pt / total_pt;
}
}  // namespace HNL::Delphes
