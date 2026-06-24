// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

namespace HNL {
/* ----- LHC ----- */
namespace LHC {
/** Centre-of-momentum energy (14 TeV) */
inline constexpr double comEnergy{14000};
}  // namespace LHC

// CMS
namespace CMS {
/** Maximum impact parameter for a track to be prompt (100 µm) */
inline constexpr double impactParameterPromptThreshold{0.1};
/** Minimum impact parameter for a track to be displaced (2 mm) */
inline constexpr double impactParameterDisplacedThreshold{2};

/** Maximum transverse displacement (60 cm) */
inline constexpr double maxDisplacementTransverse{600};
/** Maximum longitudinal displacement (150 cm) */
inline constexpr double maxDisplacementAxis{1500};

/** Spatial resolution (100 µm) */
inline constexpr double spatialResolution{0.1};  //
                                                 //
/** Maximum distance between compatible tracks (100 µm) */
inline constexpr double maxTrackDistance{0.1};
}  // namespace CMS

/* ----- FCC-ee ----- */
namespace Z {
/** Centre-of-momentum energy (90 GeV) */
inline constexpr double colliderEnergy{90};
}  // namespace Z

namespace IDEA {
/**
 * Minimum displacement for a track to be displaced (400 µm)
 *
 * Taken from figure 1 of 2210.17110.
 */
inline constexpr double minDisplacementThreshold{400e-3};  // 400 µm

/**
 * Maximum transverse displacement (1 m)
 *
 * According to Table 7.3 of https://doi.org/10.1140/epjst/e2019-900045-4, the tracker half
 * length/outer radius is 2.0m. The maximum transverse displacement is taken to be half of this.
 */
inline constexpr double maxDisplacementTransverse{1000};  // 1 m
/** Maximum longitudinal displacement (1 m) */
inline constexpr double maxDisplacementLongitudinal{1000};  // 1 m

inline constexpr double maxMuonTrackDistance{0.1};  // 100 µm

/** Spatial resolution (100 µm) */
inline constexpr double spatialResolution{0.1};  // 100 µm
}  // namespace IDEA

/* ----- FCC-hh ----- */
namespace FCChh {
/** Centre-of-momentum energy (100 TeV) */
inline constexpr double colliderEnergy{100000};

/* ----- Proton distance at the interaction point (60.5 nm) ----- */
inline constexpr double ipProtonDistance{60.5};  //

/** Maximum impact parameter for a track to be prompt (100 µm) */
inline constexpr double impactParameterPromptThreshold{0.1};
/** Minimum impact parameter for a track to be displaced (2 mm) */
inline constexpr double impactParameterDisplacedThreshold{2};

/** Maximum transverse displacement (60 cm) */
inline constexpr double maxDisplacementTransverse{600};
/** Maximum longitudinal displacement (150 cm) */
inline constexpr double maxDisplacementAxis{1500};
}  // namespace FCChh
}  // namespace HNL
