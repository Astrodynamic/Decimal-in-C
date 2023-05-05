#include "decimal.h"

int b_add(big_decimal bv_1, big_decimal bv_2, big_decimal *result) {
  int flag = F_NV;
  if (b_get_sign(&bv_1) && !b_get_sign(&bv_2)) {
    b_clear_bit(&bv_1, B_DEC_BITS - 1);
    flag = b_sub(bv_2, bv_1, result);
  } else if (!b_get_sign(&bv_1) && b_get_sign(&bv_2)) {
    b_clear_bit(&bv_2, B_DEC_BITS - 1);
    flag = b_sub(bv_1, bv_2, result);
  } else if (b_get_sign(&bv_1) && b_get_sign(&bv_2)) {
    b_clear_bit(&bv_1, B_DEC_BITS - 1);
    b_clear_bit(&bv_2, B_DEC_BITS - 1);
    flag = b_add(bv_1, bv_2, result);
    if (flag == F_PI)
      flag = F_NI;
    else
      b_set_bit(result, B_DEC_BITS - 1);
  } else {
    if ((flag = normalize_numbers(&bv_1, &bv_2)) == F_NV) {
      if ((flag = make_add(bv_1, bv_2, result)) == F_NV) {
        int s_bv1 = b_get_scale(&bv_1);
        int s_bv2 = b_get_scale(&bv_2);
        b_set_scale(result, s_bv1 > s_bv2 ? s_bv1 : s_bv2);
      }
    }
  }
  return flag;
}

int b_sub(big_decimal bv_1, big_decimal bv_2, big_decimal *result) {
  int flag = F_NV;
  if (!b_get_sign(&bv_1) && b_get_sign(&bv_2)) {
    b_clear_bit(&bv_2, B_DEC_BITS - 1);
    flag = b_add(bv_1, bv_2, result);
  } else if (b_get_sign(&bv_1) && !b_get_sign(&bv_2)) {
    b_clear_bit(&bv_1, B_DEC_BITS - 1);
    if ((flag = b_add(bv_1, bv_2, result)) == F_PI) {
      flag = F_NI;
    } else {
      b_set_bit(result, B_DEC_BITS - 1);
    }
  } else if (b_get_sign(&bv_1) && b_get_sign(&bv_2)) {
    b_clear_bit(&bv_1, B_DEC_BITS - 1);
    b_clear_bit(&bv_2, B_DEC_BITS - 1);
    flag = b_sub(bv_2, bv_1, result);
  } else {
    if ((flag = normalize_numbers(&bv_1, &bv_2)) == F_NV) {
      if (is_less_big_mantissa(&bv_1, &bv_2)) {
        if ((flag = b_sub(bv_2, bv_1, result)) == F_NV) {
          b_set_bit(result, B_DEC_BITS - 1);
        }
      } else {
        make_sub(bv_1, bv_2, result);
        int s_bv1 = b_get_scale(&bv_1);
        int s_bv2 = b_get_scale(&bv_2);
        b_set_scale(result, s_bv1 > s_bv2 ? s_bv1 : s_bv2);
      }
    }
  }
  return flag;
}

int b_mul(big_decimal bv_1, big_decimal bv_2, big_decimal *result) {
  int flag = F_NV;
  bool sign = b_get_sign(&bv_1) ^ b_get_sign(&bv_2);
  memset(result, 0, sizeof(big_decimal));
  b_clear_bit(&bv_1, B_DEC_BITS - 1);
  b_clear_bit(&bv_2, B_DEC_BITS - 1);
  int scale = b_get_scale(&bv_1) + b_get_scale(&bv_2);
  if (is_zero_big_decimal(&bv_1) || is_zero_big_decimal(&bv_2)) {
    if (scale < 10) b_set_scale(result, scale);
    if (sign) b_set_bit(result, B_DEC_BITS - 1);
  } else {
    if ((flag = make_mul(bv_1, bv_2, result)) == F_NV) {
      b_set_scale(result, scale);
      if (sign) {
        b_set_bit(result, B_DEC_BITS - 1);
      }
    }
  }
  return flag;
}

