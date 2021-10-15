#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include <array>
#include "../board/next.h"
#include "../board/padding.h"

using BoardVector = std::vector<std::vector<Cell>>;

void generate(BoardMeta& board, BoardVector vector, bool a = true) {
  const auto height = (unsigned int)vector.size();
  const auto width = (unsigned int)vector[0].size();

  board.resize(width, height);
  for (unsigned int y = 1; y < height+1; ++y)
    for (unsigned int x = 1; x < width+1; ++x)
      board.input[y*(width+2) + x] = vector[y-1][x-1] ? ALIVE : DEAD;

  padding(board.input, width, height);

  if (!a)
    board.flip();
}

BoardVector ungenerate(BoardMeta& board) {
  const auto& output = board.output;
  const auto& width = board.width;
  const auto& height = board.height;

  std::vector<std::vector<Cell>> vector(height, std::vector<Cell>(width));
  for (unsigned int y = 1; y < height-1; ++y)
    for (unsigned int x = 1; x < width-1; ++x)
      vector[y-1][x-1] = output[y * width + x] == ALIVE ? 1 : 0;

  return vector;
}

void compare(BoardVector a, BoardVector b) {
  auto boardA = BoardMeta(a.size(), a[0].size());
  auto boardB = BoardMeta(b.size(), b[0].size());
  generate(boardA, a, true);
  generate(boardB, b, true);
  REQUIRE(ungenerate(boardA) == ungenerate(boardB));
}

TEST_CASE("nothing", "[nextBoard]") {
  compare({{false}}, {{false}});
}

TEST_CASE("death", "[nextBoard]") {
  compare({{true}}, {{false}});
}

TEST_CASE("block", "[nextBoard]") {
  compare(
      {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},
      {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}});
}

TEST_CASE("block (vertical wrap)", "[nextBoard]") {
  compare(
      {{0, 1, 1, 0}, {0, 0, 0, 0}, {0, 1, 1, 0}},
      {{0, 1, 1, 0}, {0, 0, 0, 0}, {0, 1, 1, 0}});
}

TEST_CASE("block (horizontal wrap)", "[nextBoard]") {
  compare(
      {{0, 0, 0}, {1, 0, 1}, {1, 0, 1}, {0, 0, 0}},
      {{0, 0, 0}, {1, 0, 1}, {1, 0, 1}, {0, 0, 0}});
}

TEST_CASE("block (corner wrap)", "[nextBoard]") {
  compare({{1, 0, 1}, {0, 0, 0}, {1, 0, 1}}, {{1, 0, 1}, {0, 0, 0}, {1, 0, 1}});
}

TEST_CASE("bee-hive", "[nextBoard]") {
  compare(
      {{0, 0, 0, 0, 0, 0},
       {0, 0, 1, 1, 0, 0},
       {0, 1, 0, 0, 1, 0},
       {0, 0, 1, 1, 0, 0},
       {0, 0, 0, 0, 0, 0}},

      {{0, 0, 0, 0, 0, 0},
       {0, 0, 1, 1, 0, 0},
       {0, 1, 0, 0, 1, 0},
       {0, 0, 1, 1, 0, 0},
       {0, 0, 0, 0, 0, 0}});
}

TEST_CASE("loaf", "[nextBoard]") {
  compare(
      {{0, 0, 0, 0, 0, 0},
       {0, 0, 1, 1, 0, 0},
       {0, 1, 0, 0, 1, 0},
       {0, 0, 1, 0, 1, 0},
       {0, 0, 0, 1, 0, 0},
       {0, 0, 0, 0, 0, 0}},

      {{0, 0, 0, 0, 0, 0},
       {0, 0, 1, 1, 0, 0},
       {0, 1, 0, 0, 1, 0},
       {0, 0, 1, 0, 1, 0},
       {0, 0, 0, 1, 0, 0},
       {0, 0, 0, 0, 0, 0}});
}

TEST_CASE("boat", "[nextBoard]") {
  compare(
      {{0, 0, 0, 0, 0},
       {0, 1, 1, 0, 0},
       {0, 1, 0, 1, 0},
       {0, 0, 1, 0, 0},
       {0, 0, 0, 0, 0}},

      {{0, 0, 0, 0, 0},
       {0, 1, 1, 0, 0},
       {0, 1, 0, 1, 0},
       {0, 0, 1, 0, 0},
       {0, 0, 0, 0, 0}});
}

