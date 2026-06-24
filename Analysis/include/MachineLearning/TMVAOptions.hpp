// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

// Standard Library
#include <string>

// HNL
#include "MachineLearning/Methods.hpp"

namespace HNL::ML::Options {
/**
 * Options forwarded to `TMVA::DataLoader::PrepareTrainingAndTestTree`.
 *
 * Event partitioning is performed explicitly in `DataLoaderWrapper`, so this string intentionally
 * leaves the TMVA-side split configuration empty.
 */
inline constexpr const char* const split{""};

/**
 * Options forwarded to the `TMVA::Factory` constructor.
 *
 * Transformations:
 * - I = Identity,
 * - D = Decorrelation
 * - P = Principal component analysis (PCA)
 * - U = Uniform
 * - G = Gaussianisation
 */
inline constexpr const char* const factory{
    "!V:!Silent:Color:DrawProgressBar:"
    // "Transformations=I;D;P;G,D:"
    "AnalysisType=Classification:"};

/**
 * Options forwarded to `TMVA::Factory::BookMethod` for `MVAMethod::Cuts`.
 */
inline constexpr const char* const cuts{"!H:!V:"};

/**
 * Options forwarded to `TMVA::Factory::BookMethod` for `MVAMethod::BDT`.
 */
inline constexpr const char* const bdt{
    "!H:!V:"
    "NTrees=800:"
    "MaxDepth=3:"
    "MinNodeSize=5%:"
    "nCuts=20:"
    "BoostType=AdaBoost:"
    "AdaBoostR2Loss=Quadratic:"
    "AdaBoostBeta=0.5:"
    "SeparationType=GiniIndex:"};

/**
 * Return the TMVA booking options associated with a supported method.
 *
 * @param method Supported TMVA method.
 * @return Option string passed to `TMVA::Factory::BookMethod`.
 */
std::string Method(MVAMethod method);
}  // namespace HNL::ML::Options
