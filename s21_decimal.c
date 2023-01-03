#include "s21_decimal.h"

CREATE_ARITHMETIC_FUN(add);
CREATE_ARITHMETIC_FUN(sub);
CREATE_ARITHMETIC_FUN(mul);
CREATE_ARITHMETIC_FUN(div);
CREATE_ARITHMETIC_FUN(mod);

int s21_is_less(s21_decimal a, s21_decimal b) {
  s21_decimal t_less = create_dec(0, 0, 0, 0, false);
  int flag = s21_sub(a, b, &t_less);
  return get_sign(&t_less) && !is_zero_decimal(&t_less) && !flag;
}

int s21_is_less_or_equal(s21_decimal a, s21_decimal b) {
  s21_decimal t_less = create_dec(0, 0, 0, 0, false);
  int flag = s21_sub(a, b, &t_less);
  return (get_sign(&t_less) || is_zero_decimal(&t_less)) && !flag;
}

int s21_is_greater(s21_decimal a, s21_decimal b) {
  return !s21_is_less_or_equal(a, b);
}

int s21_is_greater_or_equal(s21_decimal a, s21_decimal b) {
  return !s21_is_less(a, b);
}

int s21_is_equal(s21_decimal a, s21_decimal b) {
  s21_decimal t_less = create_dec(0, 0, 0, 0, false);
  int flag = s21_sub(a, b, &t_less);
  return is_zero_decimal(&t_less) && !flag;
}

int s21_is_not_equal(s21_decimal a, s21_decimal b) {
  return !s21_is_equal(a, b);
}

/**
 * @brief Function converts int src into decimal dst
 *
 * @param src Value to convert from
 * @param dst Value to convert into
 * @return int 0 if OK; 1 if error
 */
int s21_from_int_to_decimal(int src, s21_decimal *dst) {
  int result = SUCCESS;
  if (dst) {
    memset(dst, 0, sizeof(s21_decimal));
    if (src < 0) {
      set_bit(dst, 127);
      src = ~src + 1;
    }
    dst->bits[L_BITS] = src;
    clear_bit(dst, 31);
  } else {
    result = CONVERTING_ERROR;
  }
  return result;
}

/**
 * @brief Function converts float src into decimal dst
 *
 * @param src Value to convert from
 * @param dst Value to convert into
 * @return int 0 if OK; 1 if error
 */
int s21_from_float_to_decimal(float src, s21_decimal *dst) {
  int result = SUCCESS;
  if (dst) {
    memset((void *)dst, 0, sizeof(s21_decimal));
    if (src != src || src > S21_DECIMAL_MAXF || src < S21_DECIMAL_MINF) {
      if (src < S21_DECIMAL_MINF) set_bit(dst, 127);
      result = CONVERTING_ERROR;
    } else {
      if (src < 0.0) {
        set_bit(dst, 127);
        src *= -1.0F;
      }
      int exp = get_float_exp_bin(src);
      if (exp > 28) {
        result = CONVERTING_ERROR;
        memset(dst, 0, sizeof(s21_decimal));
      } else {
        set_exp(dst, exp);
      }
      if (result != CONVERTING_ERROR) {
        char *mantissa = get_float_mantissa_bin(src);
        set_mantissa(dst, mantissa);
        free(mantissa);
      }
    }
  } else {
    result = CONVERTING_ERROR;
  }
  return result;
}

/**
 * @brief Function converts decimal src into int dst
 *
 * @param src Value to convert from
 * @param dst Value to convert into
 * @return int 0 if OK; 1 if error
 */
int s21_from_decimal_to_int(s21_decimal src, int *dst) {
  int result = SUCCESS;
  if (dst) {
    s21_decimal new_src;
    s21_truncate(src, &new_src);
    if (new_src.bits[M_BITS] || new_src.bits[H_BITS])
      result = CONVERTING_ERROR;
    else
      *dst = new_src.bits[L_BITS];
    if (get_sign(&src)) *dst *= -1;
  } else {
    result = CONVERTING_ERROR;
  }
  return result;
}

/**
 * @brief Function converts decimal src into float dst
 *
 * @param src Value to convert from
 * @param dst Value to convert into
 * @return int 0 if OK; 1 if error
 */
int s21_from_decimal_to_float(s21_decimal src, float *dst) {
  int result = SUCCESS;
  if (dst) {
    char *t = get_decimal_bits_str(&src);
    float intf = get_float_from_dec_mantissa(t);
    free(t);
    int exp = get_scale(&src);
    *dst = build_float(intf, exp);
    if (get_sign(&src)) *dst *= -1.0F;
  } else {
    result = CONVERTING_ERROR;
  }
  return result;
}

int s21_floor(s21_decimal value, s21_decimal *result) {
  memset(result, 0, sizeof(s21_decimal));
  trim_insignificant_zeros(value, &value);
  if (get_scale(&value)) {
    s21_truncate(value, result);
    if (get_sign(&value)) {
      s21_decimal temp = create_dec(1, 0, 0, 0, false);
      s21_sub(*result, temp, result);
    }
  } else {
    memcpy(result, &value, sizeof(s21_decimal));
  }
  return OK;
}

int s21_round(s21_decimal value, s21_decimal *result) {
  int r = OK;
  if (result) {
    s21_truncate(value, result);
    s21_decimal fract;
    s21_sub(value, *result, &fract);
    int (*cfunc)(s21_decimal, s21_decimal) = s21_is_greater_or_equal;
    float f05 = 0.5F;
    int i1 = 1;
    if (get_sign(&value)) {
      cfunc = s21_is_less_or_equal;
      f05 *= -1.0F;
      i1 *= -1;
    }
    s21_decimal dec05, dec1;
    s21_from_float_to_decimal(f05, &dec05);
    s21_from_int_to_decimal(i1, &dec1);
    if (cfunc(fract, dec05)) s21_add(*result, dec1, result);
  } else {
    r = C_ERROR;
  }
  return r;
}

int s21_truncate(s21_decimal value, s21_decimal *result) {
  s21_decimal denominator = create_dec(10, 0, 0, 0, false);
  int scale = get_scale(&value);
  int sign = get_bit(&value, DEC_BITS - 1);
  clear_scale(&value);
  while (scale > 0) {
    integer_part_calculation_dec(value, denominator, &value);
    scale--;
  }
  *result = value;
  if (sign) set_bit(result, DEC_BITS - 1);
  return OK;
}

int s21_negate(s21_decimal value, s21_decimal *result) {
  toggle_bit(&value, DEC_BITS - 1);
  *result = value;
  return OK;
}
