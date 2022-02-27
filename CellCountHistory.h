#include <array>

#include "Utils.h"

constexpr int history_len = W;
typedef std::array<std::array<uint16_t, history_len>, 4> CellCountHistory;

extern CellCountHistory cell_counts;

void plotCellCounts(int tmax);
