// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

// Standard Library
#include <string>
#include <vector>

// ROOT
#include "TFile.h"

namespace HNL::Model {
/**
 * @brief Identifiers for the supported physics models.
 *
 * Values are serialized into ROOT metadata and used to reconstruct the matching concrete
 * `Model::Generic` implementation when reading generated samples.
 */
enum class ModelID : int {
  SM,               ///< Standard Model.
  HeavyN_Majorana,  ///< The Standard Model + Heavy Neutrinos at NLO in QCD..
  HeavyN_Dirac,     ///< The Standard Model + Heavy Neutrinos at NLO in QCD.
  pSPSS,            ///< Phenomenological symmetry protected seesaw scenario.
  SeesawICurrents,  ///< The Standard Model + Seesaw-I currents.
  SPSS,             ///< Symmetry-protected seesaw scenario.
};

/**
 * @brief Return the display name of a model.
 * @param model Model identifier.
 * @return Human-readable model name.
 */
std::string ToString(ModelID model);

/**
 * @brief Common interface implemented by all supported model parameter sets.
 *
 * Concrete model types expose their parameters through CSV, vector, and ROOT
 * serialization helpers so that the analysis code can treat them uniformly.
 */
struct Generic {
 public:
  /**
   * @brief Construct a model wrapper with a fixed identifier.
   * @param id Stable model identifier used for serialization and dispatch.
   */
  explicit Generic(const ModelID id) : id{id} {};

  Generic(const Generic &) = default;
  Generic(Generic &&) = default;
  Generic &operator=(const Generic &) = default;
  Generic &operator=(Generic &&) = delete;

  /** @brief Destroy the polymorphic model wrapper. */
  virtual ~Generic() = default;

  /** @brief Return the model identifier. */
  [[nodiscard]] virtual ModelID ID() const final { return id; }
  /** @brief Return the display name of the model. */
  [[nodiscard]] virtual std::string Name() const final { return ToString(id); }

  /**
   * @brief Return an owning copy of the concrete model.
   * @return New heap-allocated clone preserving the dynamic type.
   */
  [[nodiscard]] virtual std::unique_ptr<Generic> Clone() const = 0;

  /**
   * @brief Return the comma-separated header used for CSV serialization.
   * @return Stable header string for `CSV()`.
   */
  [[nodiscard]] virtual constexpr const char *CSVHeader() const = 0;
  /**
   * @brief Return the model parameters serialized as one CSV row.
   * @return Comma-separated parameter values matching `CSVHeader()`.
   */
  [[nodiscard]] virtual std::string CSV() const = 0;

  /**
   * @brief Return the model parameters in numeric form.
   * @return Ordered parameter vector matching the model-specific schema.
   */
  [[nodiscard]] virtual std::vector<double> Vector() const = 0;

  /**
   * @brief Return the model cross section scaling.
   */
  [[nodiscard]] virtual double XSectionScaling() const final { return xsectionEnhancement; };

  /**
   * @brief Write the model identifier and parameters to a ROOT file.
   * @param file Output ROOT file receiving the metadata objects.
   */
  virtual void WriteToFile(TFile &file) const final;

  double xsectionEnhancement{1.};

 private:
  ModelID id;
};
}  // namespace HNL::Model

namespace HNL::PID {
/**
 * @brief Return whether a PDG id corresponds to an HNL in the given model.
 *
 * The same PDG id can have different meaning across benchmarks, so the model identifier is required
 * to interpret the candidate consistently.
 *
 * @param modelID Physics model used to interpret the PDG id.
 * @param pid PDG id to test.
 * @return `true` when `pid` denotes an HNL in `modelID`.
 */
bool IsHNL(Model::ModelID modelID, int pid);
}  // namespace HNL::PID
