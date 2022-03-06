#include "CellCountHistory.h"

#include <Gamebuino-Meta.h>
#undef min
#undef max

#include <array>

#include "LifeCa.h"

class CellCounter {
  std::array<uint8_t, 256> lookup_;

public:
  CellCounter();

  int countCells(const LifeCa& ca) const;
};

// Generated via generate_lookups.py
constexpr uint16_t y_lookup[H] = {
    0,    2,    6,   10,   16,   22,   28,   36,
   44,   52,   62,   72,   82,   94,  106,  118,
  132,  146,  160,  176,  192,  208,  226,  244,
  262,  282,  302,  322,  344,  366,  388,  412,
  436,  460,  486,  512,  538,  566,  594,  622,
  652,  682,  712,  744,  776,  808,  842,  876,
  910,  946,  982, 1018, 1056, 1094, 1132, 1172,
 1212, 1252, 1294, 1336, 1378, 1422, 1466, 1510
};

constexpr uint32_t mask_c = ~(0x1 << bits_per_unit_ca);
constexpr uint32_t mask_l = mask_c & ~0x1;
constexpr int num_bits_last_unit = ca_width % bits_per_unit_ca + 1;
constexpr uint32_t mask_r = mask_c & ((uint32_t)~0x0) >> (bits_per_unit - num_bits_last_unit);

CellCounter cell_counter;

int cellCountToY(int cellCount) {
  int y = (y_lookup[32] <= cellCount) ? 32 : 0;

  y += (y_lookup[y + 16] <= cellCount) ? 16 : 0;
  y += (y_lookup[y +  8] <= cellCount) ?  8 : 0;
  y += (y_lookup[y +  4] <= cellCount) ?  4 : 0;
  y += (y_lookup[y +  2] <= cellCount) ?  2 : 0;
  y += (y_lookup[y +  1] <= cellCount) ?  1 : 0;

  return y;
}

CellCounter::CellCounter() {
  for (int i = 0; i < 256; ++i) {
    lookup_[i] = countBits(i);
  }
}

int CellCounter::countCells(const LifeCa& ca) const {
  int num_bits = 0;
  int unit_index = 0;

  const uint32_t *src_p = &ca.data_[units_per_row_ca];
  const uint32_t *end_p = src_p + units_per_row_ca * ca_height;

  while (src_p < end_p) {
    int v;
    switch (unit_index) {
      case 0:
        v = *src_p & mask_l;
        unit_index = 1;
        break;
      case units_per_row_ca - 1:
        v = *src_p & mask_r;
        unit_index = 0;
        break;
      default:
        v = *src_p & mask_c;
        ++unit_index;
    }

    num_bits += lookup_[v & 0xff];
    num_bits += lookup_[(v >>  8) & 0xff];
    num_bits += lookup_[(v >> 16) & 0xff];
    num_bits += lookup_[(v >> 24) & 0xff];

    ++src_p;
  }

  return num_bits;
}

void CellCountHistory::reset() {
  // Populate history with only current cell count
  last_entry_index_ = history_len - 1;
  countCells();
  wrapped_ = false;
}

int CellCountHistory::countCells() {
  ++last_entry_index_;
  if (last_entry_index_ == history_len) {
    last_entry_index_ = 0;
    wrapped_ = true;
  }
  assertTrue(last_entry_index_ < history_len);

  int total = 0;
  int layer = 0;
  for (auto& ca : cas) {
    int cell_count = cell_counter.countCells(ca);
    cell_counts_[layer][last_entry_index_] = cell_count;
    total += cell_count;
    ++layer;
  }

  return total;
}

int CellCountHistory::totalCells() {
  int total = 0;

  for (int i = num_ca_layers; --i >= 0; ) {
    total += cell_counts_[i][last_entry_index_];
  }

  return total;
}

int CellCountHistory::numEmptyLayers() {
  int num_empty = 0;

  for (int i = num_ca_layers; --i >= 0; ) {
    if (cell_counts_[i][last_entry_index_] == 0) {
      ++num_empty;
    }
  }

  return num_empty;
}


void CellCountHistory::plot() {
  uint8_t* buf = reinterpret_cast<uint8_t*>(gb.display._buffer);

  int start_index = wrapped_ ? (last_entry_index_ + 1) % history_len : 0;
  int num_points = wrapped_ ? history_len : last_entry_index_ + 1;

  for (int i = 0; i < num_ca_layers; ++i) {
    const auto& history = cell_counts_[i];

    for (int x = num_points; --x >= 0; ) {
      int y = 63 - cellCountToY(history[(start_index + x) % history_len]);
      int addr = (W / 2) * y + x / 2;
      buf[addr] |= 0x1 << i + (x % 2 ? 0 : 4);
    }
  }
}
