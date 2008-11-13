//Copyright 1999-2008 Catherine Pelachaud - c.pelachaud@iut.univ-paris8.fr
//
//This file is part of Greta.
//
//Greta is free software; you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation; either version 2 of the License, or
//(at your option) any later version.
//
//Greta is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with Greta; if not, write to the Free Software
//Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

/** audio library
*   NOTE: Unix/Linux compatibility not tested
*
* @see Agent
* @author Maurizio Mancini, manmau@yahoo.com
* @author Massimo Bilvi, bilvi@gsmbox.it
* @version 1.0
*/


#ifdef UNIX
#include "SDL\SDL.h"

extern SDL_AudioSpec *desired_audio,*obtained_audio;
extern SDL_AudioSpec wav_spec;
extern Uint32 wav_length;
extern Uint8 *wav_buffer;

extern Uint32 audio_len;

extern Uint8 *audio_pos;

extern int prev_audio_pos,prev_time,prev_frame,prev_mill;
#endif

//loads audio file and associates it with the given name
int load_wave(char*,char*);

//starts audio playback from the given time (in milliseconds)
int play_wave(int,char*);

//gets the actual audio playback time (in milliseconds)
int get_wave_time(char*);

int get_audio_len(char*);

//stops audio playback
void stop_wave(char*);

int file_exists(const char * filename);

//closes audio file
void close_wave(char*);

