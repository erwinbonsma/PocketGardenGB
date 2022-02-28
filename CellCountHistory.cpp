#include "CellCountHistory.h"

#include <Gamebuino-Meta.h>
#undef min
#undef max

// TODO: Move implementation to here
#include "CellCounter.h"

#include "LifeCa.h"

CellCounter cell_counter;

// Generated via generate_lookups.py
constexpr uint16_t y_lookup[H] = {
    0,    3,    9,   16,   24,   33,   43,   54,
   66,   79,   93,  108,  124,  141,  159,  178,
  198,  219,  241,  264,  288,  313,  339,  366,
  394,  423,  453,  484,  516,  549,  583,  618,
  654,  691,  729,  768,  808,  849,  891,  934,
  978, 1023, 1069, 1116, 1164, 1213, 1263, 1314,
 1366, 1419, 1473, 1528, 1584, 1641, 1699, 1758,
 1818, 1879, 1941, 2004, 2068, 2133, 2199, 2266
};

int cellCountToY(int cellCount) {
  int y = (y_lookup[32] <= cellCount) ? 32 : 0;

  y += (y_lookup[y + 16] <= cellCount) ? 16 : 0;
  y += (y_lookup[y +  8] <= cellCount) ?  8 : 0;
  y += (y_lookup[y +  4] <= cellCount) ?  4 : 0;
  y += (y_lookup[y +  2] <= cellCount) ?  2 : 0;
  y += (y_lookup[y +  1] <= cellCount) ?  1 : 0;

  return y;
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
