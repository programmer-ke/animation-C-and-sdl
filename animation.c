#include <stdio.h>
#include <SDL/SDL.h>
#include <stdlib.h>
#include "audio.h"

struct obj {
  int x, y;
  int dx, dy;
};

typedef struct obj bus_obj;
typedef struct obj man_obj;

bus_obj psv, priv1, priv2;
man_obj passenger;

static SDL_Surface* screen;
static SDL_Surface* bus;
static SDL_Surface* man;
static SDL_Surface* man2;
static SDL_Surface* car1;
static SDL_Surface* car2;

void init_bus()
{
  psv.y  = 240;
  psv.x = 639;
  psv.dx = -6;
  return;
}

void init_man()
{
  passenger.y = 250;
  passenger.x = 0;
  passenger.dx = 3;
  return;
}

void init_cars()
{
  priv1.x = priv2.x = screen->w/2;
  priv1.y =  psv.y + 3;
  priv2.y = psv.y + 5;
  priv1.dx = -7; priv2.dx = 7;
}

void move_obj(struct obj* obj) 
{
  obj->x += obj->dx;
  return;
}

void draw_obj(struct obj* obj, SDL_Surface* surf)
{
  SDL_Rect src, dest;
  src.x = 0;
  src.y = 0;
  src.h = surf->h;
  src.w = surf->w;

  dest.x = obj->x - surf->w/2;
  dest.y = obj->y - surf->h/2;
  dest.w = surf->w;
  dest.h = surf->h;
  SDL_BlitSurface(surf, &src, screen, &dest);
  return;
}

void draw_man()
{
  SDL_Rect src, dest;
  src.x = 0;
  src.y = 0;
  src.h = man->h;
  src.w = man->w;

  dest.x = passenger.x - man->w/2;
  dest.y = passenger.y - man->h/2;
  dest.w = man->w;
  dest.h = man->h;
  if (1) {
    if (passenger.x % 12 == 9)
      SDL_BlitSurface(man2, &src, screen, &dest);
    else 
      SDL_BlitSurface(man, &src, screen, &dest);
  }
  return;
}

