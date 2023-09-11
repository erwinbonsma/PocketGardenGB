#pragma once

#include <array>
#include <cstdint>

#include "BitGrid.h"
#include "Utils.h"

constexpr int CA_WIDTH = 80;
constexpr int CA_HEIGHT = 64;

constexpr int BITS_PER_UNIT_CA = BITS_PER_UNIT - 1;
constexpr int UNITS_PER_ROW_CA = CA_WIDTH / BITS_PER_UNIT_CA + 1;
constexpr int BYTES_PER_ROW_CA = 4 * UNITS_PER_ROW_CA;

constexpr int CA_UNIT_WIDTH = UNITS_PER_ROW_CA;
constexpr int CA_UNIT_HEIGHT = CA_HEIGHT + 2;

class CellCounter;

class LifeCa {
  friend CellCounter;
  friend void revive();

  uint32_t data_[CA_UNIT_WIDTH * CA_UNIT_HEIGHT];
  BitGrid bit_grid_;

  void restoreRightBits();
  void setBorderBits();

public:
  LifeCa();

  void reset();
  void randomize();

  void step();

  void draw(int layer) const;

  bool get(int x, int y) const {
    return (data_[indexOf(x, y)] >> ((x + 1) % BITS_PER_UNIT_CA)) & 0x1;
  }
  bool set(int x, int y) {
    data_[indexOf(x, y)] |= (0x1 << ((x + 1) % BITS_PER_UNIT_CA));
  }
  bool clear(int x, int y) {
    data_[indexOf(x, y)] &= ~(0x1 << ((x + 1) % BITS_PER_UNIT_CA));
  }

private:
  int indexOf(int x, int y) const {
    return (x + 1) / BITS_PER_UNIT_CA + (y + 1) * UNITS_PER_ROW_CA;
  }

};

constexpr int NUM_CA_LAYERS = 4;
extern std::array<LifeCa, NUM_CA_LAYERS> cas;
