#include "LivelinessCheck.h"

#include <algorithm>

#include "Utils.h"

constexpr int liveliness_limit = 50;

void LivelinessCheck::reset() {
  min_cells_ = W * H;
}

bool LivelinessCheck::update(uint16_t num_cells) {
  min_cells_ = std::min(min_cells_, num_cells);

  if (num_cells > min_cells_ + liveliness_limit) {
    min_cells_ = num_cells;
    return true;
  }

  return false;
}
