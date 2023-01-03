#include "s21_decimal.h"

/**
 * @brief Function to push cah into end of str
 *
 * @param str String where to push char
 * @param c Char to push
 */
void str_push(char *str, char c) {
  if (str) {
    str += strlen(str);
    *str = c;
    *(str + 1) = '\0';
  }
}

/**
 * @brief Function to reverse string str
 *
 * @param str String to reverse
 * @return char* Reversed string
 */
char *str_reverse(char *str) {
  char *new_str = calloc(BUFSIZ, sizeof(char));
  char *end = str + strlen(str) - 1;
  char *n = new_str;
  while (end >= str) *(n++) = *(end--);
  snprintf(str, BUFSIZ, "%s", new_str);
  free(new_str);
  return str;
}

/**
 * @brief Function to divide string even number by 2
 *
 * @param str String number
 * @return true If number is dividible
 * @return false Otherwise
 */
bool divide_str_by_two(char *str) {
  bool result = true;
  if (strlen(str) == 1 && (*str == '1' || *str == '0')) {
    result = false;
  } else {
    char *new_str = calloc(BUFSIZ, sizeof(char));
    char *save = str;
    int d = 0;
    while (*str) {
      if (d < 2) d = d * 10 + (*(str++) - '0');
      while (d < 2 && *str) {
        d = d * 10 + (*(str++) - '0');
        if (strlen(new_str)) str_push(new_str, '0');
      }
      str_push(new_str, (d / 2) + '0');
      d %= 2;
    }
    str = save;
    snprintf(str, BUFSIZ, "%s", new_str);
    free(new_str);
  }
  return result;
}

/**
 * @brief Function to get exponent bits from float number src
 *
 * @param src Float number
 * @return int Integer with exponent bits
 */
int get_float_exp_bin(float src) {
  int result = 0;
  char *fstr = calloc(BUFSIZ, sizeof(char));
  snprintf(fstr, BUFSIZ, "%e", src);
  if (*fstr != '0') {
    char *exp_sym = strrchr(fstr, 'e');
    bool m_flag = false;
    if (*(exp_sym + 1) == '-') m_flag = true;
    int exponent = (*(exp_sym + 2) - '0') * 10 + (*(exp_sym + 3) - '0');
    if (m_flag) exponent *= -1;
    *(exp_sym--) = '\0';
    int precision = 6;
    while (*(exp_sym--) == '0') {
      precision--;
      *(exp_sym + 1) = '\0';
    }
    memmove(fstr + 1, fstr + 2, strlen(fstr + 1));
    if ((int)(strlen(fstr) - 1) < exponent)
      result = 0;
    else
      result = precision - exponent;
  }
  free(fstr);
  return abs(result);
}

void *insert(const char *src, const char *str, size_t start_index) {
  char *result = NULL;
  if (start_index <= strlen(src)) {
    char *temp = calloc((strlen(src) - start_index + 1), sizeof(char));
    if (temp) {
      size_t j = 0;
      for (size_t i = start_index; i < strlen(src); i++, j++) {
        temp[j] = src[i];
      }
      temp[j] = '\0';
      result = calloc(200, sizeof(char));
      char *tt = result;
      for (size_t i = 0; i < start_index; i++, tt++) {
        *tt = src[i];
      }
      for (j = 0; j < strlen(str); j++, tt++) {
        *tt = str[j];
      }
      for (j = 0; j < strlen(temp); j++, tt++) {
        *tt = temp[j];
      }
      free(temp);
    }
  }
  return result;
}

/**
 * @brief Function to convert float number to integer string number
 *
 * @param src Float number to convert
 * @return char* Integer number in string form
 */
char *float_in_str(const float src) {
  char *str = calloc(BUFSIZ, sizeof(char));
  snprintf(str, BUFSIZ, "%e", src);
  char *e_sym = strchr(str, 'e');
  bool m_flag = false;
  if (*(e_sym + 1) == '-') m_flag = true;
  int exp = (*(e_sym + 2) - '0') * 10 + (*(e_sym + 3) - '0');
  if (m_flag) exp *= -1;
  *strchr(str, 'e') = '\0';
  char *str_end = str + strlen(str) - 1;
  while (*str_end == '0') *(str_end--) = '\0';
  if (*str_end == '.')
    *str_end = '\0';
  else
    memmove(str + 1, str + 2, strlen(str + 2) + 1);
  if (exp > 0)
    while ((int)strlen(str) - 1 < exp) str_push(str, '0');
  return str;
}

