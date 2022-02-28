#ifndef LIFE_CA_H
#define LIFE_CA_H

#include <Gamebuino-Meta.h>
#undef min
#undef max

#include <array>

#include "BitGrid.h"

constexpr int ca_width = 80;
constexpr int ca_height = 64;

constexpr int bits_per_unit_ca = bits_per_unit - 1;
constexpr int units_per_row_ca = ca_width / bits_per_unit_ca + 1;
constexpr int bytes_per_row_ca = 4 * units_per_row_ca;

constexpr int ca_unit_width = units_per_row_ca;
constexpr int ca_unit_height = ca_height + 2;

constexpr int num_ca_layers = 4;

void init_expand();

class CellCounter;

class LifeCa {
  friend CellCounter;
  friend void revive();

  uint32_t data_[ca_unit_width * ca_unit_height];
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
    return (data_[indexOf(x, y)] >> ((x + 1) % bits_per_unit_ca)) & 0x1;
  }
  bool set(int x, int y) {
    data_[indexOf(x, y)] |= (0x1 << ((x + 1) % bits_per_unit_ca));
  }
  bool clear(int x, int y) {
    data_[indexOf(x, y)] &= ~(0x1 << ((x + 1) % bits_per_unit_ca));
  }

private:
  int indexOf(int x, int y) const {
    return (x + 1) / bits_per_unit_ca + (y + 1) * units_per_row_ca;
  }

};

typedef std::array<LifeCa, num_ca_layers> LifeCaLayers;

extern LifeCaLayers cas;

#endif
