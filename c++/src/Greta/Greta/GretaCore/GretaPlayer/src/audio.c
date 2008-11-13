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

#include "audio.h"

#include <stdio.h>
//#include <stdlib>

#ifdef UNIX //cambiare il nome qui per non fare casino
//usare le funzioni audio UNIX anche per windows e fare una nuova funzione
//load buffer che ne prende l'indirizzo e lo salva nella variabile wav_buffer che già c'è
//così quando fai play tutto già funziona

SDL_AudioSpec *desired_audio,*obtained_audio;
SDL_AudioSpec wav_spec;
Uint32 wav_length;
Uint8 *wav_buffer;
int prev_audio_pos,prev_time,prev_frame,prev_mill;

Uint32 audio_len;
Uint8 *audio_pos;

#include <windows.h>

#include<sys/stat.h>

int file_exists (char * fileName)
{
   struct stat buf;
   int i = stat ( fileName, &buf );
     /* File found */
     if ( i == 0 )
     {
       return 1;
     }
     return 0;
       
}

void audio_callback(void *userdata, Uint8 *stream, int len)
{
        /* Only play if we have data left */
        if ( audio_len <= 0 )
            return;

        /* Mix as much data as possible */
        len = ( len > audio_len ? audio_len : len );
        SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME);
        audio_pos += len;
        audio_len -= len;
}

int load_wave(char* filename)
{
	char buffer[256];

	SDL_Init(SDL_INIT_AUDIO);

	desired_audio=(SDL_AudioSpec *)malloc(sizeof(SDL_AudioSpec));
	obtained_audio=(SDL_AudioSpec *)malloc(sizeof(SDL_AudioSpec));
	
	sprintf(buffer,"%s",filename);
	
	desired_audio = SDL_LoadWAV(buffer, &wav_spec, &wav_buffer, &wav_length);
	if( desired_audio == NULL ){
		fprintf(stderr, "Could not open %s: %s\n", buffer, SDL_GetError());
		return 1;
	}

//	printf("%s loaded.\n", buffer);

	desired_audio->samples=1024; //deve essere una potenza di 2
	desired_audio->callback=audio_callback;

	if ( SDL_OpenAudio(desired_audio, obtained_audio) < 0 ){
	  printf("Couldn't open audio: %s\n", SDL_GetError());
	  return 0;
	}

//	free(desired_audio);

//	printf("SDL audio initialized.\n");

//	audio_len=wav_length-obtained_audio->samples;
//	audio_pos=wav_buffer+obtained_audio->samples;
	audio_len=wav_length;
	audio_pos=wav_buffer;

	atexit(SDL_Quit);
	atexit(SDL_CloseAudio);
	return 1;
}

int play_wave(int position)
{
	audio_len=wav_length-position*obtained_audio->freq*2*obtained_audio->channels/1000;

	audio_pos=wav_buffer+position*obtained_audio->freq*2*obtained_audio->channels/1000;
	SDL_PauseAudio(0);
	
	prev_audio_pos=-1;
	prev_time=-1;
	return 1;
}

int get_wave_time()
{
	int mill,sec;

	if(SDL_GetAudioStatus()==SDL_AUDIO_PAUSED)
		return prev_mill;

	if(prev_audio_pos!=(audio_pos-(obtained_audio->samples*4)-wav_buffer))
	{
		sec=(audio_pos-(obtained_audio->samples*4)-wav_buffer);
		mill=sec/((obtained_audio->freq*2*obtained_audio->channels)/1000);
		prev_audio_pos=(audio_pos-(obtained_audio->samples*4)-wav_buffer);
		prev_time=timeGetTime();
		prev_mill=mill;
	}else
	{
		mill=prev_mill+(timeGetTime()-prev_time);
	}
	return mill;
}

int get_audio_len(void)
{
	return audio_len;
}

void stop_wave()
{
	if(SDL_GetAudioStatus()==SDL_AUDIO_PLAYING)
		SDL_PauseAudio(1);
}

void close_wave()
{
	SDL_PauseAudio(0); // non dovrebbe essere 1?
	SDL_CloseAudio();
	SDL_FreeWAV(wav_buffer);
	SDL_Quit();
}
#else // WINDOWS
#include <windows.h>

char mci_command[128];
char mci_return[128];


//Cambiare questa funzione affinche prenda un buffer e non un nome di file
//guarda la funzione nella parte UNIX, li si usa il buffer

int load_wave(char* filename, char* alias)
{
	
	char buffer[1000];
	int num;
	DWORD fdwError;

	memset(buffer,' ',1000);

	if ( file_exists(filename)==0)  return 0;

	GetShortPathName(filename,buffer,strlen(buffer));

	sprintf(mci_command, "open %s type waveaudio alias %s", buffer, alias );
	printf("%s\n",mci_command);

	fdwError=mciSendString(mci_command, mci_return,128, NULL);
	if (fdwError)
	{ 
		mciGetErrorString(fdwError,buffer,&num);
		printf("ERROR: %s - %s\n", filename,buffer); 
		return 0;
	}
	return 1;
}

int play_wave(int position,char* alias)
{

	sprintf(mci_command, "play %s from %d", alias, position);
	printf("%s\n",mci_command);
	mciSendString(mci_command, mci_return, 128, NULL);

	return 1;
}

int get_wave_time(char* alias)
{
	sprintf(mci_command, "set %s time format to ms", alias); 
	mciSendString(mci_command, mci_return, 128, NULL);
	sprintf(mci_command, "status %s position", alias); 
	mciSendString(mci_command, mci_return, 128, NULL);
	return atoi(mci_return)+120/*//!!!!!!!!!!!!!!!!!!!*/;

}

int get_audio_len(char* alias)
{
	sprintf(mci_command, "set %s time format to ms", alias); 
	mciSendString(mci_command, mci_return, 128, NULL);
	sprintf(mci_command, "status %s length", alias); 
	mciSendString(mci_command, mci_return, 128, NULL);
	return atoi(mci_return)+120;
}

void stop_wave(char* alias)
{
//	memset(mci_return,0,128);
	sprintf(mci_command, "pause %s", alias); 
	mciSendString(mci_command, mci_return, 128, NULL);
	printf("stop audio\n");
}

void close_wave(char* alias)
{
//	memset(mci_return,0,128);
	sprintf(mci_command, "close %s", alias); 
	mciSendString(mci_command, mci_return, 128, NULL);
	printf("close audio\n");
}
#include<sys/stat.h>
int file_exists (char * fileName)
{
   struct stat buf;
   int i = stat ( fileName, &buf );
     /* File found */
     if ( i == 0 )
     {
       return 1;
     }
     return 0;
       
}
#endif // WINDOWS

