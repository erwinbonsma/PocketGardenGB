#pragma once

#include <cstdint>

#include "Utils.h"

constexpr int BYTES_PER_UNIT = 4;
constexpr int BITS_PER_UNIT = 32;

class BitGrid {
  uint32_t *data_;
  uint8_t units_per_row_;
  uint8_t num_rows_;

public:
  BitGrid(uint32_t *data, int width, int height)
  : data_(data), units_per_row_(width / BITS_PER_UNIT), num_rows_(height) {
    assertTrue(width % BITS_PER_UNIT == 0);
  }

  bool get(int x, int y) {
    return (data_[indexOf(x, y)] >> (x % BITS_PER_UNIT)) & 0x1;
  }
  bool set(int x, int y) {
    data_[indexOf(x, y)] |= (0x1 << (x % BITS_PER_UNIT));
  }
  bool clear(int x, int y) {
    data_[indexOf(x, y)] &= ~(0x1 << (x % BITS_PER_UNIT));
  }

  void reset();

private:
  int indexOf(int x, int y) {
    return x / BITS_PER_UNIT + y * units_per_row_;
  }
};
