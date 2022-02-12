#include <future>
#include <iostream>
#include <thread>
#include "../../build/_deps/imgui-sfml-src/imgui-SFML.h"
#include "../../build/_deps/imgui-src/imgui.h"
#include "../../build/_deps/sfml-src/include/SFML/Graphics.hpp"
#include "../graphics/gui.h"
#include "../graphics/render.h"
#include "../logic/generate.h"
#include "../logic/next.h"
#include "../util/lock.h"
#include "../util/time.h"

using namespace std::chrono;

constexpr uint INITIAL_WINDOW_WIDTH = 2560;
constexpr uint INITIAL_WINDOW_HEIGHT = 1440;
constexpr uint INITIAL_IMAGE_WIDTH = INITIAL_WINDOW_WIDTH + 2;
constexpr uint INITIAL_IMAGE_HEIGHT = INITIAL_WINDOW_HEIGHT + 2;

struct Loop {
  sf::RenderWindow window;
  sf::Texture texture;
  sf::Sprite sprite;
  sf::Image image;
  sf::Uint32* pixels = new sf::Uint32[INITIAL_IMAGE_WIDTH * INITIAL_IMAGE_HEIGHT];
  Board board = Board(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT);

  Loop(bool noResize = false)
      : window(sf::RenderWindow(
            sf::VideoMode(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT),
            "Game of Speed",
            noResize ? sf::Style::None : sf::Style::Resize)) {
    // Init board
    assignBenchmarkCells(board);

    // Init graphics
    ImGui::SFML::Init(window);
    image.create(INITIAL_IMAGE_WIDTH, INITIAL_IMAGE_HEIGHT, reinterpret_cast<sf::Uint8*>(pixels));
    texture.loadFromImage(image);
    sprite.setTexture(texture, true);
    sprite.setPosition(-1, -1);
  }

  void run(long maxComputations, uint threadCount, uint startTargetRendersPerSecond) {
    auto& board = this->board;
    auto& window = this->window;

    // Statistics
    long totalComputations = 0;
    long totalRenders = 0;
    long computationsSinceLastGuiDraw = 0;

    // Configuration
    uint targetRendersPerSecond = startTargetRendersPerSecond;
    bool running = true;

    // Computation loop
    std::thread nextBoardThread([&]() {
      while (running && totalComputations < maxComputations) {
        board.lock.pauseIfRequested();

        nextBoard(board, threadCount);
        totalComputations++;
        computationsSinceLastGuiDraw++;
      }
    });

    // Render loop
    sf::Clock renderDeltaClock;
    while (window.isOpen() && totalComputations < maxComputations) {
      const auto renderDelta = renderDeltaClock.restart();
      auto renderTimer = start();

      // Update GUI
      ImGui::SFML::Update(window, renderDelta);

      // Render everything
      drawBoard(board, window, sprite, texture, image, pixels);
      drawGui(
          board, window, renderDelta, totalComputations, computationsSinceLastGuiDraw, targetRendersPerSecond,
          threadCount);
      computationsSinceLastGuiDraw = 0;
      ImGui::SFML::Render(window);
      window.display();

      // Update window (handle window events, handle input)
      sf::Event event;
      while (window.pollEvent(event)) {
        ImGui::SFML::ProcessEvent(event);

        // On window close, exit
        if (event.type == sf::Event::Closed ||
            (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
          window.close();
          ImGui::SFML::Shutdown();

          // On window resize, update texture and board
        } else if (event.type == sf::Event::Resized) {
          auto scope = LockForScope(board.lock);

          window.setView(sf::View(sf::FloatRect(0, 0, event.size.width, event.size.height)));
          delete[] pixels;
          pixels = new sf::Uint32[(event.size.width + 2) * (event.size.height + 2)];
          board.setSize(event.size.width, event.size.height);
          assignBenchmarkCells(board);

          // On pressing enter, reset benchmark scenario
        } else if ((event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter)) {
          auto scope = LockForScope(board.lock);

          assignBenchmarkCells(board);
        }
      }

      // Limit render rate
      stopAndDelay(renderTimer, 1000000 / targetRendersPerSecond);
      totalRenders++;
    }

    // Close computation thread
    running = false;
    nextBoardThread.join();
    delete[] pixels;
  }
};