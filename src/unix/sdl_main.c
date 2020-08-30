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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

//
// sdl_main.c
// Main SDL module.
//
#include <SDL/SDL.h>

#include "../common/common.h"
#include "../common/cmd.h"
#include "x11_linux.h"
#include "unix_glimp.h"
#include "unix_local.h"
#include "sdl_main.h"

SDL_Surface *sdlwnd;

int p_mouse_x, p_mouse_y;

static qBool mouse_grabbed = qFalse;
static qBool keybd_grabbed = qFalse;
static qBool dgamouse = qFalse;
static int   grab_screenmode = -1;

static cVar_t *in_dgamouse;

cVar_t *in_mouse;
static cVar_t	*_windowed_mouse;

// Console variables that we need to access from this module
cVar_t		*vid_xpos;			// X coordinate of window position
cVar_t		*vid_ypos;			// Y coordinate of window position
cVar_t		*vid_fullscreen;    // Fullscreen or windowed.


//==================================================================//
//																	//
//		Mouse														//
//																	//
//==================================================================//
static int     mouse_buttonstate = 0;
static int     mouse_oldbuttonstate = 0;
static float old_windowed_mouse;
int xMove, yMove;
qBool isWindowGrabbed = qFalse;
qBool mouse_active = qFalse;
qBool	mouse_avail = qTrue;

void IN_MouseEvent (void)
{
	int		i;

	if (!mouse_avail)
		return;

// perform button actions
	for (i=0 ; i<5 ; i++)
	{
		if ( (mouse_buttonstate & (1<<i)) && !(mouse_oldbuttonstate & (1<<i)) )
			Key_Event (K_MOUSE1 + i, qTrue, Sys_Milliseconds());

		if ( !(mouse_buttonstate & (1<<i)) && (mouse_oldbuttonstate & (1<<i)) )
				Key_Event (K_MOUSE1 + i, qFalse, Sys_Milliseconds());
	}

	mouse_oldbuttonstate = mouse_buttonstate;
}


static void IN_DeactivateMouse( void )
{
	if (!mouse_avail)
		return;

	if (mouse_active) {
		/* uninstall_grabs(); */
		mouse_active = qFalse;
	}
}

static void IN_ActivateMouse( void )
{
	if (!mouse_avail)
		return;

	if (!mouse_active) {
		xMove = yMove = 0; // don't spazz
		_windowed_mouse = NULL;
		old_windowed_mouse = 0;
		mouse_active = qTrue;
	}

}

void IN_Activate(qBool active)
{
    /*	if (active || vidmode_active) */
    if (active)
		IN_ActivateMouse();
	else
		IN_DeactivateMouse();
}

//==================================================================//
//																	//
//		WINDOW														//
//																	//
//==================================================================//
qBool GLSDL_Init() {
    // Initialize SDL if not already initialized.
	if (SDL_WasInit(SDL_INIT_AUDIO|SDL_INIT_CDROM|SDL_INIT_VIDEO) == 0) {
		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			Sys_Error("SDL Init failed: %s\n", SDL_GetError());
			return qFalse;
		}
	} else if (SDL_WasInit(SDL_INIT_VIDEO) == 0) {
		if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) {
			Sys_Error("SDL Init failed: %s\n", SDL_GetError());
			return qFalse;
		}
	}

	// If the only thing that has changed is the fullscreen cvar, only adjust that.
	if (sdlwnd && (sdlwnd->w == ri.config.vidWidth) && (sdlwnd->h == ri.config.vidHeight)) {
		int isfullscreen = (sdlwnd->flags & SDL_FULLSCREEN) ? 1 : 0;
		if (vid_fullscreen->intVal != isfullscreen)
			SDL_WM_ToggleFullScreen(sdlwnd);

		isfullscreen = (sdlwnd->flags & SDL_FULLSCREEN) ? 1 : 0;
		if (vid_fullscreen->intVal == isfullscreen)
			return qTrue;
	}

    // Clear the window in case it exists.
    if (sdlwnd)
        SDL_FreeSurface(sdlwnd);

    return qTrue;
}

