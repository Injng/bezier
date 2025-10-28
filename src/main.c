#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <string.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#define INIT_WIDTH 1080
#define INIT_HEIGHT 720
#define POINT_SIZE 10
#define RESOLVE_STEP 0.002
#define pse()                                                                  \
  printf("Error: %s", SDL_GetError());                                         \
  code = 1;                                                                    \
  goto cleanup;

typedef struct Point {
  float x;
  float y;
} Point;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
Point *control_pts = NULL;

bool draw_point(Point *pt, int size) {
  int length = size / 2;
  SDL_FRect rect = {
      .x = pt->x - length, .y = pt->y - length, .w = size, .h = size};
  return SDL_RenderFillRect(renderer, &rect);
}

Point interpolate(Point pt_a, Point pt_b, float t) {
  float diff_x = pt_b.x - pt_a.x;
  float diff_y = pt_b.y - pt_a.y;
  Point middle = {.x = pt_a.x + diff_x * t, .y = pt_a.y + diff_y * t};
  return middle;
}

Point resolve(Point *points, float t) {
  // if there is only one point left, just return that point
  if (arrlen(points) == 1) {
    Point return_pt = points[0];
    arrfree(points);
    return return_pt;
  }

  // otherwise, interpolate between each of the other points
  Point *next = NULL;
  for (int i = 0; i < arrlen(points) - 1; i++) {
    arrput(next, interpolate(points[i], points[i + 1], t));
  }

  Point result = resolve(next, t);
  arrfree(points);
  return result;
}

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

  // add default control points
  Point start = {.x = 100, .y = 100};
  Point middle = {.x = 300, .y = 900};
  Point end = {.x = 500, .y = 500};
  arrput(control_pts, start);
  arrput(control_pts, middle);
  arrput(control_pts, end);

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

    // set drawing color to black
    if (!SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255)) {
      pse();
    }

    // draw the control points
    for (int i = 0; i < arrlen(control_pts); i++) {
      if (!draw_point(&control_pts[i], POINT_SIZE)) {
        pse();
      }
    }

    // resolve the bezier curve
    for (float i = RESOLVE_STEP; i < 1; i += RESOLVE_STEP) {
      // make a copy of the control points
      Point *control_copy = NULL;
      for (int i = 0; i < arrlen(control_pts); i++) {
        arrput(control_copy, control_pts[i]);
      }
      Point pt = resolve(control_copy, i);
      if (!draw_point(&pt, POINT_SIZE)) {
        pse();
      }
    }

    // show the drawings by presenting the screen
    if (!SDL_RenderPresent(renderer)) {
      pse();
    }
  }

  // cleanup logic
cleanup:
  arrfree(control_pts);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return code;
}
