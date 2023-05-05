# Decimal library

The Decimal library provides a decimal value type that represents decimal numbers ranging from positive 79,228,162,514,264,337,593,543,950,335 to negative 79,228,162,514,264,337,593,543,950,335. The Decimal type is appropriate for financial calculations that require large numbers of significant integral and fractional digits and no round-off errors. The Decimal type does not eliminate the need for rounding. Rather, it minimizes errors due to rounding.

## Binary representation
The binary representation of a Decimal number consists of a 1-bit sign, a 96-bit integer number, and a scaling factor used to divide the integer number and specify what portion of it is a decimal fraction. The scaling factor is implicitly the number 10, raised to an exponent ranging from 0 to 28. Decimal number can be implemented as a four-element array of 32-bit signed integers (int32_t).

### Arithmetic Operators

| Operator name | Operators  | Function                                                                           | 
| ------ | ------ |------------------------------------------------------------------------------------|
| Addition | + | int castom_add(castom_decimal value_1, castom_decimal value_2, castom_decimal *result)         |
| Subtraction | - | int castom_sub(castom_decimal value_1, castom_decimal value_2, castom_decimal *result) |
| Multiplication | * | int castom_mul(castom_decimal value_1, castom_decimal value_2, castom_decimal *result) | 
| Division | / | int castom_div(castom_decimal value_1, castom_decimal value_2, castom_decimal *result) |
| Modulo | Mod | int castom_mod(castom_decimal value_1, castom_decimal value_2, castom_decimal *result) |

The functions return the error code:
- 0 - OK
- 1 - the number is too large or equal to infinity
- 2 - the number is too small or equal to negative infinity
- 3 - division by 0

*Note on the numbers that do not fit into the mantissa:*
- *When getting numbers that do not fit into the mantissa during arithmetic operations, use bank rounding (for example, 79,228,162,514,264,337,593,543,950,335 - 0.6 = 79,228,162,514,264,337,593,543,950,334)*

*Note on the mod operation:*
- *If an overflow occurred as a result, discard the fractional part (for example, 70,000,000,000,000,000,000,000,000,000 % 0.001 = 0.000)*


### Comparison Operators

| Operator name | Operators  | Function | 
| ------ | ------ | ------ |
| Less than | < | int castom_is_less(castom_decimal, castom_decimal) |
| Less than or equal to | <= | int castom_is_less_or_equal(castom_decimal, castom_decimal) | 
| Greater than | > |  int castom_is_greater(castom_decimal, castom_decimal) |
| Greater than or equal to | >= | int castom_is_greater_or_equal(castom_decimal, castom_decimal) | 
| Equal to | == |  int castom_is_equal(castom_decimal, castom_decimal) |
| Not equal to | != |  int castom_is_not_equal(castom_decimal, castom_decimal) |

Return value:
- 0 - FALSE
- 1 - TRUE

### Convertors and parsers

| Convertor/parser | Function | 
| ------ | ------ |
| From int  | int castom_from_int_to_decimal(int src, castom_decimal *dst) |
| From float  | int castom_from_float_to_decimal(float src, castom_decimal *dst) |
| To int  | int castom_from_decimal_to_int(castom_decimal src, int *dst) |
| To float  | int castom_from_decimal_to_float(castom_decimal src, float *dst) |

Return value - code error:
- 0 - OK
- 1 - convertation error

*Note on the conversion of a float type number:*
- *If the numbers are too small (0 < |x| < 1e-28), return an error and value equal to 0*
- *If the numbers are too large (|x| > 79,228,162,514,264,337,593,543,950,335) or are equal to infinity, return an error*
- *When processing a number with the float type, convert all the digits contained in it*

*Note on the conversion from decimal type to int:*
- *If there is a fractional part in a decimal number, it should be discarded (for example, 0.9 is converted to 0)*


### Another functions

| Description | Function                                                         | 
| ------ |------------------------------------------------------------------|
| Rounds a specified Decimal number to the closest integer toward negative infinity. | int castom_floor(castom_decimal value, castom_decimal *result)            |	
| Rounds a decimal value to the nearest integer. | int castom_round(castom_decimal value, castom_decimal *result)    |
| Returns the integral digits of the specified Decimal; any fractional digits are discarded, including trailing zeroes. | int castom_truncate(castom_decimal value, castom_decimal *result) |
| Returns the result of multiplying the specified Decimal value by negative one. | int castom_negate(castom_decimal value, castom_decimal *result)   |

Return value - code error:
- 0 - OK
- 1 - calculation error



## Build
To build the library, run the following commands:
```
cmake -S . -B ./build
cmake --build ./build
```

## Usage
To use the library in your project, include the header file `decimal.h`:
```c
#include "decimal.h"
```

### Example usage
```c
#include "decimal.h"
#include <stdio.h>

int main() {
    castom_decimal x = {{4422, 5515, 9254, 0b10000000000000110000000000000000}};
    castom_decimal y = {{6312, 5616, 1844, 0b10000000000001010000000000000000}};
    castom_decimal z = castom_add(x, y);
    printf("%s\n", print_decimal_in_bin(z));
    return 0;
}
```

## Dependencies
The Decimal library has the following dependencies:
- CMake (version 3.10 or higher)

## Development
To run tests, use the following command:
```
cmake -S ./test -B ./test/build
cmake --build ./test/build
cd ./test/build/; ./TEST
```

To run cppcheck and clang-format, use the following commands:
```
cd build/; make cppcheck
cd build/; make clang-format
```

## License
The Decimal library is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.