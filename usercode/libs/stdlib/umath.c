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

bool_t is_nan(double x) {
    union { uint64_t u; double f; } ieee754;
    ieee754.f = x;
    return ( (unsigned)(ieee754.u >> 32) & 0x7fffffff ) +
           ( (unsigned)ieee754.u != 0 ) > 0x7ff00000;
}

double min(double x, double y) {
    if (x < y) {
        return x;
    }

    return y;
} 

double max(double x, double y) {
    if (x > y) {
        return x;
    }

    return y;
} 

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

double sqrt(double x) {
    int start, end = 0;
    int mid;

    double result;

    while (start <= end)
    {
        mid = (start + end)/2;

        if (mid * mid == x) {
            result = mid;
            break;
        }

        if (mid * mid < x){
            result = start;
            start = mid + 1;
        } else{
            end = mid-1;
        }
    }
    
    float increment = 0.1;
    for (int i = 0; i < 5; i++){
        while (result * result <= x) {
            result+= increment;
        }
        
        result -= increment;
        increment /= 10; 
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

double cos(double x) {
    // Perform a phase translation and use sin
    return sin(x + (PI/2));
}

double map_range(double x, double src_low, double src_high, double trg_low, double trg_high) {
    return  ((trg_high - trg_low) / (src_high - src_low)) * (x - src_low);
}

#endif