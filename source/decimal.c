#include "decimal.h"

CREATE_ARITHMETIC_FUN(add)
CREATE_ARITHMETIC_FUN(sub)
CREATE_ARITHMETIC_FUN(mul)
CREATE_ARITHMETIC_FUN(div)
CREATE_ARITHMETIC_FUN(mod)

int castom_is_less(castom_decimal a, castom_decimal b) {
  castom_decimal t_less = create_dec(0, 0, 0, 0, false);
  int flag = castom_sub(a, b, &t_less);
  return get_sign(&t_less) && !is_zero_decimal(&t_less) && !flag;
}

int castom_is_less_or_equal(castom_decimal a, castom_decimal b) {
  castom_decimal t_less = create_dec(0, 0, 0, 0, false);
  int flag = castom_sub(a, b, &t_less);
  return (get_sign(&t_less) || is_zero_decimal(&t_less)) && !flag;
}

int castom_is_greater(castom_decimal a, castom_decimal b) {
  return !castom_is_less_or_equal(a, b);
}

int castom_is_greater_or_equal(castom_decimal a, castom_decimal b) {
  return !castom_is_less(a, b);
}

int castom_is_equal(castom_decimal a, castom_decimal b) {
  castom_decimal t_less = create_dec(0, 0, 0, 0, false);
  int flag = castom_sub(a, b, &t_less);
  return is_zero_decimal(&t_less) && !flag;
}

int castom_is_not_equal(castom_decimal a, castom_decimal b) {
  return !castom_is_equal(a, b);
}

int castom_from_int_to_decimal(int src, castom_decimal *dst) {
  int result = SUCCESS;
  if (dst) {
    memset(dst, 0, sizeof(castom_decimal));
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

int castom_from_float_to_decimal(float src, castom_decimal *dst) {
  int result = SUCCESS;
  if (dst) {
    memset((void *)dst, 0, sizeof(castom_decimal));
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
        memset(dst, 0, sizeof(castom_decimal));
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

int castom_from_decimal_to_int(castom_decimal src, int *dst) {
  int result = SUCCESS;
  if (dst) {
    castom_decimal new_src;
    castom_truncate(src, &new_src);
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

int castom_from_decimal_to_float(castom_decimal src, float *dst) {
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

int castom_floor(castom_decimal value, castom_decimal *result) {
  memset(result, 0, sizeof(castom_decimal));
  trim_insignificant_zeros(value, &value);
  if (get_scale(&value)) {
    castom_truncate(value, result);
    if (get_sign(&value)) {
      castom_decimal temp = create_dec(1, 0, 0, 0, false);
      castom_sub(*result, temp, result);
    }
  } else {
    memcpy(result, &value, sizeof(castom_decimal));
  }
  return OK;
}

int castom_round(castom_decimal value, castom_decimal *result) {
  int r = OK;
  if (result) {
    castom_truncate(value, result);
    castom_decimal fract;
    castom_sub(value, *result, &fract);
    int (*cfunc)(castom_decimal, castom_decimal) = castom_is_greater_or_equal;
    float f05 = 0.5F;
    int i1 = 1;
    if (get_sign(&value)) {
      cfunc = castom_is_less_or_equal;
      f05 *= -1.0F;
      i1 *= -1;
    }
    castom_decimal dec05, dec1;
    castom_from_float_to_decimal(f05, &dec05);
    castom_from_int_to_decimal(i1, &dec1);
    if (cfunc(fract, dec05)) castom_add(*result, dec1, result);
  } else {
    r = C_ERROR;
  }
  return r;
}

int castom_truncate(castom_decimal value, castom_decimal *result) {
  castom_decimal denominator = create_dec(10, 0, 0, 0, false);
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

int castom_negate(castom_decimal value, castom_decimal *result) {
  toggle_bit(&value, DEC_BITS - 1);
  *result = value;
  return OK;
}
