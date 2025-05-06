/**
** @file	ulibc.c
**
** @author	numerous CSCI-452 classes
**
** @brief	C implementations of user-level library functions.
*/

#include <common.h>

#include <lib.h>

/*
**********************************************
** MEMORY MANIPULATION FUNCTIONS
**********************************************
*/

#include <usercode/libs/stdlib/umem.c>

/*
**********************************************
** STRING MANIPULATION FUNCTIONS
**********************************************
*/

#include <usercode/libs/stdlib/ustrings.c>

/*
**********************************************
** CONVERSION FUNCTIONS
**********************************************
*/

#include <usercode/libs/stdlib/uconvert.c>

/*
**********************************************
** MATH FUNCTIONS
**********************************************
*/

#include <usercode/libs/stdlib/umath.c>

/*
**********************************************
** CONVENIENT "SHORTHAND" VERSIONS OF SYSCALLS
**********************************************
*/

#include <usercode/libs/syslib/uextras.c>

/*
**********************************************
** GRAPHICAL LIBRARY FUNCTIONS
**********************************************
*/

#include <usercode/libs/graphics/ugraphics.c>