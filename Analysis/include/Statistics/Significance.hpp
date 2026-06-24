// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

namespace HNL::Statistics {
/**
 * @brief Compute the Poisson significance.
 *
 * The significance of n measured events when λ expected events are expected is
 * \f[
 *    Z(n, \lambda) = \sqrt{-2\log\left( \frac{P(n | \lambda)}{P(\lambda | \lambda)} \right)} ,
 * \f]
 * where the Poisson probability mass function is
 * \f[
 *    P(n | \lambda) = \frac{\lambda^n}{n!} e^{-\lambda} .
 * \f]
 * This is the same as
 * \f[
 *    Z(n, \lambda) = \sqrt{2 \left( \lambda - n \left(1 - \log\frac{n}{\lambda} \right) \right)}
 * \f]
 * @param nMeasured Observed event count used in the likelihood ratio.
 * @param nExpected Expected mean event count under the reference hypothesis.
 * @return Significance in Gaussian-standard-deviation units.
 */
double PoissonSignificance(double nMeasured, double nExpected);

/**
 * @brief Compute the projected Poisson significance for model exclusion.
 *
 * Null hypothesis: Background and signal
 *
 * Alternative hypothesis: Background only
 *
 * \f[
 *    Z = Z_{Poisson}(B, B + S) ,
 * \f]
 * where \f$Z_{Poisson}(n, \lambda)\f$ is the Poisson significance.
 * @param nSignal Expected signal yield.
 * @param nBkgrnd Expected background yield.
 * @return Exclusion significance in Gaussian-standard-deviation units.
 */
double ExclusionSignificance(double nSignal, double nBkgrnd);

/**
 * @brief Compute the projected Poisson significance for model discovery.
 *
 * Null hypothesis: Background
 *
 * Alternative hypothesines: Background and signal
 *
 * \f[
 *    Z = Z_{Poisson}(B + S, B) ,
 * \f]
 * where \f$Z_{Poisson}(n, \lambda)\f$ is the Poisson significance.
 * @param nSignal Expected signal yield.
 * @param nBkgrnd Expected background yield.
 * @return Discovery significance in Gaussian-standard-deviation units.
 */
double DiscoverySignificance(double nSignal, double nBkgrnd);
}  // namespace HNL::Statistics
