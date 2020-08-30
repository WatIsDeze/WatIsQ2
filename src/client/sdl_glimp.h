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
// sdl_glimp.h
//

#ifndef __SDL_GLIMP_H__
#define __SDL_GLIMP_H__

#define APPLICATION "WatIsQ2 - [" WATISQ2_VERSTR "]"

typedef struct glState_s {
	qBool		active;

	void		*OpenGLLib;		// OpenGL library handle

	FILE		*oglLogFP;		// for gl_log logging
} glState_t;

extern glState_t	glState;

#endif	// __SDL_GLIMP_H__
