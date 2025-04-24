/**
** @file	uconvert.c
**
** @author	Adam Dernis
**
** @brief	C implementations of math library functions
*/

#ifndef STDLIB_MATH_SRC_INC
#define STDLIB_MATH_SRC_INC

#include "umath.h"

double abs(double x){
    if (x < 0){
        return -x;
    }
    
    return x;
}

int factorial(int n) {
    if (n == 0) {
        return 1;
    }
  
    return n * factorial(n - 1);
}
  
double pow(double base, int exponent) {
    double result = 1;
    for (int i = 0; i < exponent; i++) {
        result *= base;
    }
  
    return result;
}
  
double sign(double x) {
    if (x > 0) {
        return 1;
    } else if (x < 0) {
        return -1;
    } else {
        return 0;
    }
}

double sin(double x) {
    // Our implementation only works for numbers from -2*M_PI to 2*M_PI
    // Therefore, we use the fact that sin(x) == sin(x + 2*M_PI) for any x
    while (abs(x) >= 2 * PI) {
      x -= sign(x) * 2 * PI;
    }
  
    double result = x;
    // Here we calculate 4 terms of the series
    // We can compute more terms for better precision
    for (int i = 1; i <= 4; i++) {
        double term = (double)pow(x, 1 + 2 * i) / factorial(1 + 2 * i);
        if (i % 2 == 1) {
            result -= term;
        } else {
            result += term;
        }
    }
    return result;
}
#endif