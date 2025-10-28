#include <SDL3/SDL_mouse.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

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

int is_control(float click_x, float click_y) {
  int length = POINT_SIZE / 2;
  for (int i = 0; i < arrlen(control_pts); i++) {
    if (fabsf(click_x - control_pts[i].x) <= length &&
        fabsf(click_y - control_pts[i].y) <= length) {
      return i;
    }
  }
  return -1;
}

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
  int moving_point = -1;
  while (!quit) {
    // handle events by repeatedly polling from event queue
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_EVENT_QUIT:
        quit = true;
        break;
      case SDL_EVENT_MOUSE_BUTTON_DOWN:
        if (event.button.button == SDL_BUTTON_LEFT) {
          moving_point = is_control(event.button.x, event.button.y);
        } else if (event.button.button == SDL_BUTTON_RIGHT) {
          Point new = {.x = event.button.x, .y = event.button.y};
          arrins(control_pts, arrlen(control_pts) - 2, new);
        }
        break;
      case SDL_EVENT_MOUSE_BUTTON_UP:
        moving_point = -1;
        break;
      }
    }

    // move control points smoothly if they should be moved
    if (moving_point > -1) {
      float mouse_x;
      float mouse_y;
      SDL_GetMouseState(&mouse_x, &mouse_y);
      Point new = {.x = mouse_x, .y = mouse_y};
      control_pts[moving_point] = new;
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
