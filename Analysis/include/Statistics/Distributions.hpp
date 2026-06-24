// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

// Standard Library
#include <cstddef>

namespace HNL::Statistics {
///@name Logistic transforms
///@{
/**
 * @brief Evaluate a centered logistic remapping.
 * @param x Input value.
 * @param x0 Midpoint of the transition.
 * @return Value in the interval `[-1, 1]`.
 */
double Logistic(double x, double x0 = 0);
///@}

///@name Polynomial helpers
///@{
/**
 * @brief Evaluate a polynomial using Horner's method.
 * @param x Evaluation point.
 * @param a Pointer to the coefficient array.
 * @param N Highest coefficient index used by the recurrence.
 * @return Polynomial value at `x`.
 */
long double Polynomialeval(long double x, long double *a, size_t N);
/**
 * @brief Evaluate a shifted polynomial recurrence used by the special-function helpers.
 * @param x Evaluation point.
 * @param a Pointer to the coefficient array.
 * @param N Highest coefficient index used by the recurrence.
 * @return Polynomial value at `x`.
 */
long double Polynomial1eval(long double x, long double *a, size_t N);
///@}

///@name Incomplete-gamma and chi-squared helpers
///@{
/**
 * @brief Evaluate the natural logarithm of the gamma function.
 * @param x Function argument.
 * @return `log(Gamma(x))`.
 */
long double lgam(long double x);
/**
 * @brief Evaluate the lower regularized incomplete gamma helper used in this module.
 * @param a Shape parameter.
 * @param x Upper integration bound.
 * @return Incomplete gamma helper value.
 */
long double igam(long double a, long double x);
/**
 * @brief Evaluate the complementary incomplete gamma helper used in this module.
 * @param a Shape parameter.
 * @param x Lower integration bound.
 * @return Complementary incomplete gamma helper value.
 */
long double igamc(long double a, long double x);
/** @brief Alias for `igamc`. */
long double inc_gamma_c(long double a, long double x);
/**
 * @brief Evaluate the complementary chi-squared cumulative distribution.
 * @param x Test statistic.
 * @param r Number of chi-squared degrees of freedom.
 * @param x0 Optional horizontal shift applied before evaluation.
 * @return Upper-tail chi-squared probability.
 */
long double chisquared_cdf_c(long double x, long double r, long double x0 = 0);
///@}

///@name Gaussian-tail quantile helpers
///@{
/**
 * @brief Evaluate the inverse standard normal cumulative distribution helper.
 * @param y0 Upper-tail probability-like input.
 * @return Corresponding standard-normal quantile.
 */
long double ndtri(long double y0);
/**
 * @brief Convert an upper-tail probability into a Gaussian quantile.
 * @param z Upper-tail probability.
 * @param sigma Gaussian width.
 * @return Quantile in the same units as `sigma`.
 */
long double normal_quantile_c(long double z, long double sigma);
/** @brief Alias for `normal_quantile_c`. */
long double gaussian_quantile_c(long double z, long double sigma);
///@}

/**
 * Normal distribution probability density function
 *
 * Adapted from: https://stackoverflow.com/a/10848293
 *
 * @param Z Significance, in standard deviations (Z / σ)
 * @return Confidence level from 0 to 1
 */
// template <std::floating_point T>
// consteval T NormalDistribution(T mean, T stdDev, T x) {
//   constexpr T norm{1 / std::sqrt(2 * std::numbers::pi)};
//   const T exp{(x - mean) / stdDev};
//
//   return norm / stdDev * std::exp(-T(0.5) * exp * exp);
// }
}  // namespace HNL::Statistics