qBool GLSDL_CreateWindow(int width, int height, qBool fullscreen) {
    // Window flags.
    int windowFlags = SDL_OPENGL;

    if (sdlwnd)
        SDL_FreeSurface(sdlwnd);

    // Setup GL Attributes.
   	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    // TODO
	// if (gl_multisample->integer) {
	// 	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	// 	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, gl_multisample->integer);
	// } else {
	// 	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
	// }

	// SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, gl_swapinterval->integer);

    // Fullscreen?
   	if (vid_fullscreen->intVal)
		windowFlags |= SDL_FULLSCREEN;

    // TODO
	//SetSDLIcon(); /* currently uses q2icon.xbm data */
	if ((sdlwnd = SDL_SetVideoMode(ri.config.vidWidth, ri.config.vidHeight, 32, windowFlags)) == NULL) {
		Sys_Error("(SDLGL) SDL SetVideoMode failed: %s\n", SDL_GetError());
		return qFalse;
	}

	ri.config.vidWidth = sdlwnd->w;
	ri.config.vidHeight = sdlwnd->h;

	SDL_WM_SetCaption(APPLICATION, APPLICATION);

	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY,
			SDL_DEFAULT_REPEAT_INTERVAL);

	//SDL_ShowCursor(0);

    return qTrue;
}

