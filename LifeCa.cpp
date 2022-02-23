#include "LifeCa.h"

constexpr uint32_t bits_in_last_ca_unit = ca_width % bits_per_unit_ca + 1;

// Masks with valid bits
constexpr uint32_t mask_center = ~(0x1 << bits_per_unit_ca);
constexpr uint32_t mask_left = mask_center & ~0x1;
constexpr uint32_t mask_right = (
  (units_per_row_ca == 1 ? mask_left : mask_center)
  & ~0x0 >> (bits_per_unit - bits_in_last_ca_unit)
);

// Bit positions used by setBorderBits
constexpr uint8_t bit_pos_l_dst = 0;
constexpr uint8_t bit_pos_l_src = 1;
constexpr uint8_t bit_pos_r_dst = ca_width % bits_per_unit_ca + 1;
constexpr uint8_t bit_pos_r_src = bit_pos_r_dst - 1;

LifeCa::LifeCa()
: bit_grid_(data_, bit_grid_width, bit_grid_height) {
  // void
}

void LifeCa::reset() {
  steps_  = 0;
  bit_grid_.reset();
}

void LifeCa::restoreRightBits() {
  uint16_t unit_index = units_per_row_ca;
  uint16_t limit = units_per_row_ca * (H + 1);

  uint32_t val = data_[unit_index];

  while (unit_index < limit) {
    uint32_t val_nxt = data_[unit_index + 1];

    data_[unit_index] = val & (~0x1 | val_nxt) << bits_per_unit_ca;
    val = val_nxt;
    ++unit_index;
  }
}

void LifeCa::setBorderBits() {
  uint16_t unit_index_l = units_per_row_ca;
  uint16_t unit_index_r = units_per_row_ca * 2 - 1;

  uint16_t limit = units_per_row_ca * (H + 1);
  while (unit_index_l < limit) {
    uint32_t l = data_[unit_index_l];
    uint32_t r = data_[unit_index_r];

    data_[unit_index_l] = l & (~0x1 << bit_pos_l_dst) | (r & (0x1 << bit_pos_r_src)) >> (bit_pos_r_src - bit_pos_l_dst);
    data_[unit_index_r] = r & (~0x1 << bit_pos_r_dst) | (l & (0x1 << bit_pos_l_src)) << (bit_pos_r_dst - bit_pos_l_src);

    unit_index_l += units_per_row_ca;
    unit_index_r += units_per_row_ca;
  }
}

void LifeCa::step() {
  uint8_t row_above = 0;
  uint8_t row_currn = units_per_row_ca;
  uint8_t row_below = units_per_row_ca * 2;

  ++steps_;

  restoreRightBits();
  setBorderBits();

  // Init row above and row current
  memcpy(&rows_[0], &data_[0], bytes_per_row_ca * 2);
  uint16_t data_index = units_per_row_ca * 2;

  uint16_t unit_index = units_per_row_ca;
  for (int i = 0; i < H; ++i) {
    // Init row below
    memcpy(&rows_[row_below], &data_[data_index], bytes_per_row_ca);
    data_index += units_per_row_ca;

    uint32_t abc_sum_prev = 0;
    uint32_t abc_car_prev = 0;

    for (int j = 0; j < units_per_row_ca; ++j) {
      uint32_t above = rows_[row_above + j];
      uint32_t below = rows_[row_below + j];
      uint32_t currn = rows_[row_currn + j];

      // above + below
      uint32_t ab_sum = above ^ below;
      uint32_t ab_car = above & below;

      // above + below + current
      uint32_t abc_sum = currn ^ ab_sum;
      uint32_t abc_car = currn & ab_sum | ab_car;

      // sum of bit0 (sum of sums)
      uint32_t l = abc_sum << 1 | abc_sum_prev >> (bits_per_unit_ca - 1);
      uint32_t r = abc_sum >> 1; // Note: cannot include abc_sum_next, so incorrect for rightmost bit.

      uint32_t lr = l ^ r;
      uint32_t sum0 = lr ^ ab_sum;
      uint32_t car0 = l & r | lr & ab_sum;

      // sum of bit1 (sum of carry's)
      l = abc_car << 1 | abc_car_prev >> (bits_per_unit_ca - 1);
      r = abc_car >> 1;
      lr = l ^ r;

      uint32_t sum1 = lr ^ ab_sum;
      uint32_t car1 = l & r | lr & ab_car;

      data_[unit_index] = (currn | sum0) & (car0 ^ sum1) & ~car1;
      ++unit_index;

      abc_sum_prev = abc_sum;
      abc_car_prev = abc_car;
    }

    uint8_t row_tmp;
    row_above = row_currn;
    row_currn = row_below;
    row_below = row_tmp;
  }
}

void LifeCa::draw() {
  // TODO: draw to display by directly OR-ing bits to gb.display._buffer
}
