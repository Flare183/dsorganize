/**
 * @ingroup   file68_debug68_devel
 * @file      file68/debugmsg68.h
 * @author    Benjamin Gerard <ben@sashipa.com>
 * @date      2003/08/22
 * @brief     debug message header.
 *
 * $Id: debugmsg68.h 503 2005-06-24 08:52:56Z loke $
 *
 */

/* Copyright (C) 1998-2003 Benjamin Gerard */

#ifndef _DEBUGMSG68_H_
#define _DEBUGMSG68_H_

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup  file68_debug68_devel  Debug messages
 *  @ingroup   file68_devel
 *
 *    Provides functions for debugging message.
 *
 *  @{
 */

/** Debug handler type. */
typedef void (*debugmsg68_t)(void * cookie, const char * fmt, va_list list);

/** Set debug function.
 *
 *  @param  handler  pointer to a function to display debug messages
 *                   (0 desactive debug message).
 *  @return previous handler
 */
debugmsg68_t debugmsg68_set_handler(debugmsg68_t handler);

/** Set debug function user parameter.
 *
 *  @param  cookie  user parameter that will be sent as first parameter to
 *                  the debug handler.
 *
 *  @see debugmsg68_set_handler();
 */
void * debugmsg68_set_cookie(void * cookie);

/** Print debug message.
 *
 *  @param fmt  printf() like format string.
 *
 */
void debugmsg68(const char * fmt, ...);

/** Print debug message (variable argument).
 *
 *  @param fmt  printf() like format string.
 *  @param list variable argument list (stdarg.h)
 *
 */
void vdebugmsg68(const char * fmt, va_list list);

/** Wrapper for debugmsg68().
 *
 *  @param fmt  printf() like format string.
 *
 *  @deprecated Use debugmsg68() instead.
 */
void SC68os_pdebug(const char *fmt, ...);

/**
 *@}
 */

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _DEBUGMSG68_H_ */
