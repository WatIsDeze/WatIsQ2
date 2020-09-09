/*
Copyright (C) 1997-2001 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

//
// rb_gl.c
//
// This file implements the operating system binding of GL to QGL function pointers.
//
// QGL_Init () - loads libraries, assigns function pointers, etc.
// QGL_Shutdown () - unloads libraries, NULLs function pointers
// QGL_ToggleDebug () - toggles glGetError calls after every gl function call
// QGL_ToggleLogging () - toggles logging of gl function calls
// QGL_LogBeginFrame () - Prints to the log file the beginning of a frame
// QGL_LogEndFrame () - Prints to the log file the end of a frame
// QGL_GetProcAddress () - returns the address of a gl function
//

#ifdef _WIN32

# include "../renderer/r_local.h"
# include "../client/sdl_glimp.h"

#elif defined __unix__

# include <dlfcn.h>
//# include <GL/gl.h>
//# include <GL/glx.h>
# include "../renderer/r_local.h"
# include "../client/sdl_glimp.h"

#endif

/*
===========
QGL_Shutdown

Unloads the specified DLL then nulls out all the proc pointers.
===========
*/
void QGL_Shutdown (void)
{

}


/*
===========
QGL_Init

This is responsible for binding our gl function pointers to the appropriate GL stuff.
In Windows this means doing a LoadLibrary and a bunch of calls to GetProcAddress. On other
operating systems we need to do the right thing, whatever that might be.
===========
*/
qBool QGL_Init (const char *dllName)
{

	return true;
}


/*
===========
QGL_ToggleDebug
===========
*/
void QGL_ToggleDebug (void)
{

}


/*
===========
QGL_ToggleLogging
===========
*/
void QGL_ToggleLogging (void)
{	

}


/*
===========
QGL_LogBeginFrame
===========
*/
void QGL_LogBeginFrame (void)
{

}


/*
===========
QGL_LogEndFrame
===========
*/
void QGL_LogEndFrame (void)
{

}


/*
===========
QGL_GetProcAddress
===========
*/
void *QGL_GetProcAddress (const char *procName)
{
	return NULL;
}
