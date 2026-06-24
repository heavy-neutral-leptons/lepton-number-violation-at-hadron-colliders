// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

// Standard Library
#include <filesystem>
#include <string>

// ROOT
#include "TMVA/Types.h"

namespace HNL::ML {
/**
 * TMVA methods supported by this module.
 *
 * The method is recovered from the dataset path and drives both the training configuration and the
 * run-time cut semantics used when applying the classifier.
 */
enum class MVAMethod : uint8_t {
  Cuts,  ///< Cuts
  BDT,   ///< BDT
};

/**
 * Return the canonical method name used in TMVA outputs.
 *
 * @param method Supported TMVA method.
 * @return String name used for booking and reading the method.
 */
std::string ToString(MVAMethod method);

/**
 * Infer the TMVA method from the dataset path.
 *
 * The current implementation searches for the substrings `Cuts` and `BDT` in the dataset path.
 *
 * @param datasetPath Path identifying the trained dataset.
 * @return Method encoded in the dataset path.
 */
MVAMethod IdentifyMethod(const std::filesystem::path& datasetPath);

/**
 * Return the method name inferred from the dataset path.
 *
 * @param datasetPath Path identifying the trained dataset.
 * @return Canonical method name.
 */
inline std::string IdentifyMethodName(const std::filesystem::path& datasetPath) {
  return ToString(IdentifyMethod(datasetPath));
}

/**
 * Convert a module method identifier into the TMVA enum value.
 *
 * @param method Supported TMVA method.
 * @return Matching `TMVA::Types::EMVA` value.
 */
TMVA::Types::EMVA EMVA(MVAMethod method);

/**
 * Build the path to the TMVA XML weights file for a trained dataset.
 *
 * The reader expects a file named `weights/HNL_<Method>.weights.xml` below the dataset directory.
 *
 * @param datasetPath Path identifying the trained dataset.
 * @param method Supported TMVA method.
 * @return Path to the XML weights file consumed by `TMVA::Reader`.
 */
std::filesystem::path WeightsPath(const std::filesystem::path& datasetPath, MVAMethod method);

/**
 * Return a simple default cut associated with a method.
 *
 * This is a coarse method-dependent threshold and not the optimized cut returned by
 * `MethodHistograms::OptimalCut`.
 *
 * @param method Supported TMVA method.
 * @return Method-dependent default threshold.
 */
double NaiveCut(MVAMethod method);

}  // namespace HNL::ML
