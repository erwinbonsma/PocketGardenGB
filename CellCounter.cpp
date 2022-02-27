#include "CellCounter.h"

#include "LifeCa.h"

constexpr uint32_t mask_c = ~(0x1 << bits_per_unit_ca);
constexpr uint32_t mask_l = mask_c & ~0x1;
constexpr int num_bits_last_unit = ca_width % bits_per_unit_ca + 1;
constexpr uint32_t mask_r = mask_c & ~0x0 >> (bits_per_unit - num_bits_last_unit);

int countBits(int val) {
  int num_bits = 0;

  while (val) {
    if (val & 0x1) {
      ++num_bits;
    }
    val >>= 1;
  }

  return num_bits;
}

CellCounter::CellCounter() {
  for (int i = 0; i < 256; ++i) {
    lookup_[i] = countBits(i);
  }
}

int CellCounter::countCells(const LifeCa& ca) const {
  int num_bits = 0;
  int byte_index = 0;

  const uint32_t *src_p = &ca.data_[units_per_row_ca];
  const uint32_t *end_p = src_p + units_per_row_ca * ca_height;

  while (src_p < end_p) {
    int v;
    switch (byte_index) {
      case 0:
        v = *src_p & mask_l;
        byte_index = 1;
        break;
      case bytes_per_row_ca - 1:
        v = *src_p & mask_r;
        byte_index = 0;
        break;
      default:
        v = *src_p & mask_c;
        ++byte_index;
    }

    num_bits += lookup_[v & 0xff];
    num_bits += lookup_[(v >>  8) & 0xff];
    num_bits += lookup_[(v >> 16) & 0xff];
    num_bits += lookup_[(v >> 24) & 0xff];

    ++src_p;
  }

  return num_bits;
}
