#include <math.h>
#include <stdlib.h>
#include <kernels/simd.h>
#include <utils/log.h>
#include <stdbool.h>

typedef struct fraction {
  __int128 numerator;
  __int128 denominator;
} fraction_t;


static inline __int128 gcd(__int128 l, __int128 r) {
  if (l < r) {
    __int128 tmp = l;
    l = r;
    r = tmp;
  }
  //printf("l %lld r %lld\n", l, r);
  while (r != 0) {
    __int128 tmp = r;
    r = l % r;
    l = tmp;
  }
  //printf("l %lld r %lld\n", l, r);
  return l;
}


static inline __int128 int128abs(__int128 num) {
  return num > 0 ? num : -num;
}


static inline fraction_t fraction_constructor(__int128 numerator, __int128 denominator) {
  fraction_t res; 
  res.numerator = numerator;
  res.denominator = denominator;
  return res;
}


static inline __int128 fraction_to_int128(fraction_t fraction) {
  return fraction.numerator / fraction.denominator;
}


static inline fraction_t ll_to_fraction(__int128 ll) {
  return fraction_constructor(ll, 1);
}


static inline fraction_t fraction_norm(fraction_t fraction) {
  fraction_t res;
  if (fraction.numerator == 0) {
    res.numerator = 0;
    res.denominator = 1;
    return res;
  }
  if (fraction.denominator < 0) {
    fraction.numerator = -fraction.numerator;
    fraction.denominator = -fraction.denominator;
  }
  __int128 m_gcd = gcd(int128abs(fraction.numerator), int128abs(fraction.denominator));
  //printf("fraction.numerator %lld fraction.denominator %lld gcd %lld\n", fraction.numerator, fraction.denominator, m_gcd);
  res.numerator = fraction.numerator / m_gcd;
  res.denominator = fraction.denominator / m_gcd;
  return res;
}


static inline fraction_t fraction_abs(fraction_t fraction) {
  fraction_t res; 
  res.numerator = int128abs(fraction.numerator);
  res.denominator = int128abs(fraction.denominator);
  return res;
}


static inline fraction_t fraction_plus(fraction_t l, fraction_t r) {
  fraction_t res; 
  __int128 m_gcd = gcd(int128abs(l.denominator), int128abs(r.denominator));
  __int128 m_lcm = l.denominator * r.denominator / m_gcd;
  __int128 l_numerator = l.numerator * (m_lcm / l.denominator);
  __int128 r_numerator = r.numerator * (m_lcm / r.denominator);
  res.numerator = l_numerator + r_numerator;
  res.denominator = m_lcm;
  return fraction_norm(res);
}


static inline fraction_t fraction_minus(fraction_t l, fraction_t r) {
  r.numerator = -r.numerator;
  return fraction_plus(l, r);
}


static inline fraction_t fraction_multi(fraction_t l, fraction_t r) {
  fraction_t res;
  res.numerator = l.numerator * r.numerator;
  res.denominator = l.denominator * r.denominator;
  return fraction_norm(res);
}


static inline fraction_t fraction_div(fraction_t l, fraction_t r) {
  fraction_t res;
  __int128 tmp = r.denominator;
  r.denominator = r.numerator;
  r.numerator = tmp;
  return fraction_multi(l, fraction_norm(r));
}


static inline bool fraction_greater(fraction_t l, fraction_t r) {
  fraction_t res = fraction_minus(l, r);
  if (res.numerator >= 0 && res.denominator < 0 ||
      res.numerator <= 0 && res.denominator > 0) {
    return false;
  } else {
    return true;
  }
}


static inline bool fraction_zero(fraction_t fraction) {
  return fraction.numerator == 0;
}


#ifdef DEBUG
static inline void fraction_matrix_print(fraction_t *fractions, size_t N) {
  size_t i, j;
  for (i = 0; i < N; ++i) {
    for (j = 0; j < N; ++j) {
      fraction_t fraction = ACCESS(N, fractions, i, j);
      printf("%lld/%lld ", fraction.numerator, fraction.denominator);
    }
    printf("\n");
  }
}
#endif


long long determinant_ll_simd_kernel(size_t N, long long *M) {
  size_t i, j, k;
  size_t max_loc;

  // Init fractions
  fraction_t *fractions = (fraction_t *)malloc(N * N * sizeof(fraction_t));
  for (i = 0; i < N * N; ++i) {
    fractions[i] = ll_to_fraction(M[i]);
  }

  fraction_t max_val = fraction_constructor(0, 1);
  fraction_t det = fraction_constructor(1, 1);
  fraction_t sign = fraction_constructor(1, 1);

#ifdef DEBUG
  fraction_matrix_print(fractions, N);
  printf("\n");
#endif
  for (i = 0; i < N; ++i) {
    max_loc = i;
    for (j = i; j < N; ++j) {
      if (fraction_zero(ACCESS(N, fractions, j, i)) == false) {
        max_val = fraction_abs(ACCESS(N, fractions, j, i));
        max_loc = j;
        break;
      }
    }
    if (max_loc != i) {
      sign = fraction_multi(fraction_constructor(-1, 1), sign);
      for (j = i; j < N; ++j) {
        fraction_t tmp = ACCESS(N, fractions, max_loc, j);
        ACCESS(N, fractions, max_loc, j) = ACCESS(N, fractions, i, j);
        ACCESS(N, fractions, i, j) = tmp;
      }
    }
    fraction_t piv = ACCESS(N, fractions, i, i);
#ifdef DEBUG
    printf("piv:\n");
    printf("%lld/%lld\n", piv.numerator, piv.denominator);
#endif
    for (j = i + 1; j < N; ++j) {
      if (fraction_zero(ACCESS(N, fractions, j, i)) == false) {
        fraction_t factor = fraction_multi(fraction_constructor(-1, 1), ACCESS(N, fractions, j, i));
        for (k = i; k < N; ++k) {
          ACCESS(N, fractions, j, k) = fraction_multi(ACCESS(N, fractions, j, k), piv);
          ACCESS(N, fractions, j, k) = fraction_plus(ACCESS(N, fractions, j, k),
                                       fraction_multi(ACCESS(N, fractions, i, k), factor));
          ACCESS(N, fractions, j, k) = fraction_div(ACCESS(N, fractions, j, k), det);
        }
      } else {
        fraction_t factor = fraction_div(piv, det);
        for (k = i; k < N; ++k) {
          ACCESS(N, fractions, j, k) = fraction_multi(ACCESS(N, fractions, j, k), factor);
        }
      }
    }
    det = piv;
#ifdef DEBUG
    fraction_matrix_print(fractions, N);
    printf("det:\n");
    printf("%lld/%lld", det.numerator, det.denominator);
    printf("\n");
#endif
  }

  free(fractions);
  long long num = fraction_to_int128(fraction_multi(sign, det));
  return num;
}

