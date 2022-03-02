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
    0,    2,    6,   10,   16,   22,   28,   36,
   44,   52,   62,   72,   82,   94,  106,  118,
  132,  146,  160,  176,  192,  208,  226,  244,
  262,  282,  302,  322,  344,  366,  388,  412,
  436,  460,  486,  512,  538,  566,  594,  622,
  652,  682,  712,  744,  776,  808,  842,  876,
  910,  946,  982, 1018, 1056, 1094, 1132, 1172,
 1212, 1252, 1294, 1336, 1378, 1422, 1466, 1510
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
