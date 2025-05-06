/**
** @file    uextras.c
**
** @author  Adam Dernis
**
** @brief   Additional "shorthand" versions of system calls declarations
*/

#ifndef EXTRAS_H_
#define EXTRAS_H_

#include <common.h>

/**
** wait - wait for any child to exit
**
** usage:   pid = wait(&status)
**
** Calls waitpid(0,status)
**
** @param status Pointer to int32_t into which the child's status is placed,
**               or NULL
**
** @return  The PID of the terminated child, or an error code
*/
int wait( int32_t *status );

/**
** spawn - create a new process running a different program
**
** usage:       n = spawn(entry,args)
**
** Creates a new process and then execs 'entry'
**
** @param entry The entry point of the 'main' function for the process
** @param args  The command-line argument vector for the process
**
** @return      The PID of the child, or an error code
*/
int spawn( uint32_t entry, char **args );

/**
** cwritech(ch) - write a single character to the console
**
** @param ch The character to write
**
** @return  The return value from calling write()
*/
int cwritech( char ch );

/**
** cwrites(str) - write a NUL-terminated string to the console
**
** @param str The string to write
**
*/
int cwrites( const char *str );

/**
** cwrite(buf,leng) - write a sized buffer to the console
**
** @param buf  The buffer to write
** @param leng The number of bytes to write
**
** @return  The return value from calling write()
*/
int cwrite( const char *buf, uint32_t leng );

/**
** swritech(ch) - write a single character to the SIO
**
** @param ch The character to write
**
** @return  The return value from calling write()
*/
int swritech( char ch );

/**
** swrites(str) - write a NUL-terminated string to the SIO
**
** @param str The string to write
**
** @return  The return value from calling write()
*/
int swrites( const char *str );

/**
** swrite(buf,leng) - write a sized buffer to the SIO
**
** @param buf  The buffer to write
** @param leng The number of bytes to write
**
** @return  The return value from calling write()
*/
int swrite( const char *buf, uint32_t leng );

#endif