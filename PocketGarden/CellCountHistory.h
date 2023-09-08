#include <array>

#include "Utils.h"

constexpr int history_len = W;

class LifeCa;

class CellCountHistory {
  std::array<uint16_t, history_len> cell_counts_;
  uint8_t last_entry_index_ = 0;
  bool wrapped_;

public:
  void reset(const LifeCa& ca);
  int countCells(const LifeCa& ca);

  bool isEmpty() const { return cell_counts_[last_entry_index_] == 0; }
  void plot(int layer) const;

  uint16_t numCells() { return cell_counts_[last_entry_index_]; }
};
