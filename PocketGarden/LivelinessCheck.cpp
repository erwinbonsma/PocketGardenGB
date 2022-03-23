#include "LivelinessCheck.h"

#include <algorithm>

#include "Utils.h"

// The cell growth (in number of cells) to trigger a liveliness increase
constexpr int liveliness_delta = 50;

// The maximum liveliness increase
constexpr int liveliness_inc_exp = 6;
constexpr uint16_t liveliness_inc = 0x1 << liveliness_inc_exp;

// The maximum liveliness level
constexpr int liveliness_max_exp = 9;
constexpr uint16_t liveliness_max = 0x1 << liveliness_max_exp;

void LivelinessCheck::reset() {
  min_cells_ = W * H;
}

bool LivelinessCheck::update(uint16_t num_cells) {
  min_cells_ = std::min(min_cells_, num_cells);

  if (num_cells > min_cells_ + liveliness_delta) {
    min_cells_ += liveliness_delta;

    liveliness_ += (
      (liveliness_max - std::min(liveliness_, liveliness_max))
    ) >> (liveliness_max_exp - liveliness_inc_exp);

    return true;
  } else if (liveliness_ > 0) {
    --liveliness_;
  }

  return false;
}
