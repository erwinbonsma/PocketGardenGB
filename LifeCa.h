#ifndef LIFE_CA_H
#define LIFE_CA_H

#include "BitGrid.h"

#include <Gamebuino-Meta.h>

constexpr int ca_width = 80;
constexpr int ca_height = 64;

constexpr int bits_per_unit_ca = bits_per_unit - 1;
constexpr int units_per_row_ca = (ca_width + bits_per_unit_ca - 1) / bits_per_unit_ca + 1;
constexpr int bytes_per_row_ca = 4 * units_per_row_ca;

constexpr int bit_grid_width = units_per_row_ca * bits_per_unit;
constexpr int bit_grid_height = ca_height + 2;

class LifeCa {
  uint32_t data_[bit_grid_width * bit_grid_height];
  BitGrid bit_grid_;

  uint32_t steps_;

public:
  LifeCa();

  void reset();
};

#endif
