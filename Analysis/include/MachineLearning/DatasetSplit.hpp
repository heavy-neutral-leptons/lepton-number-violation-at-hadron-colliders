// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

// Standard Library
#include <concepts>
#include <cstdint>

namespace HNL::ML {
/**
 * Enumeration of dataset partitions used in machine learning workflows.
 */
enum class DatasetSplit : uint8_t {
  Training,    ///< The training dataset is forwarded to TMVA training histograms and training trees
  Validation,  ///< The validation dataset is forwarded to TMVA validation histograms and test trees
  Application  ///< The application dataset is reserved for held-out event counting during
               ///< classifier
};

/**
 * Assign an event to a dataset split based on its index.
 *
 * This function deterministically partitions events into three subsets using a modulo-based scheme:
 * - 10% of events are assigned to the training set
 * - 10% of events are assigned to the validation set
 * - 80% of events are assigned to the application set
 *
 * @param idx Index of the event in the processed tree.
 * @return Assigned dataset partition.
 */
template <std::integral T>
constexpr DatasetSplit AssignDatasetSplit(const T idx) {
  switch (idx % 10) {
    case 0:
      return DatasetSplit::Training;
    case 1:
      return DatasetSplit::Validation;
    default:
      return DatasetSplit::Application;
  }
}

/**
 * Test whether an event index belongs to the training partition.
 *
 * @param idx Index of the event in the processed tree.
 * @return `true` when the event is assigned to `DatasetSplit::Training`.
 */
template <std::integral T>
constexpr bool ToTrainingSet(const T idx) {
  return AssignDatasetSplit(idx) == DatasetSplit::Training;
}

/**
 * Test whether an event index belongs to the validation partition.
 *
 * @param idx Index of the event in the processed tree.
 * @return `true` when the event is assigned to `DatasetSplit::Validation`.
 */
template <std::integral T>
constexpr bool ToValidationSet(const T idx) {
  return AssignDatasetSplit(idx) == DatasetSplit::Validation;
}

/**
 * Test whether an event index belongs to the held-out application partition.
 *
 * @param idx Index of the event in the processed tree.
 * @return `true` when the event is assigned to `DatasetSplit::Application`.
 */
template <std::integral T>
constexpr bool ToApplicationSet(const T idx) {
  return AssignDatasetSplit(idx) == DatasetSplit::Application;
}
}  // namespace HNL::ML
