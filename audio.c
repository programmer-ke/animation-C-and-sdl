#include <SDL/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "audio.h"

/*array for all active sound effects*/
#define MAX_PLAYING_SOUNDS 10
playing_t playing[MAX_PLAYING_SOUNDS];

#define VOLUME_PER_SOUND SDL_MIX_MAXVOLUME

/*more samples callback function*/
void AudioCallback(void *userdata, Uint8 *audio, int length)
{
  int i;
  /*clear audio buffer*/
  memset(audio, 0, length);

  /*mix in each sound*/
  for (i = 0; i < MAX_PLAYING_SOUNDS; i++) {
    if (playing[i].active) {
      Uint8 *sound_buf;
      Uint32 sound_len;

      /*locate this sound's current buffer position*/
      sound_buf = playing[i].sound->samples;
      sound_buf += playing[i].position;

      //Determine the number of samples to mix
      if ((playing[i].position + length) > playing[i].sound->length) {
	sound_len = playing[i].sound->length - playing[i].position;
      } else {
	sound_len = length;
      }

      //mix this sound into the stream
      SDL_MixAudio(audio, sound_buf, sound_len, VOLUME_PER_SOUND);
      
      //Update the sounds buffer position
      playing[i].position += length;
      
      //Check if end of sound is reached
      if (playing[i].position >= playing[i].sound->length) {
	playing[i].active = 0;
      }
    }
  }
}

/*load and convert sound*/
int LoadAndConvertSound(char *filename, SDL_AudioSpec *spec, sound_p sound)
{
  SDL_AudioCVT cvt;
  SDL_AudioSpec loaded;
  Uint8 *new_buf;

  //load the WAV in its original sample format
  if (SDL_LoadWAV(filename, &loaded, &sound->samples, &sound->length) == NULL) {
    printf("Unable to load sound: %s\n", SDL_GetError());
    return 1;
  }

  //build a conversion structure
  if (SDL_BuildAudioCVT(&cvt, loaded.format, loaded.channels, loaded.freq, spec->format, spec->channels, spec->freq) < 0) {
    printf("Unable to convert sound: %s\n", SDL_GetError());
    return 1;
  }

  //allocate new buffer for converted data
  cvt.len = sound->length;
  new_buf = (Uint8 *)malloc(cvt.len * cvt.len_mult);
  if (new_buf == NULL) {
    printf("Memory allocation failed.\n");
    SDL_FreeWAV(sound->samples);
    return 1;
  }

  //copy the sound samples into new buffer
  memcpy(new_buf, sound->samples, sound->length);

  //perform conversion on new buffer
  cvt.buf = new_buf;
  if (SDL_ConvertAudio(&cvt) < 0) {
    printf("Audio conversion error: %s\n", SDL_GetError());
    free(new_buf);
    SDL_FreeWAV(sound->samples);
    return 1;
  }

  //swap converted data for original
  SDL_FreeWAV(sound->samples);
  sound->samples = new_buf;
  sound->length = sound->length * cvt.len_mult;

  printf("'%s' was loaded and converted successfully.\n", filename);
  return 0;
}


//Remove all currently playing songs
void ClearPlayingSounds(void)
{
  int i;
  for (i=0; i<MAX_PLAYING_SOUNDS; i++) {
    playing[i].active = 0;
  }
}

//Add sound to list of playing sounds
int PlaySound(sound_p sound)
{
  int i;
  //find empty slot
  for (i=0; i<MAX_PLAYING_SOUNDS; i++) {
    if (playing[i].active == 0) break;
  }

  //report if no slot
  if (i == MAX_PLAYING_SOUNDS)
    return 1;

  //obtain lock for 'playing' structure
  SDL_LockAudio();
  playing[i].active = 1;
  playing[i].sound = sound;
  playing[i].position = 0;
  SDL_UnlockAudio();

  return 0;
}