int b_div(big_decimal bv_1, big_decimal bv_2, big_decimal *result) {
  int flag = F_NV;
  bool sign = b_get_sign(&bv_1) ^ b_get_sign(&bv_2);
  b_clear_bit(&bv_1, B_DEC_BITS - 1);
  b_clear_bit(&bv_2, B_DEC_BITS - 1);
  memset(result, 0, sizeof(big_decimal));
  if (is_zero_big_decimal(&bv_2)) {
    flag = F_NN;
  } else {
    if ((flag = normalize_numbers(&bv_1, &bv_2)) == F_NV) {
      flag = make_div(bv_1, bv_2, result);
      if (sign) b_set_bit(result, B_DEC_BITS - 1);
    }
  }
  return flag;
}

int b_mod(big_decimal bv_1, big_decimal bv_2, big_decimal *result) {
  int flag = F_NV;
  bool sign = b_get_sign(&bv_1);
  b_clear_bit(&bv_1, B_DEC_BITS - 1);
  b_clear_bit(&bv_2, B_DEC_BITS - 1);
  memset(result, 0, sizeof(big_decimal));
  if (is_zero_big_decimal(&bv_2)) {
    flag = F_NN;
  } else {
    if ((flag = normalize_numbers(&bv_1, &bv_2)) == F_NV) {
      big_decimal integer = create_big_dec(0, 0, 0, 0, false);
      flag = integer_part_calculation(bv_1, bv_2, &integer, result);
      if (sign) b_set_bit(result, B_DEC_BITS - 1);
    }
  }
  return flag;
}

int normalize_numbers(big_decimal *a, big_decimal *b) {
  int flag = F_NV;
  int scale = b_get_scale(a) - b_get_scale(b);
  big_decimal t_mv = create_big_dec(10, 0, 0, 1, false);
  if (scale < 0) {
    flag = multiple_in_loop(a, t_mv, abs(scale));
  } else if (scale > 0) {
    flag = multiple_in_loop(b, t_mv, abs(scale));
  }
  return flag;
}

int multiple_in_loop(big_decimal *src, big_decimal multiplicand, size_t times) {
  int flag = F_NV;
  for (size_t i = 0; i < times && flag == F_NV; ++i) {
    flag = b_mul(*src, multiplicand, src);
  }
  return flag;
}

int make_add(big_decimal bv_1, big_decimal bv_2, big_decimal *result) {
  int flag = F_NV, carry = 0;
  memset(result, 0, sizeof(big_decimal));
  for (size_t i = B_L_BITS; i < B_MAN_BITS; ++i) {
    carry += b_get_bit(&bv_1, i) + b_get_bit(&bv_2, i);
    if (carry & 0x1) b_set_bit(result, i);
    carry >>= 0x1;
  }
  if (carry) {
    flag = F_PI;
    memset(result, 0, sizeof(big_decimal));
  }
  return flag;
}

void make_sub(big_decimal bv_1, big_decimal bv_2, big_decimal *result) {
  memset(result, 0, sizeof(big_decimal));
  int carry = 0, diff;
  for (size_t i = B_L_BITS; i < B_MAN_BITS; ++i) {
    if (b_get_bit(&bv_1, i) >= b_get_bit(&bv_2, i) + carry) {
      diff = b_get_bit(&bv_1, i) - b_get_bit(&bv_2, i) - carry;
      carry = 0;
    } else {
      diff = b_get_bit(&bv_1, i) + 2 - b_get_bit(&bv_2, i) - carry;
      carry = 1;
    }
    if (diff) b_set_bit(result, i);
  }
}

int make_mul(big_decimal bv_1, big_decimal bv_2, big_decimal *result) {
  int flag = F_NV;
  memset(result, 0, sizeof(big_decimal));
  for (size_t i = B_L_BITS; i < B_MAN_BITS && flag == F_NV; ++i) {
    if (i != B_L_BITS) shift_bits_to_left(&bv_1, 1);
    if (b_get_bit(&bv_1, B_MAN_BITS) && b_get_bit(&bv_2, i)) {
      flag = F_PI;
    } else if (b_get_bit(&bv_2, i)) {
      flag = make_add(*result, bv_1, result);
    }
  }
  return flag;
}

