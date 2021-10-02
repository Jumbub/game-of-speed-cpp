#include "../board/generate.h"
#include "../board/next.h"
#include "../board/sdl.h"
#include <SDL2/SDL.h>
#include <benchmark/benchmark.h>

const int TEST_WIDTH = 2560;
const int TEST_HEIGHT = 1440;

static void BM_NextBoard(benchmark::State &state) {
  auto board = benchmarkBoard(TEST_WIDTH, TEST_HEIGHT);

  for (auto _ : state) {
    board = nextBoard(board);
  }
}

static void BM_RenderNextBoard(benchmark::State &state) {
  auto board = benchmarkBoard(TEST_WIDTH, TEST_HEIGHT);

  // Initialize graphics
  SDL_Init(SDL_INIT_VIDEO);
  SDL_Renderer *renderer;
  SDL_Window *window;
  SDL_CreateWindowAndRenderer(TEST_WIDTH, TEST_HEIGHT, 0, &window, &renderer);
  auto texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                    SDL_TEXTUREACCESS_STATIC, TEST_WIDTH, TEST_HEIGHT);

  for (auto _ : state) {
    board = nextBoard(board);
    renderBoardSdl(board, renderer, texture);
  }

  // Destroy graphics
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

BENCHMARK(BM_NextBoard)->Unit(benchmark::kMillisecond)->MinTime(5);
BENCHMARK(BM_RenderNextBoard)->Unit(benchmark::kMillisecond)->MinTime(5);

BENCHMARK_MAIN();
