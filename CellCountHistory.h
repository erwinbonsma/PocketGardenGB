#include <array>

#include "Utils.h"

constexpr int history_len = W;

class CellCountHistory {
  std::array<std::array<uint16_t, history_len>, num_ca_layers> cell_counts_;
  uint8_t last_entry_index_ = 0;
  bool wrapped_;

public:
  void reset();
  int countCells();
  int totalCells();
  void plot();

  uint16_t numCells(int layer) { return cell_counts_[layer][last_entry_index_]; }
};
