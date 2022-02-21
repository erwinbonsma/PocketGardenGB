#include "LifeCa.h"

constexpr uint32_t bits_in_last_ca_unit = ca_width % bits_per_unit_ca + 1;

// Masks with valid bits
constexpr uint32_t mask_center = ~(0x1 << bits_per_unit_ca);
constexpr uint32_t mask_left = mask_center & ~0x1;
constexpr uint32_t mask_right = (
  (units_per_row_ca == 1 ? mask_left : mask_center)
  & ~0x0 >> (bits_per_unit - bits_in_last_ca_unit)
);

LifeCa::LifeCa()
: bit_grid_(data_, bit_grid_width, bit_grid_height) {
  // void
}

void LifeCa::reset() {
  steps_  = 0;
  bit_grid_.reset();
}
