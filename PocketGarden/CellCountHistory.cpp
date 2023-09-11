#include "CellCountHistory.h"

#include <Gamebuino-Meta.h>

#include "LifeCa.h"

class CellCounter {
  std::array<uint8_t, 256> lookup_;

public:
  CellCounter();

  int countCells(const LifeCa& ca) const;
};

// Generated via generate_lookups.py
constexpr uint16_t Y_LOOKUP[H] = {
    0,    2,    6,   10,   16,   22,   28,   36,
   44,   52,   62,   72,   82,   94,  106,  118,
  132,  146,  160,  176,  192,  208,  226,  244,
  262,  282,  302,  322,  344,  366,  388,  412,
  436,  460,  486,  512,  538,  566,  594,  622,
  652,  682,  712,  744,  776,  808,  842,  876,
  910,  946,  982, 1018, 1056, 1094, 1132, 1172,
 1212, 1252, 1294, 1336, 1378, 1422, 1466, 1510
};

constexpr uint32_t MASK_C = ~(0x1 << BITS_PER_UNIT_CA);
constexpr uint32_t MASK_L = MASK_C & ~0x1;
constexpr int NUM_BITS_LAST_UNIT = CA_WIDTH % BITS_PER_UNIT_CA + 1;
constexpr uint32_t MASK_R = MASK_C & ((uint32_t)~0x0) >> (BITS_PER_UNIT - NUM_BITS_LAST_UNIT);

CellCounter cell_counter;

std::array<CellCountHistory, NUM_CA_LAYERS> cell_count_histories;

int cellCountToY(int cellCount) {
  int y = (Y_LOOKUP[32] <= cellCount) ? 32 : 0;

  y += (Y_LOOKUP[y + 16] <= cellCount) ? 16 : 0;
  y += (Y_LOOKUP[y +  8] <= cellCount) ?  8 : 0;
  y += (Y_LOOKUP[y +  4] <= cellCount) ?  4 : 0;
  y += (Y_LOOKUP[y +  2] <= cellCount) ?  2 : 0;
  y += (Y_LOOKUP[y +  1] <= cellCount) ?  1 : 0;

  return y;
}

int countCells(const LifeCa& ca) {
  return cell_counter.countCells(ca);
}

CellCounter::CellCounter() {
  for (int i = 0; i < 256; ++i) {
    lookup_[i] = countBits(i);
  }
}

int CellCounter::countCells(const LifeCa& ca) const {
  int num_bits = 0;
  int unit_index = 0;

  const uint32_t *src_p = &ca.data_[UNITS_PER_ROW_CA];
  const uint32_t *end_p = src_p + UNITS_PER_ROW_CA * CA_HEIGHT;

  while (src_p < end_p) {
    int v;
    switch (unit_index) {
      case 0:
        v = *src_p & MASK_L;
        unit_index = 1;
        break;
      case UNITS_PER_ROW_CA - 1:
        v = *src_p & MASK_R;
        unit_index = 0;
        break;
      default:
        v = *src_p & MASK_C;
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

void CellCountHistory::reset(const LifeCa& ca) {
  // Populate history with only current cell count
  last_entry_index_ = HISTORY_LEN - 1;
  countCells(ca);
  wrapped_ = false;
}

void CellCountHistory::addCount(int count) {
  ++last_entry_index_;
  if (last_entry_index_ == HISTORY_LEN) {
    last_entry_index_ = 0;
    wrapped_ = true;
  }
  assertTrue(last_entry_index_ < HISTORY_LEN);

  cell_counts_[last_entry_index_] = count;
}

int CellCountHistory::countCells(const LifeCa& ca) {
  int cell_count = cell_counter.countCells(ca);
  addCount(cell_count);

  return cell_count;
}

void CellCountHistory::plot(int layer) const {
  uint8_t* buf = reinterpret_cast<uint8_t*>(gb.display._buffer);

  int start_index = wrapped_ ? (last_entry_index_ + 1) % HISTORY_LEN : 0;
  int num_points = wrapped_ ? HISTORY_LEN : last_entry_index_ + 1;

  for (int x = num_points; --x >= 0; ) {
    int y = 63 - cellCountToY(cell_counts_[(start_index + x) % HISTORY_LEN]);
    int addr = (W / 2) * y + x / 2;
    buf[addr] |= 0x1 << layer + (x % 2 ? 0 : 4);
  }
}