int main()
{
  SDL_Surface* background, *background2, *background3, *background4;
  SDL_Rect src, dest;
  int frames;  
  Uint32 colorkey;


  SDL_AudioSpec desired, obtained;
  sound_t bus_sound;

  /*initialize video and sound subsystem*/
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0){
    printf("Unable to initialize video: %s\n.", SDL_GetError());
    return 1;
  }

  /*ensure all subsystems exit safely*/
  atexit(SDL_Quit);
  atexit(SDL_CloseAudio);

  /*set video mode*/
  screen = SDL_SetVideoMode(640, 480, 16, SDL_DOUBLEBUF | SDL_HWSURFACE);
  if (screen == NULL) {
    printf("Unable to set video mode: %s\n", SDL_GetError());
    return 1;
  }

  /*load background images*/
  background = SDL_LoadBMP("src.bmp");
  if (background == NULL) {
    printf("Unable to load image.");
    return 1;
  }

  background2 = SDL_LoadBMP("background2.bmp");
  if (background2 == NULL) {
    printf("Unable to load background2.");
    return 1;
  }

  background3 = SDL_LoadBMP("background3.bmp");
  if (background3 == NULL) {
    printf("Unable to load background3.");
    return 1;
  }

  background4 = SDL_LoadBMP("background4.bmp");
  if (background4 == NULL) {
    printf("Unable to load background4.");
    return 1;
  }

  /*load bus*/
  bus = SDL_LoadBMP("bus.bmp");
  if (bus == NULL) {
    printf("Unable to load image.");
    return 1;
  }
  colorkey = SDL_MapRGB(bus->format, 255, 255, 255);

  /*set color key*/
  SDL_SetColorKey(bus,
		  SDL_SRCCOLORKEY,
		  colorkey);
  
  /*load man*/
  man = SDL_LoadBMP("man.bmp");
  if (man == NULL) {
    printf("Unable to load image");
    return 1;
  }

  colorkey = SDL_MapRGB(man->format, 255, 255, 255);

  /*set color key*/
  SDL_SetColorKey(man,
		  SDL_SRCCOLORKEY,
		  colorkey);

  man2 = SDL_LoadBMP("man2.bmp");
  if (man2 == NULL) {
    printf("Unable to load image");
    return 1;
  }

  colorkey = SDL_MapRGB(man2->format, 255, 255, 255);

  /*set color key*/
  SDL_SetColorKey(man2,
		  SDL_SRCCOLORKEY,
		  colorkey);

  car1 = SDL_LoadBMP("car1.bmp");
  if (car1 == NULL) {
    printf("Unable to load image");
    return 1;
  }

  colorkey = SDL_MapRGB(car1->format, 255, 255, 255);

  /*set color key*/
  SDL_SetColorKey(car1,
		  SDL_SRCCOLORKEY,
		  colorkey);


  car2 = SDL_LoadBMP("car2.bmp");
  if (car2 == NULL) {
    printf("Unable to load image");
    return 1;
  }

  colorkey = SDL_MapRGB(car2->format, 255, 255, 255);

  /*set color key*/
  SDL_SetColorKey(car2,
		  SDL_SRCCOLORKEY,
		  colorkey);

  
  //open audio device
  desired.freq = 44100;
  desired.format = AUDIO_S16;
  desired.samples = 4096;
  desired.channels = 2;
  desired.callback = AudioCallback;
  desired.userdata = NULL;

  if (SDL_OpenAudio(&desired, &obtained) < 0) {
    printf("Unable to open audio device: %s\n", SDL_GetError());
    return 1;
  }

  //load sound files and convert them to sound card's format
  if (LoadAndConvertSound("bus-pass.wav", &obtained, &bus_sound) != 0) {
    printf("Unable to load sound.\n");
    return 1;
  }

  ClearPlayingSounds();
  SDL_PauseAudio(0);

  init_bus();
  init_man();
  
  int passenger_in = 0;

  PlaySound(&bus_sound);
  while (psv.x > 0) {
    src.x = 0;
    src.y = 0;
    src.w = background->w;
    src.h = background->h; 
    dest = src;
    SDL_BlitSurface(background, &src, screen, &dest);

    draw_obj(&psv, bus);
    
    if (psv.x < screen->w/2 && !passenger_in) { /*pause bus for passenger to enter*/
      SDL_PauseAudio(1);
      if (passenger.x > psv.x + 40){  /*check if passenger has got in*/
	passenger_in = 1; SDL_PauseAudio(0);
      }

      draw_man();
      SDL_Flip(screen);
      move_obj(&passenger);
    } else {
      SDL_Flip(screen);
      move_obj(&psv);
    }
  }

  psv.x = 639;  psv.y = 320;
  init_cars();
  PlaySound(&bus_sound);
  while (psv.x + bus->w/2 > 0) {
    SDL_BlitSurface(background2, &src, screen, &dest);
    draw_obj(&priv1, car1); draw_obj(&priv2, car2);
    draw_obj(&psv, bus);
    SDL_Flip(screen);
    move_obj(&psv);
    move_obj(&priv1); move_obj(&priv2);
  }

  psv.x = 639; psv.y = 350;
  PlaySound(&bus_sound);
  while (psv.x + bus->w/2 > 0) {
    SDL_BlitSurface(background3, &src, screen, &dest);
    draw_obj(&psv, bus);
    SDL_Flip(screen);
    move_obj(&psv);
  }

  psv.x = 639; psv.y = 267; passenger.y = 270;
  passenger_in = 1;
  int has_paused = 0;
  PlaySound(&bus_sound);
  while (psv.x + bus->w/2 > 0) {
    SDL_BlitSurface(background4, &src, screen, &dest);
    if (screen->w/2 > psv.x && passenger_in == 1) {
      SDL_PauseAudio(1);
      if (has_paused == 0) { SDL_Delay(1000); has_paused = 1;}
      if (passenger.x > 639) {passenger_in = 0; SDL_PauseAudio(0);}
      draw_obj(&psv, bus);
      draw_man();
      SDL_Flip(screen);
      move_obj(&passenger);
    } else {
      draw_obj(&psv, bus);
      SDL_Flip(screen);
      move_obj(&psv);
    }
  }

  //pause and lock sound system
  SDL_PauseAudio(1);
  SDL_LockAudio();

  free(bus_sound.samples);

  SDL_UnlockAudio();


  SDL_FreeSurface(background4);
  SDL_FreeSurface(background3);
  SDL_FreeSurface(background2);
  SDL_FreeSurface(background);
  SDL_FreeSurface(man);
  SDL_FreeSurface(bus);
  return 0;
}
