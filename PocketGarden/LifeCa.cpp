#include "LifeCa.h"

#include <Gamebuino-Meta.h>
#undef min
#undef max

constexpr uint32_t BITS_IN_LAST_CA_UNIT = CA_WIDTH % BITS_PER_UNIT_CA + 1;

// Masks with valid bits
constexpr uint32_t MASK_CENTER = ~(0x1 << BITS_PER_UNIT_CA);
constexpr uint32_t MASK_LEFT = MASK_CENTER & ~0x1;
constexpr uint32_t MASK_RIGHT = (
  (UNITS_PER_ROW_CA == 1 ? MASK_LEFT : MASK_CENTER)
  & ~0x0 >> (BITS_PER_UNIT - BITS_IN_LAST_CA_UNIT)
);

// Bit positions used by setBorderBits
constexpr uint8_t BIT_POS_L_DST = 0;
constexpr uint8_t BIT_POS_L_SRC = 1;
constexpr uint8_t BIT_POS_R_DST = CA_WIDTH % BITS_PER_UNIT_CA + 1;
constexpr uint8_t BIT_POS_R_SRC = BIT_POS_R_DST - 1;

uint32_t rows[3 * UNITS_PER_ROW_CA];

std::array<LifeCa, NUM_CA_LAYERS> cas;

LifeCa::LifeCa()
: bit_grid_(data_, CA_UNIT_WIDTH * BITS_PER_UNIT, CA_UNIT_HEIGHT) {
  // void
}

void LifeCa::reset() {
  bit_grid_.reset();
}

void LifeCa::randomize() {
  for (int i = CA_UNIT_WIDTH * CA_UNIT_HEIGHT; --i >= 0; ) {
    data_[i] = random(0xffff) | random(0xffff) << 16;
  }
}

void LifeCa::restoreRightBits() {
  uint16_t unit_index = UNITS_PER_ROW_CA;
  uint16_t limit = UNITS_PER_ROW_CA * (CA_HEIGHT + 1);

  uint32_t val = data_[unit_index];

  while (unit_index < limit) {
    uint32_t val_nxt = data_[unit_index + 1];

    data_[unit_index] = (val & ~(0x1 << BITS_PER_UNIT_CA)) | (val_nxt & 0x1) << BITS_PER_UNIT_CA;
    val = val_nxt;
    ++unit_index;
  }
}

void LifeCa::setBorderBits() {
  uint16_t unit_index_l = UNITS_PER_ROW_CA;
  uint16_t unit_index_r = UNITS_PER_ROW_CA * 2 - 1;

  uint16_t limit = UNITS_PER_ROW_CA * (CA_HEIGHT + 1);
  while (unit_index_l < limit) {
    uint32_t l = data_[unit_index_l];
    uint32_t r = data_[unit_index_r];

    data_[unit_index_l] = l & ~(0x1 << BIT_POS_L_DST) | (r & 0x1 << BIT_POS_R_SRC) >> BIT_POS_R_SRC - BIT_POS_L_DST;
    data_[unit_index_r] = r & ~(0x1 << BIT_POS_R_DST) | (l & 0x1 << BIT_POS_L_SRC) << BIT_POS_R_DST - BIT_POS_L_SRC;

    unit_index_l += UNITS_PER_ROW_CA;
    unit_index_r += UNITS_PER_ROW_CA;
  }

  memcpy(&data_[0], &data_[UNITS_PER_ROW_CA * CA_HEIGHT], BYTES_PER_ROW_CA);
  memcpy(&data_[limit], &data_[UNITS_PER_ROW_CA], BYTES_PER_ROW_CA);
}

void LifeCa::step() {
  uint8_t row_above = 0;
  uint8_t row_currn = UNITS_PER_ROW_CA;
  uint8_t row_below = UNITS_PER_ROW_CA * 2;

  restoreRightBits();
  setBorderBits();

  // Init row above and row current
  memcpy(&rows[0], &data_[0], BYTES_PER_ROW_CA * 2);
  uint16_t data_index = UNITS_PER_ROW_CA * 2;

  uint16_t unit_index = UNITS_PER_ROW_CA;
  for (int i = 0; i < H; ++i) {
    // Init row below
    memcpy(&rows[row_below], &data_[data_index], BYTES_PER_ROW_CA);
    data_index += UNITS_PER_ROW_CA;

    uint32_t abc_sum_prev = 0;
    uint32_t abc_car_prev = 0;

    for (int j = 0; j < UNITS_PER_ROW_CA; ++j) {
      uint32_t above = rows[row_above + j];
      uint32_t currn = rows[row_currn + j];
      uint32_t below = rows[row_below + j];

      // above + below
      uint32_t ab_sum = above ^ below;
      uint32_t ab_car = above & below;

      // above + below + current
      uint32_t abc_sum = currn ^ ab_sum;
      uint32_t abc_car = currn & ab_sum | ab_car;

      // sum of bit0 (sum of sums)
      uint32_t l = abc_sum << 1 | abc_sum_prev >> (BITS_PER_UNIT_CA - 1);
      uint32_t r = abc_sum >> 1; // Note: cannot include abc_sum_next, so incorrect for rightmost bit.

      uint32_t lr = l ^ r;
      uint32_t sum0 = lr ^ ab_sum;
      uint32_t car0 = l & r | lr & ab_sum;

      // sum of bit1 (sum of carry's)
      l = abc_car << 1 | abc_car_prev >> (BITS_PER_UNIT_CA - 1);
      r = abc_car >> 1;
      lr = l ^ r;

      uint32_t sum1 = lr ^ ab_car;
      uint32_t car1 = l & r | lr & ab_car;

      data_[unit_index] = (currn | sum0) & (car0 ^ sum1) & ~car1;
      ++unit_index;

      abc_sum_prev = abc_sum;
      abc_car_prev = abc_car;
    }

    uint8_t row_tmp = row_above;
    row_above = row_currn;
    row_currn = row_below;
    row_below = row_tmp;
  }
}

void LifeCa::draw(int layer) const {
  uint16_t  *dst_p = gb.display._buffer;
  const uint32_t  *src_p = &data_[UNITS_PER_ROW_CA];
  uint16_t  layer_mask = expand[15] << layer;

  for (int y = 0; y < CA_HEIGHT; ++y) {
    int bits_remaining = CA_WIDTH;
    int rbpu = BITS_PER_UNIT_CA - 1;

    while (bits_remaining > 0) {
      int v;
      int num_bits = std::min(rbpu, bits_remaining);
      if (rbpu >= 4) {
        v = (*src_p >> (BITS_PER_UNIT_CA - rbpu)) & 0xf;
        rbpu -= 4;
      } else {
        v = (*src_p & 0x7fffffff) >> (BITS_PER_UNIT_CA - rbpu);
        ++src_p;
        v |= (*src_p << rbpu) & 0xf;
        rbpu = BITS_PER_UNIT_CA - (4 - rbpu);
      }
      bits_remaining -= 4;
      uint16_t set_mask = expand[v] << layer;

      // Set the display bits for the alive cells.
      *dst_p |= set_mask;

      // Do not assume that screen was empty. Explicitly clear the
      // display bits for the empty cells.
      *dst_p &= ~layer_mask | set_mask;

      ++dst_p;
    }
    ++src_p;
  }
}
