#pragma once

#include <cstdint>

constexpr int num_flower_colors = 4;
constexpr int num_flower_frames = 11;

class Flower {
  uint16_t cycle_;
  uint8_t colors_[num_flower_colors];
  uint8_t sprites_[num_flower_frames];
  uint8_t grow_step_;

public:
  void init();

  void update();
  void draw(int x, int y);
};
