#include "CellCountHistory.h"

#include <Gamebuino-Meta.h>
#undef min
#undef max

CellCountHistory cell_counts;

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

void plotCellCounts(int tmax) {
  int tmin = std::max(tmax - history_len, 0);
  uint8_t* buf = reinterpret_cast<uint8_t*>(gb.display._buffer);

  for (int i = 0; i < cell_counts.size(); ++i) {
    const auto& history = cell_counts[i];

    for (int t = tmin; t < tmax; ++t) {
      int x = t - tmin;
      int y = 63 - cellCountToY(history[t % history_len]);
      int addr = (W / 2) * y + x / 2;
      buf[addr] |= 0x1 << i + (x % 2 ? 0 : 4);
    }
  }
}
