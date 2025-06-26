#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_log.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>

void _SDL_Error_Handler(const char *calling_function_name, SDL_Window *window, SDL_Renderer *renderer);
void Draw_Axis(SDL_Renderer *renderer, SDL_Window *window);

int main() {
  if (SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("SDL_Init error: %s", SDL_GetError());
    return 1;
  }

  SDL_Window *window = SDL_CreateWindow("Graph plotter", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 800, SDL_WINDOW_FULLSCREEN_DESKTOP);
  if (window == NULL) {
    _SDL_Error_Handler("SDL_CreateWindow", window, NULL);
  }

  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == NULL) {
    _SDL_Error_Handler("SDL_CreateRenderer", window, renderer);
  }

  int width, height;
  SDL_GetRendererOutputSize(renderer, &width, &height);

  SDL_Texture *axis_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
  if (axis_texture == NULL) {
    _SDL_Error_Handler("SDL_CreateTexture", window, renderer);
  }

  SDL_SetRenderTarget(renderer, axis_texture);
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderClear(renderer);
  Draw_Axis(renderer, window);
  SDL_SetRenderTarget(renderer, NULL);

  SDL_RenderCopy(renderer, axis_texture, NULL, NULL);
  SDL_RenderPresent(renderer);

  SDL_Event event;
  int running = 1;

  while (running) {
    while(SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
          running = 0;
          break;
      }
    }
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}

void _SDL_Error_Handler(const char *calling_function_name, SDL_Window *window, SDL_Renderer *renderer) {
  SDL_Log("%s error: %s", calling_function_name, SDL_GetError());

  if (renderer != NULL) {
    SDL_DestroyRenderer(renderer);
  }
  if (window != NULL) {
    SDL_DestroyWindow(window);
  }

  SDL_Quit();
  exit(1);
}

void Draw_Axis(SDL_Renderer *renderer, SDL_Window *window) {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

  if (SDL_RenderDrawLine(renderer, 80, 1000, 1840, 1000) != 0) {
    _SDL_Error_Handler("SDL_RenderDrawLine", window);
  }
  if (SDL_RenderDrawLine(renderer, 80, 80, 80, 1000) != 0) {
    _SDL_Error_Handler("SDL_RenderDrawLine", window);
  }
}