void GLSDL_Shutdown() {
    if (sdlwnd)
		SDL_FreeSurface(sdlwnd);
	sdlwnd = NULL;

	if (SDL_WasInit(SDL_INIT_EVERYTHING) == SDL_INIT_VIDEO)
		SDL_Quit();
	else
		SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

void SDLGL_SwapBuffers() {
    SDL_GL_SwapBuffers();
}

//==================================================================//
//																	//
//		KEYBOARD													//
//																	//
//==================================================================//
struct
{
	int key;
	int down;
} keyq[64];
int keyq_head=0;
int keyq_tail=0;

static unsigned char KeyStates[SDLK_LAST];

static cVar_t	*_windowed_mouse;
void KBD_Close(void);

int XLateKey(unsigned int keysym)
{
	int key;
	char	buf[64];
	
	key = 0;
	switch(keysym) {
	case SDLK_KP9:			key = K_KP_PGUP; break;
	case SDLK_PAGEUP:		key = K_PGUP; break;

	case SDLK_KP3:			key = K_KP_PGDN; break;
	case SDLK_PAGEDOWN:		key = K_PGDN; break;

	case SDLK_KP7:			key = K_KP_HOME; break;
	case SDLK_HOME:			key = K_HOME; break;

	case SDLK_KP1:			key = K_KP_END; break;
	case SDLK_END:			key = K_END; break;

	case SDLK_KP4:			key = K_KP_LEFTARROW; break;
	case SDLK_LEFT:			key = K_LEFTARROW; break;

	case SDLK_KP6:			key = K_KP_RIGHTARROW; break;
	case SDLK_RIGHT:		key = K_RIGHTARROW; break;

	case SDLK_KP2:			key = K_KP_DOWNARROW; break;
	case SDLK_DOWN:			key = K_DOWNARROW; break;

	case SDLK_KP8:			key = K_KP_UPARROW; break;
	case SDLK_UP:			key = K_UPARROW; break;

	case SDLK_ESCAPE:		key = K_ESCAPE; break;

	case SDLK_KP_ENTER:		key = K_KP_ENTER; break;
	case SDLK_RETURN:		key = K_ENTER; break;

	case SDLK_TAB:			key = K_TAB; break;

	case SDLK_F1:			key = K_F1; break;
	case SDLK_F2:			key = K_F2; break;
	case SDLK_F3:			key = K_F3; break;
	case SDLK_F4:			key = K_F4; break;
	case SDLK_F5:			key = K_F5; break;
	case SDLK_F6:			key = K_F6; break;
	case SDLK_F7:			key = K_F7; break;
	case SDLK_F8:			key = K_F8; break;
	case SDLK_F9:			key = K_F9; break;
	case SDLK_F10:			key = K_F10; break;
	case SDLK_F11:			key = K_F11; break;
	case SDLK_F12:			key = K_F12; break;

	case SDLK_BACKSPACE:	key = K_BACKSPACE; break;

	case SDLK_KP_PERIOD:	key = K_KP_DEL; break;
	case SDLK_DELETE:		key = K_DEL; break;

	case SDLK_PAUSE:		key = K_PAUSE; break;

	case SDLK_LSHIFT:
	case SDLK_RSHIFT:		key = K_SHIFT; break;

	case SDLK_LCTRL:
	case SDLK_RCTRL:		key = K_CTRL; break;

	case SDLK_LMETA:
	case SDLK_RMETA:
	case SDLK_LALT:
	case SDLK_RALT:			key = K_ALT; break;

//	case SDLK_KP5:			key = K_KP_5; break;

	case SDLK_INSERT:		key = K_INS; break;
	case SDLK_KP0:			key = K_KP_INS; break;

	case SDLK_KP_MULTIPLY:	key = '*'; break;
	case SDLK_KP_PLUS:		key = K_KP_PLUS; break;
	case SDLK_KP_MINUS:		key = K_KP_MINUS; break;
	case SDLK_KP_DIVIDE:	key = K_KP_SLASH; break;

	/* suggestions on how to handle this better would be appreciated */
	case SDLK_WORLD_7:		key = '`'; break;

	default: /* assuming that the other sdl keys are mapped to ascii */
		if (keysym < 128)
		{
			key = keysym;

		if (key >= 'A' && key<= 'Z')
			key |= 32;
		if (key > 26)
			return key;
		}
		break;
	}

	return key;
}

static unsigned char KeyStates[SDLK_LAST];

void SDLGL_HandleEvents(void)
{
	SDL_Event event;
	int buttonStates;

	// Reset xMove and yMove
	xMove = yMove = 0;

	// PollEvents.
    while (SDL_PollEvent(&event)) {
		switch(event.type) {
			case SDL_ACTIVEEVENT: {
				if ( event.active.state & SDL_APPACTIVE ) {
					if ( event.active.gain ) {
						IN_Activate(qTrue);
					} else {
						IN_Activate(qFalse);
					}
				}
			}
			case SDL_MOUSEBUTTONDOWN:
				if (event.button.button == 4) {
					keyq[keyq_head].key = K_MWHEELUP;
					keyq[keyq_head].down = qTrue;
					keyq_head = (keyq_head + 1) & 63;
					keyq[keyq_head].key = K_MWHEELUP;
					keyq[keyq_head].down = qFalse;
					keyq_head = (keyq_head + 1) & 63;
				} else if (event.button.button == 5) {
					keyq[keyq_head].key = K_MWHEELDOWN;
					keyq[keyq_head].down = qTrue;
					keyq_head = (keyq_head + 1) & 63;
					keyq[keyq_head].key = K_MWHEELDOWN;
					keyq[keyq_head].down = qFalse;
					keyq_head = (keyq_head + 1) & 63;
				}
				break;
			case SDL_MOUSEBUTTONUP:
				break;
			case SDL_MOUSEMOTION:
					xMove = event.motion.xrel;
					yMove = event.motion.yrel;
				break;
			case SDL_KEYDOWN:
				if ( (KeyStates[SDLK_LALT] || KeyStates[SDLK_RALT]) &&
					(event.key.keysym.sym == SDLK_RETURN) ) {
					cVar_t	*_fullscreen;

					SDL_WM_ToggleFullScreen(sdlwnd);

					if (sdlwnd->flags & SDL_FULLSCREEN) {
						Cvar_SetValue( "vid_fullscreen", 1, qFalse );
					} else {
						Cvar_SetValue( "vid_fullscreen", 0, qFalse );
					}

					_fullscreen = Cvar_Exists( "vid_fullscreen" );
					_fullscreen->modified = qFalse;	// we just changed it with SDL.

					break; /* ignore this key */
				}

				if ( (KeyStates[SDLK_LCTRL] || KeyStates[SDLK_RCTRL]) &&
					(event.key.keysym.sym == SDLK_g) ) {
					SDL_GrabMode gm = SDL_WM_GrabInput(SDL_GRAB_QUERY);
					/*	
					SDL_WM_GrabInput((gm == SDL_GRAB_ON) ? SDL_GRAB_OFF : SDL_GRAB_ON);
					gm = SDL_WM_GrabInput(SDL_GRAB_QUERY);
					*/
					Cvar_SetValue( "_windowed_mouse", (gm == SDL_GRAB_ON) ? /*1*/ 0 : /*0*/ 1, qFalse );

					break; /* ignore this key */
				}

				KeyStates[event.key.keysym.sym] = 1;

				int key = XLateKey(event.key.keysym.sym);
				if (key) {
					keyq[keyq_head].key = key;
					keyq[keyq_head].down = qTrue;
					keyq_head = (keyq_head + 1) & 63;
				}
				break;
			case SDL_KEYUP:
				if (KeyStates[event.key.keysym.sym]) {
					KeyStates[event.key.keysym.sym] = 0;

					key = XLateKey(event.key.keysym.sym);
					if (key) {
						keyq[keyq_head].key = key;
						keyq[keyq_head].down = qFalse;
						keyq_head = (keyq_head + 1) & 63;
					}
				}
				break;
			case SDL_QUIT:
				Cbuf_AddText("quit");
				Cbuf_Execute();
				break;
		}	
	
	}
	
	// Fetch mouse delta motion.
   	//SDL_GetRelativeMouseState(&xMove, &yMove);

	// Fetch mouse button states.
    mouse_buttonstate = 0;
    buttonStates = SDL_GetMouseState(NULL, NULL);
    if (SDL_BUTTON(1) & buttonStates)
        mouse_buttonstate |= (1 << 0);
    if (SDL_BUTTON(3) & buttonStates) /* quake2 has the right button be mouse2 */
        mouse_buttonstate |= (1 << 1);
    if (SDL_BUTTON(2) & buttonStates) /* quake2 has the middle button be mouse3 */
        mouse_buttonstate |= (1 << 2);
    if (SDL_BUTTON(6) & buttonStates)
        mouse_buttonstate |= (1 << 3);
    if (SDL_BUTTON(7) & buttonStates)
        mouse_buttonstate |= (1 << 4);

	// Determine windowed mouse.
    if(!_windowed_mouse)
        _windowed_mouse = Cvar_Register("_windowed_mouse", "1", CVAR_ARCHIVE);

    if (old_windowed_mouse != _windowed_mouse->intVal) {
        old_windowed_mouse = _windowed_mouse->intVal;

        if (!_windowed_mouse->intVal) {
            /* ungrab the pointer */
			isWindowGrabbed = qFalse;
            SDL_WM_GrabInput(SDL_GRAB_OFF);
        } else {
			isWindowGrabbed = qTrue;
            /* grab the pointer */
            SDL_WM_GrabInput(SDL_GRAB_OFF);
        }
    }

	// Execute mouse events.
    IN_MouseEvent();

	// Loop and execute all key events.
    while (keyq_head != keyq_tail)
    {
        Key_Event (keyq[keyq_tail].key, keyq[keyq_tail].down, Sys_Milliseconds());
        keyq_tail = (keyq_tail + 1) & 63;
    }

	// Move the client mouse.
	CL_MoveMouse(xMove, yMove);

	// Warp mouse back to center.
	if (isWindowGrabbed) {// && (xMove != 0 || yMove != 0)) {
		SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
		//SDL_WarpMouse(ri.config.vidWidth / 2, ri.config.vidHeight / 2);
		SDL_EventState(SDL_MOUSEMOTION, SDL_ENABLE);
	}
}

void KBD_Close(void)
{
	keyq_head = 0;
	keyq_tail = 0;

	memset(keyq, 0, sizeof(keyq));
}

/*
=============================================================================

	INPUT

=============================================================================
*/

/*
=============
Force_CenterView_f
=============
*/
void Force_CenterView_f (void)
{
	cl.viewAngles[PITCH] = 0;
}


/*
=============
IN_Init
=============
*/
void IN_Init (void)
{
	// Compatibility variables
	in_mouse	= Cvar_Register ("in_mouse", "1", CVAR_ARCHIVE);

	// Mouse variables
	in_dgamouse	= Cvar_Register ("in_dgamouse", "1", CVAR_ARCHIVE);

	Cmd_AddCommand ("force_centerview", Force_CenterView_f, "Force the screen to a center view");
}


/*
=============
IN_Shutdown
=============
*/
void IN_Shutdown (void)
{
	
}


/*
=============
IN_Commands
=============
*/
void IN_Commands (void)
{
	SDLGL_HandleEvents ();
}


/*
=============
IN_Move
=============
*/
void IN_Move (userCmd_t *cmd)
{

}


/*
=============
IN_Frame
=============
*/
void IN_Frame (void)
{
    if (!sdlwnd)
        return;

	// Ungrab the mouse from window in case we are in the console or in a menu.
	if (Key_GetDest() == KD_CONSOLE || Key_GetDest() == KD_MENU) {
		isWindowGrabbed = qFalse;	
		SDL_WM_GrabInput(SDL_GRAB_OFF);
	} else {
		isWindowGrabbed = qTrue;
		SDL_WM_GrabInput(SDL_GRAB_OFF);
	}
}