/**
 * @brief Function to determine if float string odd or even
 *
 * @param str String to determine
 * @return true if odd
 * @return false if even
 */
bool is_str_float_odd(const char *str) {
  bool result = false;
  char *s = (char *)str + strlen(str) - 1;
  if ((*s - '0') % 2) result = true;
  return result;
}

/**
 * @brief Function to decrement float string
 *
 * @param str Float string
 */
void dec_float_str(char *str) {
  str += strlen(str) - 1;
  *str -= 1;
}

/**
 * @brief Function to calculate float mantissa
 *
 * @param src Float value
 * @return char* Mantissa bits
 */
char *get_float_mantissa_bin(float src) {
  char *result = calloc(96 + 1, sizeof(char));
  char *fstr = float_in_str(src);
  if (is_str_float_odd(fstr)) {
    str_push(result, '1');
    dec_float_str(fstr);
  } else {
    str_push(result, '0');
  }
  while (divide_str_by_two(fstr)) {
    if (is_str_float_odd(fstr)) {
      str_push(result, '1');
      dec_float_str(fstr);
    } else {
      str_push(result, '0');
    }
  }
  result = str_reverse(result);
  free(fstr);
  return result;
}

/**
 * @brief Function to set exponent bits in decimal value
 *
 * @param dst Decimal value where to put exponent
 * @param exp_bits Float value where to get exponent
 */
void set_exp(s21_decimal *dst, int exp_bits) {
  int bit = 1 << 7;
  for (int i = 95 + 24; i >= 95 + 17; i--, bit >>= 1)
    if (!!(exp_bits & bit)) set_bit(dst, i);
}

/**
 * @brief Function to set mantissa bits in decimal value
 *
 * @param dst Decimal value where to put mantissa
 * @param mantissa_bits Float value where to get mantissa
 */
void set_mantissa(s21_decimal *dst, char *mantissa_bits) {
  char *end = mantissa_bits + strlen(mantissa_bits) - 1;
  for (int i = 0; i < 96 && end >= mantissa_bits; i++, end--) {
    if (*end == '1')
      set_bit(dst, i);
    else
      clear_bit(dst, i);
  }
}

/**
 * @brief Function to get low, medium and high bits of decimal value in string
 * form
 *
 * @param src Decimal value
 * @return char* Decimal bits in string form
 */
char *get_decimal_bits_str(s21_decimal *src) {
  char *result = NULL;
  if (src) {
    result = calloc(150, sizeof(char));
    for (int i = 95; i >= 0; i--) {
      if (get_bit(src, i))
        str_push(result, '1');
      else
        str_push(result, '0');
    }
  }
  return result;
}

/**
 * @brief Function to convert string decimal mantissa to float number
 *
 * @param mantissa Mantissa of decimal value
 * @return float Float result number
 */
float get_float_from_dec_mantissa(const char *mantissa) {
  float result = 0.0F, pow2 = 1.0F;
  char *end = (char *)mantissa + strlen(mantissa) - 1;
  do {
    if (*(end--) == '1') result += pow2;
    pow2 *= 2.0F;
  } while (end != mantissa);
  if (*mantissa == '1') result += pow2;
  return result;
}

/**
 * @brief Function to create float number using its integer part and exponent
 * part
 *
 * @param tmp Float integer part
 * @param exp Float exponent part
 * @return float Float number
 */
float build_float(float tmp, int exp) {
  char *float_str_int = calloc(BUFSIZ, sizeof(char));
  snprintf(float_str_int, BUFSIZ, "%050.0f", tmp);
  char *float_str = NULL;
  if (exp)
    float_str = (char *)insert(float_str_int, ".", strlen(float_str_int) - exp);
  else
    float_str = float_str_int;
  sscanf(float_str, "%f", &tmp);
  if (float_str_int == float_str) float_str = NULL;
  free(float_str_int);
  if (float_str) free(float_str);
  return tmp;
}