int make_div(big_decimal bv_1, big_decimal bv_2, big_decimal *result) {
  int flag = F_NV;
  memset(result, 0, sizeof(big_decimal));
  big_decimal rest = create_big_dec(0, 0, 0, 0, false);
  if ((flag = integer_part_calculation(bv_1, bv_2, result, &rest)) == F_NV) {
    if (!is_zero_big_decimal(&rest)) {
      fractional_part(rest, bv_2, result);
    }
  }
  return flag;
}

void shift_bits_to_left(big_decimal *src, int shift) {
  for (int i = 0; i < shift; ++i) {
    for (int j = B_H_BITS; j >= B_L_BITS; --j) {
      if (b_get_bit(src, (j + 1) * 32 - 1)) b_set_bit(src, (j + 1) * 32);
      src->bits[j] <<= 1;
    }
  }
}

int integer_part_calculation(big_decimal bv_1, big_decimal bv_2,
                             big_decimal *result, big_decimal *rest) {
  int flag = F_NV;
  memset(rest, 0, sizeof(big_decimal));
  memset(result, 0, sizeof(big_decimal));
  b_clear_scale(&bv_1);
  b_clear_scale(&bv_2);
  for (int i = B_MAN_BITS - 1; i >= B_L_BITS; --i) {
    shift_bits_to_left(rest, 1);
    if (b_get_bit(&bv_1, i)) b_set_bit(rest, 0);
    if (is_big_less_or_equal(bv_2, *rest)) {
      b_set_bit(result, i);
      b_sub(*rest, bv_2, rest);
    }
  }
  return flag;
}

void fractional_part(big_decimal div, big_decimal den, big_decimal *result) {
  size_t offset = 0;
  big_decimal t_ten = create_big_dec(10, 0, 0, 0, false);
  big_decimal t_div = create_big_dec(0, 0, 0, 0, false);
  while (!is_zero_big_decimal(&div) && offset < EXP) {
    b_mul(*result, t_ten, result);
    b_mul(div, t_ten, &div);
    integer_part_calculation(div, den, &t_div, &div);
    b_add(*result, t_div, result);
    ++offset;
  }
  b_set_scale(result, offset);
}

castom_decimal integer_part_calculation_dec(castom_decimal value_1,
                                            castom_decimal value_2,
                                            castom_decimal *result) {
  castom_decimal rest = create_dec(0, 0, 0, get_scale(&value_1), false);
  memset(result, 0, sizeof(castom_decimal));
  for (int i = MAN_BITS - 1; i >= L_BITS; --i) {
    shift_bits_to_left_dec(&rest, 1);
    if (get_bit(&value_1, i)) set_bit(&rest, 0);
    if (castom_is_less_or_equal(value_2, rest)) {
      set_bit(result, i);
      castom_sub(rest, value_2, &rest);
    }
  }
  return rest;
}

void shift_bits_to_left_dec(castom_decimal *src, int shift) {
  for (int i = 0; i < shift; ++i) {
    for (int j = H_BITS; j >= L_BITS; --j) {
      if (get_bit(src, (j + 1) * 32 - 1)) set_bit(src, (j + 1) * 32);
      src->bits[j] <<= 1;
    }
  }
}

void trim_insignificant_zeros(castom_decimal src, castom_decimal *result) {
  int s_src = get_scale(&src);
  if (s_src > 0) {
    clear_scale(&src);
    castom_decimal t_den = create_dec(10, 0, 0, 0, false);
    castom_decimal t_res = create_dec(0, 0, 0, 0, false);
    castom_decimal rest;
    bool sign = get_bit(&src, DEC_BITS - 1);
    bool check = true;
    do {
      rest = integer_part_calculation_dec(src, t_den, &t_res);
      if (is_zero_decimal(&rest)) {
        src = t_res;
        --s_src;
      } else {
        check = false;
      }
    } while (check && s_src > 0);
    *result = src;
    set_scale(result, s_src);
    if (sign) set_bit(result, DEC_BITS - 1);
  } else {
    memcpy(result, &src, sizeof(castom_decimal));
  }
}
