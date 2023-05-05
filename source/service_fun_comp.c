#include "decimal.h"

bool is_less_big_mantissa(big_decimal *a, big_decimal *b) {
  int flag = false;
  for (int i = B_MAN_BITS - 1; i >= 0; --i) {
    if (is_big_bit_less(a, b, i, i)) {
      flag = true;
      break;
    } else if (is_big_bit_greater(a, b, i, i)) {
      flag = false;
      break;
    }
  }
  return flag;
}

bool is_big_bit_less(big_decimal *a, big_decimal *b, size_t a_ind,
                     size_t b_ind) {
  return b_get_bit(a, a_ind) < b_get_bit(b, b_ind);
}

bool is_big_bit_greater(big_decimal *a, big_decimal *b, size_t a_ind,
                        size_t b_ind) {
  return b_get_bit(a, a_ind) > b_get_bit(b, b_ind);
}

int is_big_less_or_equal(big_decimal a, big_decimal b) {
  big_decimal t_less = create_big_dec(0, 0, 0, 0, false);
  b_sub(a, b, &t_less);
  return b_get_sign(&t_less) || is_zero_big_decimal(&t_less);
}
