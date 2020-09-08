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
// rb_gl.h
//

#ifndef __RB_GL_H__
#define __RB_GL_H__

#ifdef _WIN32
# include <windows.h>
#endif

#if defined(__unix__)
# include <GL/glx.h>
#endif

#ifdef _WIN32
//typedef int GLsizeiptrARB;
//typedef int GLintptrARB;
#endif

//
// functions
//

qBool	QGL_Init (const char *dllName);
void	QGL_Shutdown (void);

void	QGL_ToggleDebug (void);

void	QGL_ToggleLogging (void);
void	QGL_LogBeginFrame (void);
void	QGL_LogEndFrame (void);

void	*QGL_GetProcAddress (const char *procName);

#endif // __RB_GL_H__
