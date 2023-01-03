

## Implementation of the decimal.h library functions

The functions of the decimal.h library described [above](#information) must be implemented:
- The library developed in C language of C11 standard using gcc compiler
- Do not use outdated and legacy language constructions and library functions.
- Make it as a static library
- The library developed according to the principles of structured programming;
- Full coverage of library functions code with unit-tests using the Check library
- Unit tests cover at least 80% of each function (checked using gcov)   
- Provide a Makefile for building the library and tests (with targets all, clean, test, s21_decimal.a, gcov_report)  
- The gcov_report target should generate a gcov report in the form of an html page. Unit tests must be run with gcov flags to do this
- Forbidden to use the __int128 type
- Trailing zeros can be as preserved as deleted (except for the `s21_truncate` function)
- The defined type must support numbers from -79,228,162,514,264,337,593,543,950,335 to +79,228,162,514,264,337,593,543,950,335.
