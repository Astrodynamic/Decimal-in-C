#ifndef SRC_S21_DECIMAL_H_
#define SRC_S21_DECIMAL_H_

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EXP 28

typedef enum { F_NV, F_PI, F_NI, F_NN } e_arithmetic_e_t;

#define INT_BITS (sizeof(int) * 8)

typedef enum { L_BITS, M_BITS, H_BITS, S_BITS, ALL_BITS } e_dec_bit_t;

#define DEC_BITS (ALL_BITS * INT_BITS)
#define MAN_BITS ((ALL_BITS - 1) * INT_BITS)

typedef struct {
  int bits[ALL_BITS];
} s21_decimal;

typedef enum {
  B_L_BITS,
  B_M1_BITS,
  B_M2_BITS,
  B_M3_BITS,
  B_M4_BITS,
  B_M5_BITS,
  B_H_BITS,
  B_S_BITS,
  B_ALL_BITS
} e_big_bit_t;

#define B_DEC_BITS (B_ALL_BITS * INT_BITS)
#define B_MAN_BITS ((B_ALL_BITS - 1) * INT_BITS)

typedef struct {
  int bits[B_ALL_BITS];
} big_decimal;

#define CREATE_ARITHMETIC_FUN(_1)                           \
  int s21_##_1(s21_decimal value_1, s21_decimal value_2,    \
               s21_decimal *result) {                       \
    big_decimal b_v1 = transformation_to_big_dec(&value_1); \
    big_decimal b_v2 = transformation_to_big_dec(&value_2); \
    big_decimal b_r = create_big_dec(0, 0, 0, 0, false);    \
    memset(result, 0, sizeof(s21_decimal));                 \
    int flag = F_NV;                                        \
    if ((flag = b_##_1(b_v1, b_v2, &b_r)) == F_NV)          \
      flag = transformation_to_dec(&b_r, result);           \
    return flag;                                            \
  }

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_mod(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

int s21_is_less(s21_decimal a, s21_decimal b);
int s21_is_less_or_equal(s21_decimal a, s21_decimal b);
int s21_is_greater(s21_decimal a, s21_decimal b);
int s21_is_greater_or_equal(s21_decimal a, s21_decimal b);
int s21_is_equal(s21_decimal a, s21_decimal b);
int s21_is_not_equal(s21_decimal a, s21_decimal b);

#define S21_DECIMAL_MAX 79228162514264337593543950335
#define S21_DECIMAL_MIN -79228162514264337593543950335

#define S21_DECIMAL_MAXF 79228162514264337593543950335.0F
#define S21_DECIMAL_MINF -79228162514264337593543950335.0F

typedef enum { SUCCESS, CONVERTING_ERROR } e_cast_code_t;

int s21_from_int_to_decimal(int src, s21_decimal *dst);
int s21_from_float_to_decimal(float src, s21_decimal *dst);
int s21_from_decimal_to_int(s21_decimal src, int *dst);
int s21_from_decimal_to_float(s21_decimal src, float *dst);

typedef enum { OK, C_ERROR } e_round_code_t;

int s21_floor(s21_decimal value, s21_decimal *result);
int s21_round(s21_decimal value, s21_decimal *result);
int s21_truncate(s21_decimal value, s21_decimal *result);
int s21_negate(s21_decimal value, s21_decimal *result);

bool get_bit(const s21_decimal *src, const size_t index);
bool b_get_bit(const big_decimal *src, const size_t index);
void set_bit(s21_decimal *src, const size_t index);
void b_set_bit(big_decimal *src, const size_t index);
void clear_bit(s21_decimal *src, const size_t index);
void b_clear_bit(big_decimal *src, const size_t index);
void toggle_bit(s21_decimal *src, const size_t index);
void b_toggle_bit(big_decimal *src, const size_t index);
int get_scale(const s21_decimal *src);
void clear_scale(s21_decimal *src);
void set_scale(s21_decimal *src, int scale);
int b_get_scale(const big_decimal *src);
void b_clear_scale(big_decimal *src);
void b_set_scale(big_decimal *src, int scale);
s21_decimal create_dec(int l_b, int m_b, int h_b, int scale, bool sign);
big_decimal create_big_dec(const int l_b, const int m1_b, const int m2_b,
                           const int scale, const bool sign);
big_decimal transformation_to_big_dec(const s21_decimal *src);
int transformation_to_dec(big_decimal *b_dec, s21_decimal *dec);
void print_decimal_in_bin(const s21_decimal *decimal);
void print_big_decimal_in_bin(const big_decimal *decimal);
bool get_sign(const s21_decimal *src);
bool b_get_sign(const big_decimal *src);
int get_remainder(const big_decimal *src);
void clear_remainder(big_decimal *src);
void set_remainder(big_decimal *src, int rest);
bool is_zero_decimal(s21_decimal *src);
bool is_zero_big_decimal(big_decimal *src);
void print_big_decimal(big_decimal src);
bool is_empty_big_part(big_decimal *src);
void copy_dec_bits_to_dec(big_decimal *b_dec, s21_decimal *dec);

int b_add(big_decimal bv_1, big_decimal bv_2, big_decimal *result);
int b_sub(big_decimal bv_1, big_decimal bv_2, big_decimal *result);
int b_mul(big_decimal bv_1, big_decimal bv_2, big_decimal *result);
int b_div(big_decimal bv_1, big_decimal bv_2, big_decimal *result);
int b_mod(big_decimal bv_1, big_decimal bv_2, big_decimal *result);
int normalize_numbers(big_decimal *a, big_decimal *b);
int multiple_in_loop(big_decimal *src, big_decimal multiplicand, size_t times);
int make_add(big_decimal bv_1, big_decimal bv_2, big_decimal *result);
void make_sub(big_decimal bv_1, big_decimal bv_2, big_decimal *result);
int make_mul(big_decimal bv_1, big_decimal bv_2, big_decimal *result);
int make_div(big_decimal bv_1, big_decimal bv_2, big_decimal *result);
void shift_bits_to_left(big_decimal *src, int shift);
int integer_part_calculation(big_decimal bv_1, big_decimal bv_2,
                             big_decimal *result, big_decimal *rest);
void fractional_part(big_decimal div, big_decimal den, big_decimal *result);
s21_decimal integer_part_calculation_dec(s21_decimal value_1,
                                         s21_decimal value_2,
                                         s21_decimal *result);
void shift_bits_to_left_dec(s21_decimal *src, int shift);
void trim_insignificant_zeros(s21_decimal src, s21_decimal *result);

void set_exp(s21_decimal *dst, int exp_bits);
int get_float_exp_bin(float src);
char *get_float_mantissa_bin(float src);
void set_mantissa(s21_decimal *dst, char *mantissa_bits);
char *get_decimal_bits_str(s21_decimal *src);
float get_float_from_dec_mantissa(const char *mantissa);
float build_float(float tmp, int exp);

bool is_less_big_mantissa(big_decimal *a, big_decimal *b);
bool is_big_bit_less(big_decimal *a, big_decimal *b, size_t a_ind,
                     size_t b_ind);
bool is_big_bit_greater(big_decimal *a, big_decimal *b, size_t a_ind,
                        size_t b_ind);
int is_big_less_or_equal(big_decimal a, big_decimal b);

#endif  // SRC_S21_DECIMAL_H_
