/**
** @file	umath.h
**
** @author	AdamDernis
**
** @brief	C declaration of math library functions
*/

#ifndef STDLIB_MATH_H_
#define STDLIB_MATH_H_

#include <common.h>

#define PI 3.1415926535f
#define NaN 0.0/0.0

bool_t is_nan(double x);

double min(double x, double y);

double max(double x, double y);

double abs(double x);

int factorial(int n);

double pow(double base, int exponent);

double sqrt(double x);

double sign(double x);

double sin(double x);

double cos(double x);

double map_range(double x, double src_low, double src_high, double trg_low, double trg_high);

#endif