TEST_CASE("tub", "[nextBoard]") {
  compare(
      {{0, 0, 0, 0, 0},
       {0, 0, 1, 0, 0},
       {0, 1, 0, 1, 0},
       {0, 0, 1, 0, 0},
       {0, 0, 0, 0, 0}},

      {{0, 0, 0, 0, 0},
       {0, 0, 1, 0, 0},
       {0, 1, 0, 1, 0},
       {0, 0, 1, 0, 0},
       {0, 0, 0, 0, 0}});
}

TEST_CASE("blinker 1", "[nextBoard]") {
  compare(
      {{0, 0, 0, 0, 0},
       {0, 0, 1, 0, 0},
       {0, 0, 1, 0, 0},
       {0, 0, 1, 0, 0},
       {0, 0, 0, 0, 0}},

      {{0, 0, 0, 0, 0},
       {0, 0, 0, 0, 0},
       {0, 1, 1, 1, 0},
       {0, 0, 0, 0, 0},
       {0, 0, 0, 0, 0}});
}

TEST_CASE("blinker 2", "[nextBoard]") {
  compare(
      {{0, 0, 0, 0, 0},
       {0, 0, 0, 0, 0},
       {0, 1, 1, 1, 0},
       {0, 0, 0, 0, 0},
       {0, 0, 0, 0, 0}},

      {{0, 0, 0, 0, 0},
       {0, 0, 1, 0, 0},
       {0, 0, 1, 0, 0},
       {0, 0, 1, 0, 0},
       {0, 0, 0, 0, 0}});
}

TEST_CASE("toad 1", "[nextBoard]") {
  compare(
      {{0, 0, 0, 0, 0, 0},
       {0, 0, 0, 0, 0, 0},
       {0, 0, 1, 1, 1, 0},
       {0, 1, 1, 1, 0, 0},
       {0, 0, 0, 0, 0, 0},
       {0, 0, 0, 0, 0, 0}},

      {{0, 0, 0, 0, 0, 0},
       {0, 0, 0, 1, 0, 0},
       {0, 1, 0, 0, 1, 0},
       {0, 1, 0, 0, 1, 0},
       {0, 0, 1, 0, 0, 0},
       {0, 0, 0, 0, 0, 0}});
}

TEST_CASE("toad 2", "[nextBoard]") {
  compare(
      {{0, 0, 0, 0, 0, 0},
       {0, 0, 0, 1, 0, 0},
       {0, 1, 0, 0, 1, 0},
       {0, 1, 0, 0, 1, 0},
       {0, 0, 1, 0, 0, 0},
       {0, 0, 0, 0, 0, 0}},

      {{0, 0, 0, 0, 0, 0},
       {0, 0, 0, 0, 0, 0},
       {0, 0, 1, 1, 1, 0},
       {0, 1, 1, 1, 0, 0},
       {0, 0, 0, 0, 0, 0},
       {0, 0, 0, 0, 0, 0}});
}

TEST_CASE("beacon 1", "[nextBoard]") {
  compare(
      {{0, 0, 0, 0, 0, 0},
       {0, 1, 1, 0, 0, 0},
       {0, 1, 0, 0, 0, 0},
       {0, 0, 0, 0, 1, 0},
       {0, 0, 0, 1, 1, 0},
       {0, 0, 0, 0, 0, 0}},

      {{0, 0, 0, 0, 0, 0},
       {0, 1, 1, 0, 0, 0},
       {0, 1, 1, 0, 0, 0},
       {0, 0, 0, 1, 1, 0},
       {0, 0, 0, 1, 1, 0},
       {0, 0, 0, 0, 0, 0}});
}

TEST_CASE("beacon 2", "[nextBoard]") {
  compare(
      {{0, 0, 0, 0, 0, 0},
       {0, 1, 1, 0, 0, 0},
       {0, 1, 1, 0, 0, 0},
       {0, 0, 0, 1, 1, 0},
       {0, 0, 0, 1, 1, 0},
       {0, 0, 0, 0, 0, 0}},

      {{0, 0, 0, 0, 0, 0},
       {0, 1, 1, 0, 0, 0},
       {0, 1, 0, 0, 0, 0},
       {0, 0, 0, 0, 1, 0},
       {0, 0, 0, 1, 1, 0},
       {0, 0, 0, 0, 0, 0}});
}

