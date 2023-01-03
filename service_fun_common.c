#include "s21_decimal.h"

bool get_bit(const s21_decimal *src, const size_t index) {
  int mask = (1U << (index % 0x20));
  return src->bits[index / 0x20] & mask;
}

bool b_get_bit(const big_decimal *src, const size_t index) {
  int mask = (1U << (index % 0x20));
  return src->bits[index / 0x20] & mask;
}

void set_bit(s21_decimal *src, const size_t index) {
  int mask = (1U << (index % 0x20));
  src->bits[index / 0x20] |= mask;
}

void b_set_bit(big_decimal *src, const size_t index) {
  int mask = (1U << (index % 0x20));
  src->bits[index / 0x20] |= mask;
}

void clear_bit(s21_decimal *src, const size_t index) {
  int mask = ~(1U << (index % 0x20));
  src->bits[index / 0x20] &= mask;
}

void b_clear_bit(big_decimal *src, const size_t index) {
  int mask = ~(1U << (index % 0x20));
  src->bits[index / 0x20] &= mask;
}

void toggle_bit(s21_decimal *src, const size_t index) {
  int mask = (1U << (index % 0x20));
  src->bits[index / 0x20] ^= mask;
}

// void b_toggle_bit(big_decimal *src, const size_t index) {
//   int mask = (1U << (index % 0x20));
//   src->bits[index / 0x20] ^= mask;
// }

int get_scale(const s21_decimal *src) {
  return (src->bits[S_BITS] & 0x00FF0000) >> 16;
}

void clear_scale(s21_decimal *src) { src->bits[S_BITS] &= (~0x00FF0000); }

void set_scale(s21_decimal *src, int scale) {
  clear_scale(src);
  src->bits[S_BITS] |= (scale << 16);
}

int b_get_scale(const big_decimal *src) {
  return (src->bits[B_S_BITS] & 0x00FF0000) >> 16;
}

void b_clear_scale(big_decimal *src) { src->bits[B_S_BITS] &= (~0x00FF0000); }

void b_set_scale(big_decimal *src, int scale) {
  b_clear_scale(src);
  src->bits[B_S_BITS] |= (scale << 16);
}

s21_decimal create_dec(int l_b, int m_b, int h_b, int scale, bool sign) {
  s21_decimal temp = {{l_b, m_b, h_b, ((scale << 16) | (sign << 31))}};
  return temp;
}

big_decimal create_big_dec(const int l_b, const int m1_b, const int m2_b,
                           const int scale, const bool sign) {
  big_decimal temp = {
      {l_b, m1_b, m2_b, 0, 0, 0, 0, ((scale << 16) | (sign << 31))}};
  return temp;
}

big_decimal transformation_to_big_dec(const s21_decimal *src) {
  big_decimal temp = create_big_dec(0, 0, 0, 0, false);
  for (size_t i = L_BITS; i < S_BITS; ++i) temp.bits[i] = src->bits[i];
  b_set_scale(&temp, get_scale(src));
  if (get_sign(src)) {
    b_set_bit(&temp, B_DEC_BITS - 1);
  }
  return temp;
}

// void print_decimal_in_bin(const s21_decimal *decimal) {
//   for (int i = DEC_BITS - 1; i >= 0; --i)
//     printf("%d%s", get_bit(decimal, i),
//            i % 8 == 0 ? i % 32 == 0 ? " | " : " " : "");
//   putc('\n', stdout);
// }

// void print_big_decimal_in_bin(const big_decimal *decimal) {
//   for (int i = B_DEC_BITS - 1; i >= 0; --i)
//     printf("%d%s", b_get_bit(decimal, i),
//            i % 8 == 0 ? i % 32 == 0 ? " | " : " " : "");
//   putc('\n', stdout);
// }

bool get_sign(const s21_decimal *src) { return get_bit(src, DEC_BITS - 1); }

bool b_get_sign(const big_decimal *src) {
  return b_get_bit(src, B_DEC_BITS - 1);
}

// int get_remainder(const big_decimal *src) {
//   return (src->bits[B_S_BITS] & 0x000000F0) >> 4;
// }

// void clear_remainder(big_decimal *src) { src->bits[B_S_BITS] &=
// (~0x000000F0); }

// void set_remainder(big_decimal *src, int rest) {
//   clear_remainder(src);
//   src->bits[B_S_BITS] |= (rest << 4);
// }

bool is_zero_decimal(s21_decimal *src) {
  bool flag = true;
  for (size_t i = L_BITS; i < S_BITS; ++i)
    if (src->bits[i] != 0) flag = false;
  return flag;
}

bool is_zero_big_decimal(big_decimal *src) {
  bool flag = true;
  for (size_t i = B_L_BITS; i < B_S_BITS; ++i)
    if (src->bits[i] != 0) flag = false;
  return flag;
}

bool is_empty_big_part(big_decimal *src) {
  bool flag = true;
  for (size_t i = S_BITS; i < B_S_BITS; ++i)
    if (src->bits[i] != 0) flag = false;
  return flag;
}

// void print_big_decimal(big_decimal src) {
//   char buf[65] = {};
//   memset(buf, 0, sizeof(buf));
//   big_decimal div = create_big_dec(0, 0, 0, 0, false);
//   big_decimal dev = create_big_dec(10, 0, 0, 0, false);
//   int scale = b_get_scale(&src);
//   bool sign = b_get_bit(&src, B_DEC_BITS - 1);
//   b_clear_scale(&src);
//   for (int i = 0; !is_zero_big_decimal(&src); ++i) {
//     if (i == scale && scale != 0) {
//       buf[i] = '.';
//     } else {
//       integer_part_calculation(src, dev, &src, &div);
//       buf[i] = div.bits[B_L_BITS] + '0';
//     }
//   }
//   if (sign)
//     putc('-', stdout);
//   else
//     putc('+', stdout);
//   for (int i = 64; i >= 0; --i) putc(buf[i], stdout);
// }

int transformation_to_dec(big_decimal *b_dec, s21_decimal *dec) {
  int flag = F_NV;
  int scale = b_get_scale(b_dec);
  bool sign = b_get_sign(b_dec);
  b_clear_scale(b_dec);
  big_decimal dev = create_big_dec(10, 0, 0, 0, false);
  big_decimal rest = create_big_dec(0, 0, 0, 0, false);
  while ((!is_empty_big_part(b_dec) && scale) || scale > EXP) {
    integer_part_calculation(*b_dec, dev, b_dec, &rest);
    --scale;
  }
  if (is_empty_big_part(b_dec)) {
    if (rest.bits[B_L_BITS] >= 5) {
      b_add(*b_dec, create_big_dec(1, 0, 0, 0, false), b_dec);
    }
    b_set_scale(b_dec, scale);
    if (sign) b_set_bit(b_dec, B_DEC_BITS - 1);
    copy_dec_bits_to_dec(b_dec, dec);
  } else if (sign) {
    flag = F_NI;
  } else {
    flag = F_PI;
  }
  return flag;
}

void copy_dec_bits_to_dec(big_decimal *b_dec, s21_decimal *dec) {
  for (size_t i = L_BITS; i < S_BITS; ++i) {
    dec->bits[i] = b_dec->bits[i];
  }
  set_scale(dec, b_get_scale(b_dec));
  if (b_get_sign(b_dec)) {
    set_bit(dec, DEC_BITS - 1);
  }
}
