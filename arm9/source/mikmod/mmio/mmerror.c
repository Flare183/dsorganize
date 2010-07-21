/*	MikMod sound library
	(c) 1998, 1999, 2000 Miodrag Vallat and others - see file AUTHORS for
	complete list.

	This library is free software; you can redistribute it and/or modify
	it under the terms of the GNU Library General Public License as
	published by the Free Software Foundation; either version 2 of
	the License, or (at your option) any later version.
 
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Library General Public License for more details.
 
	You should have received a copy of the GNU Library General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
	02111-1307, USA.
*/

/*==============================================================================

  $Id: mmerror.c,v 1.1.1.1 2004/01/21 01:36:35 raph Exp $

  Error handling functions.
  Register an error handler with _mm_RegisterErrorHandler() and you're all set.

==============================================================================*/

/*

	The global variables _mm_errno, and _mm_critical are set before the error
	handler in called.  See below for the values of these variables.

*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "..\include\mikmod_internals.h"

/* User installed error callback */
MikMod_handler_t _mm_errorhandler = NULL;
MIKMODAPI int  _mm_errno = 0;
MIKMODAPI BOOL _mm_critical = 0;

/* ex:set ts=4: */
