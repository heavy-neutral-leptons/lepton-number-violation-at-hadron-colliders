// Copyright 2023-2026 Bruno M. S. Oliveira

// Standard Library
#include <cmath>

// ROOT
#include "Math/Math.h"

// HNL
#include "Statistics/Distributions.hpp"

namespace HNL::Statistics {
/* ----- Logistic ----- */
double Logistic(const double x, const double x0) { return 2 / (1 + std::exp(-(x - x0))) - 1; }

/* ----- Polynomial ----- */
long double Polynomialeval(const long double x, const long double *const a, size_t N) {
  if (N == 0) {
    return a[0];
  } else {
    long double pom{a[0]};
    for (size_t i{1}; i <= N; ++i) pom = pom * x + a[i];
    return pom;
  }
}

long double Polynomial1eval(const long double x, const long double *const a, size_t N) {
  if (N == 0) {
    return a[0];
  } else {
    long double pom{x + a[0]};
    for (size_t i{1}; i < N; ++i) pom = pom * x + a[i];
    return pom;
  }
}

/* ----- chisquared_cdf_ ----- */
static constexpr long double kMACHEP{1.11022302462515654042363166809e-16};
static constexpr long double kMAXLGM{2.556348e305};

static constexpr long double kBig{4.503599627370496e15};
static constexpr long double kBiginv{2.22044604925031308085e-16};
static constexpr double LS2PI{0.91893853320467274178};

static constexpr long double A[]{8.11614167470508450300E-4, -5.95061904284301438324E-4,
                                 7.93650340457716943945E-4, -2.77777777730099687205E-3,
                                 8.33333333333331927722E-2};

static constexpr long double B[]{-1.37825152569120859100E3, -3.88016315134637840924E4,
                                 -3.31612992738871184744E5, -1.16237097492762307383E6,
                                 -1.72173700820839662146E6, -8.53555664245765465627E5};

static constexpr long double C[]{-3.51815701436523470549E2, -1.70642106651881159223E4,
                                 -2.20528590553854454839E5, -1.13933444367982507207E6,
                                 -2.53252307177582951285E6, -2.01889141433532773231E6};

long double lgam(long double x) {
  long double p, q, u, w, z;
  int64_t i;

  int64_t sgngam{1};

  if (x >= std::numeric_limits<long double>::infinity())
    return (std::numeric_limits<long double>::infinity());

  if (x < -34.0) {
    q = -x;
    w = lgam(q);
    p = std::floor(q);
    if (p == q) {  // _unur_FP_same(p,q)
      return (std::numeric_limits<long double>::infinity());
    }
    i = (int64_t)p;
    if ((i & 1) == 0) {
      sgngam = -1;
    } else {
      sgngam = 1;
    }
    z = q - p;
    if (z > 0.5) {
      p += 1.0;
      z = p - q;
    }
    z = q * std::sin(ROOT::Math::Pi() * z);
    if (z == 0) return (std::numeric_limits<long double>::infinity());
    z = std::log(ROOT::Math::Pi()) - std::log(z) - w;
    return (z);
  }

  if (x < 13.0) {
    z = 1.0;
    p = 0.0;
    u = x;
    while (u >= 3.0) {
      p -= 1.0;
      u = x + p;
      z *= u;
    }
    while (u < 2.0) {
      if (u == 0) return (std::numeric_limits<long double>::infinity());
      z /= u;
      p += 1.0;
      u = x + p;
    }
    if (z < 0.0) {
      sgngam = -1;
      z = -z;
    } else {
      sgngam = 1;
    }
    if (u == 2.0) return (std::log(z));
    p -= 2.0;
    x = x + p;
    p = x * Polynomialeval(x, B, 5) / Polynomial1eval(x, C, 6);
    return (std::log(z) + p);
  }

  if (x > kMAXLGM) return (sgngam * std::numeric_limits<long double>::infinity());

  q = (x - 0.5) * std::log(x) - x + LS2PI;
  if (x > 1.0e8) return (q);

  p = 1.0 / (x * x);
  if (x >= 1000.0) {
    q += ((7.9365079365079365079365e-4 * p - 2.7777777777777777777778e-3) * p +
          0.0833333333333333333333) /
         x;
  } else {
    q += Polynomialeval(p, A, 4) / x;
  }
  return (q);
}

long double igam(const long double a, const long double x) {
  long double ans, ax, c, r;

  // LM: for negative values returns 1.0 instead of zero
  // This is correct if a is a negative integer since Gamma(-n) = +/- inf
  if (a <= 0) return 1.0;

  if (x <= 0) return 0.0;

  if ((x > 1.0) && (x > a)) return (1.0 - igamc(a, x));

  /* Compute  x**a * exp(-x) / gamma(a)  */
  ax = a * std::log(x) - x - lgam(a);

  ax = std::exp(ax);

  /* power series */
  r = a;
  c = 1.0;
  ans = 1.0;

  do {
    r += 1.0;
    c *= x / r;
    ans += c;
  } while (c / ans > kMACHEP);

  return (ans * ax / a);
}

long double igamc(const long double a, const long double x) {
  long double ans, ax, c, yc, r, t, y, z;
  long double pk, pkm1, pkm2, qk, qkm1, qkm2;

  // LM: for negative values returns 0.0
  // This is correct if a is a negative integer since Gamma(-n) = +/- inf
  if (a <= 0) return 0.0;

  if (x <= 0) return 1.0;

  if ((x < 1.0) || (x < a)) return (1.0 - igam(a, x));

  ax = a * std::log(x) - x - lgam(a);

  ax = std::exp(ax);

  /* continued fraction */
  y = 1.0 - a;
  z = x + y + 1.0;
  c = 0.0;
  pkm2 = 1.0;
  qkm2 = x;
  pkm1 = x + 1.0;
  qkm1 = z * x;
  ans = pkm1 / qkm1;

  do {
    c += 1.0;
    y += 1.0;
    z += 2.0;
    yc = y * c;
    pk = pkm1 * z - pkm2 * yc;
    qk = qkm1 * z - qkm2 * yc;
    if (qk) {
      r = pk / qk;
      t = std::abs((ans - r) / r);
      ans = r;
    } else {
      t = 1.0;
    }
    pkm2 = pkm1;
    pkm1 = pk;
    qkm2 = qkm1;
    qkm1 = qk;
    if (std::abs(pk) > kBig) {
      pkm2 *= kBiginv;
      pkm1 *= kBiginv;
      qkm2 *= kBiginv;
      qkm1 *= kBiginv;
    }
  } while (t > kMACHEP);

  return (ans * ax);
}

long double inc_gamma_c(const long double a, const long double x) { return igamc(a, x); }

long double chisquared_cdf_c(const long double x, const long double r, const long double x0) {
  return inc_gamma_c(0.5 * r, 0.5 * (x - x0));
}

/* ----- gaussian_quantile_c ----- */
static constexpr long double s2pi{2.50662827463100050242E0};

static constexpr long double P0[5]{
    -5.99633501014107895267E1, 9.80010754185999661536E1,  -5.66762857469070293439E1,
    1.39312609387279679503E1,  -1.23916583867381258016E0,
};

static constexpr long double Q0[8]{
    1.95448858338141759834E0,  4.67627912898881538453E0,  8.63602421390890590575E1,
    -2.25462687854119370527E2, 2.00260212380060660359E2,  -8.20372256168333339912E1,
    1.59056225126211695515E1,  -1.18331621121330003142E0,
};

static constexpr long double P1[9]{
    4.05544892305962419923E0,   3.15251094599893866154E1,   5.71628192246421288162E1,
    4.40805073893200834700E1,   1.46849561928858024014E1,   2.18663306850790267539E0,
    -1.40256079171354495875E-1, -3.50424626827848203418E-2, -8.57456785154685413611E-4,
};

static constexpr long double Q1[8]{
    1.57799883256466749731E1,   4.53907635128879210584E1,   4.13172038254672030440E1,
    1.50425385692907503408E1,   2.50464946208309415979E0,   -1.42182922854787788574E-1,
    -3.80806407691578277194E-2, -9.33259480895457427372E-4,
};

static constexpr long double P2[9]{
    3.23774891776946035970E0,  6.91522889068984211695E0,  3.93881025292474443415E0,
    1.33303460815807542389E0,  2.01485389549179081538E-1, 1.23716634817820021358E-2,
    3.01581553508235416007E-4, 2.65806974686737550832E-6, 6.23974539184983293730E-9,
};

static constexpr long double Q2[8]{
    6.02427039364742014255E0,  3.67983563856160859403E0,  1.37702099489081330271E0,
    2.16236993594496635890E-1, 1.34204006088543189037E-2, 3.28014464682127739104E-4,
    2.89247864745380683936E-6, 6.79019408009981274425E-9,
};

long double ndtri(const long double y0) {
  long double x, y, z, y2, x0, x1;
  int64_t code;
  if (y0 <= 0.0) return (-std::numeric_limits<long double>::infinity());
  if (y0 >= 1.0) return (+std::numeric_limits<long double>::infinity());
  code = 1;
  y = y0;
  if (y > (1.0 - 0.13533528323661269189)) {
    y = 1.0 - y;
    code = 0;
  }
  if (y > 0.13533528323661269189) {
    y = y - 0.5;
    y2 = y * y;
    x = y + y * (y2 * Polynomialeval(y2, P0, 4) / Polynomial1eval(y2, Q0, 8));
    x = x * s2pi;
    return (x);
  }
  x = std::sqrt(-2.0 * std::log(y));
  x0 = x - std::log(x) / x;
  z = 1.0 / x;
  if (x < 8.0) {
    x1 = z * Polynomialeval(z, P1, 8) / Polynomial1eval(z, Q1, 8);
  } else {
    x1 = z * Polynomialeval(z, P2, 8) / Polynomial1eval(z, Q2, 8);
  }
  x = x0 - x1;
  if (code != 0) x = -x;
  return (x);
}

long double normal_quantile_c(const long double z, const long double sigma) {
  return -sigma * ndtri(z);
}

long double gaussian_quantile_c(const long double z, const long double sigma) {
  return normal_quantile_c(z, sigma);
}
}  // namespace HNL::Statistics
