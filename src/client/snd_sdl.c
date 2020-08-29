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
// snd_sdl.c 
//

#include <SDL/SDL.h>
#include "snd_local.h"

static qBool s_inited = qFalse;

cVar_t	*s_bits;
cVar_t	*s_speed;
cVar_t	*s_channels;
cVar_t	*s_system;

void Snd_Memset (void* dest, const int val, const size_t count)
{
	memset(dest,val,count);
}

static void sdl_audio_callback (void *unused, Uint8 * stream, int len)
{
	if (s_inited) {
		snd_audioDMA.buffer = stream;
		snd_audioDMA.samplePos += len / (snd_audioDMA.sampleBits / 4);
		// Check for samplepos overflow?
		DMASnd_PaintChannels (snd_audioDMA.samplePos);
	}
}

/*
==============
SndImp_Init
===============
*/
qBool SndImp_Init (void)
{
	SDL_AudioSpec desired, obtained;
	
	if(s_inited)
		return qTrue;

	if (!s_bits) {
		s_bits = Cvar_Register("sndbits", "16", CVAR_ARCHIVE);
		s_speed = Cvar_Register("sndspeed", "0", CVAR_ARCHIVE);
		s_channels = Cvar_Register("sndchannels", "2", CVAR_ARCHIVE);
		//snddevice = Cvar_Get("snddevice", "/dev/dsp", CVAR_ARCHIVE);
	}

	if (!SDL_WasInit(SDL_INIT_EVERYTHING)) {
		if (SDL_Init(SDL_INIT_AUDIO) < 0) {
			Com_Printf(PRNT_ERROR, "Couldn't init SDL audio: %s\n", SDL_GetError ());
			return qFalse;
		}
	} else if (!SDL_WasInit(SDL_INIT_AUDIO)) {
		if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
			Com_Printf(PRNT_ERROR, "Couldn't init SDL audio: %s\n", SDL_GetError ());
			return qFalse;
		}
	}
	

	/* Set up the desired format */
	switch (s_bits->intVal) {
		case 8:
			desired.format = AUDIO_U8;
			break;
		default:
			Com_Printf(PRNT_ERROR, "Unknown number of audio bits: %i, trying with 16\n", s_bits->intVal);
		case 16:
			desired.format = AUDIO_S16SYS;
			break;
	}
	
	if(s_speed->intVal)
		desired.freq = s_speed->intVal;
	else
		desired.freq = 22050;

    // just pick a sane default.
    if (desired.freq <= 11025)
        desired.samples = 256;
    else if (desired.freq <= 22050)
        desired.samples = 512;
    else if (desired.freq <= 44100)
        desired.samples = 1024;
    else
        desired.samples = 2048;  // (*shrug*)
	
	desired.channels = s_channels->intVal;
	if (desired.channels < 1 || desired.channels > 2)
		desired.channels = 2;

	desired.callback = sdl_audio_callback;
	
	/* Open the audio device */
	if (SDL_OpenAudio (&desired, &obtained) < 0) {
		Com_Printf(PRNT_ERROR, "SDL_OpenAudio() failed: %s\n", SDL_GetError ());
		if (SDL_WasInit(SDL_INIT_EVERYTHING) == SDL_INIT_AUDIO)
			SDL_Quit();
		else
			SDL_QuitSubSystem(SDL_INIT_AUDIO);
		return qFalse;
	}

	/* Fill the audio DMA information block */
	snd_audioDMA.sampleBits = (obtained.format & 0xFF);
	snd_audioDMA.speed = obtained.freq;
	snd_audioDMA.channels = obtained.channels;
	snd_audioDMA.samples = obtained.samples * snd_audioDMA.channels;
	snd_audioDMA.samplePos = 0;
	snd_audioDMA.submissionChunk = 1;
	snd_audioDMA.buffer = NULL;

    Com_Printf(PRNT_ERROR, "Starting SDL audio callback...\n");
    SDL_PauseAudio(0);  // start callback.

    Com_Printf(PRNT_ERROR, "SDL audio initialized.\n");

	s_inited = qTrue;
	return qTrue;
}


/*
==============
SndImp_Shutdown
===============
*/
void SndImp_Shutdown (void)
{
	if (!s_inited)
		return;

	SDL_PauseAudio(1);
	SDL_CloseAudio ();
	s_inited = qFalse;

	if (SDL_WasInit(SDL_INIT_EVERYTHING) == SDL_INIT_AUDIO)
		SDL_Quit();
	else
		SDL_QuitSubSystem(SDL_INIT_AUDIO);
}


/*
==============
SndImp_GetDMAPos
===============
*/
int SndImp_GetDMAPos (void)
{
	if(!s_inited)
		return 0;

	return snd_audioDMA.samplePos;
}


/*
==============
SndImp_BeginPainting
===============
*/
void SndImp_BeginPainting (void)
{

}


/*
==============
SndImp_Submit

Send sound to device if buffer isn't really the DMA buffer
===============
*/
void SndImp_Submit (void)
{

}
