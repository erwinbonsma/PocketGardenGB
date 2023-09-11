#pragma once

#include <cstdint>

constexpr int NUM_FLOWER_COLORS = 4;
constexpr int NUM_FLOWER_FRAMES = 11;

class Flower {
  uint16_t cycle_;
  uint8_t colors_[NUM_FLOWER_COLORS];
  uint8_t sprites_[NUM_FLOWER_FRAMES];
  uint8_t grow_step_;

public:
  void init();

  void update();
  void draw(int x, int y);
};
