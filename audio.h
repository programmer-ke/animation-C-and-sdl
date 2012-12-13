#include <stdio.h>
#include <SDL/SDL.h>

/*structure for loaded sounds*/
typedef struct sound_s {
  Uint8 *samples;
  Uint32 length;
} sound_t, *sound_p;

/*structure for currently playing sounds*/
typedef struct playing_s {
  int active;
  sound_p sound;
  Uint32 position;
} playing_t, *playing_p;

void AudioCallback(void *, Uint8 *, int );
int LoadAndConvertSound(char *, SDL_AudioSpec *, sound_p );
void ClearPlayingSounds(void);
int PlaySound(sound_p);