TEST_CASE("glider 1", "[nextBoard]") {
  compare(
      {{1, 0, 0, 0, 0},
       {0, 1, 1, 0, 0},
       {1, 1, 0, 0, 0},
       {0, 0, 0, 0, 0},
       {0, 0, 0, 0, 0}},

      {{0, 1, 0, 0, 0},
       {0, 0, 1, 0, 0},
       {1, 1, 1, 0, 0},
       {0, 0, 0, 0, 0},
       {0, 0, 0, 0, 0}});
}

TEST_CASE("glider 2", "[nextBoard]") {
  compare(
      {{0, 1, 0, 0, 0},
       {0, 0, 1, 0, 0},
       {1, 1, 1, 0, 0},
       {0, 0, 0, 0, 0},
       {0, 0, 0, 0, 0}},

      {{0, 0, 0, 0, 0},
       {1, 0, 1, 0, 0},
       {0, 1, 1, 0, 0},
       {0, 1, 0, 0, 0},
       {0, 0, 0, 0, 0}});
}

TEST_CASE("glider 3", "[nextBoard]") {
  compare(
      {{0, 0, 0, 0, 0},
       {0, 1, 0, 1, 0},
       {0, 0, 1, 1, 0},
       {0, 0, 1, 0, 0},
       {0, 0, 0, 0, 0}},

      {{0, 0, 0, 0, 0},
       {0, 0, 0, 1, 0},
       {0, 1, 0, 1, 0},
       {0, 0, 1, 1, 0},
       {0, 0, 0, 0, 0}});
}

TEST_CASE("glider 4", "[nextBoard]") {
  compare(
      {{0, 0, 0, 0, 0},
       {0, 0, 0, 1, 0},
       {0, 1, 0, 1, 0},
       {0, 0, 1, 1, 0},
       {0, 0, 0, 0, 0}},

      {{0, 0, 0, 0, 0},
       {0, 0, 1, 0, 0},
       {0, 0, 0, 1, 1},
       {0, 0, 1, 1, 0},
       {0, 0, 0, 0, 0}});
}

TEST_CASE("glider 4 [long board]", "[nextBoard]") {
  compare(
      {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
       {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
       {1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0},
       {0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0},
       {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},

      {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
       {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
       {0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1},
       {0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0},
       {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}});
}

TEST_CASE("glider 4 [tall board]", "[nextBoard]") {
  compare(
      {{0, 0, 0, 0, 0},
       {0, 0, 0, 1, 0},
       {0, 1, 0, 1, 0},
       {0, 0, 1, 1, 0},
       {0, 0, 0, 0, 0},
       {0, 0, 0, 0, 0},
       {0, 0, 0, 0, 0},
       {0, 0, 0, 0, 0},
       {0, 0, 0, 0, 0},
       {0, 0, 0, 0, 0},
       {0, 0, 0, 0, 0},
       {0, 0, 0, 0, 0},
       {0, 0, 0, 0, 0},
       {0, 0, 0, 1, 0},
       {0, 1, 0, 1, 0},
       {0, 0, 1, 1, 0},
       {0, 0, 0, 0, 0}},

      {{0, 0, 0, 0, 0},
       {0, 0, 1, 0, 0},
       {0, 0, 0, 1, 1},
       {0, 0, 1, 1, 0},
       {0, 0, 0, 0, 0},
       {0, 0, 0, 0, 0},
       {0, 0, 0, 0, 0},
       {0, 0, 0, 0, 0},
       {0, 0, 0, 0, 0},
       {0, 0, 0, 0, 0},
       {0, 0, 0, 0, 0},
       {0, 0, 0, 0, 0},
       {0, 0, 0, 0, 0},
       {0, 0, 1, 0, 0},
       {0, 0, 0, 1, 1},
       {0, 0, 1, 1, 0},
       {0, 0, 0, 0, 0}});
}

TEST_CASE("basic", "[padding]") {
  Cell input[16]{
    0, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 0,
  };
  Cell expected[16]{
    1, 0, 1, 0,
    0, 1, 0, 1,
    1, 0, 1, 0,
    0, 1, 0, 1,
  };
  padding((Cell*)&input, 2, 2);
  REQUIRE(std::equal(std::begin(input), std::end(input), std::begin(expected)));
}
