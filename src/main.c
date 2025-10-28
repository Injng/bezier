#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#define INIT_WIDTH 1080
#define INIT_HEIGHT 720
#define pse()                                                                  \
  printf("Error: %s", SDL_GetError());                                         \
  code = 1;                                                                    \
  goto cleanup;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

int main(void) {
  // code to return from the program with
  int code = 0;

  // initialize SDL with video subsystem
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    pse();
  }

  // create a resizable window
  window =
      SDL_CreateWindow("bezier", INIT_WIDTH, INIT_HEIGHT, SDL_WINDOW_RESIZABLE);
  if (window == NULL) {
    pse();
  }

  // create a renderer
  renderer = SDL_CreateRenderer(window, NULL);
  printf("%s", SDL_GetError());
  if (renderer == NULL) {
    pse();
  }

  // event loop with quit event state
  bool quit = false;
  while (!quit) {
    // handle events by repeatedly polling from event queue
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_EVENT_QUIT:
        quit = true;
        break;
      }
    }

    // set drawing color to white
    if (!SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255)) {
      pse();
    }

    // clear window background with drawing color
    if (!SDL_RenderClear(renderer)) {
      pse();
    }

    // show the drawings by presenting the screen
    if (!SDL_RenderPresent(renderer)) {
      pse();
    }
  }

  // cleanup logic
cleanup:
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return code;
}
