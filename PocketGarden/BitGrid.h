#pragma once

#include <cstdint>

#include "Utils.h"

constexpr int bytes_per_unit = 4;
constexpr int bits_per_unit = 32;

class BitGrid {
  uint32_t *data_;
  uint8_t units_per_row_;
  uint8_t num_rows_;

public:
  BitGrid(uint32_t *data, int width, int height)
  : data_(data), units_per_row_(width / bits_per_unit), num_rows_(height) {
    assertTrue(width % bits_per_unit == 0);
  }

  bool get(int x, int y) {
    return (data_[indexOf(x, y)] >> (x % bits_per_unit)) & 0x1;
  }
  bool set(int x, int y) {
    data_[indexOf(x, y)] |= (0x1 << (x % bits_per_unit));
  }
  bool clear(int x, int y) {
    data_[indexOf(x, y)] &= ~(0x1 << (x % bits_per_unit));
  }

  void reset();

private:
  int indexOf(int x, int y) {
    return x / bits_per_unit + y * units_per_row_;
  }
};
