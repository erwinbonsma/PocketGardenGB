#include "LivelinessCheck.h"

#include "Utils.h"

// The cell growth (in number of cells) to trigger a liveliness increase
constexpr int LIVELINESS_DELTA = 50;

// The maximum liveliness increase
constexpr int LIVELINESS_INC_EXP = 6;
constexpr uint16_t LIVELINESS_INC = 0x1 << LIVELINESS_INC_EXP;

// The maximum liveliness level
constexpr int LIVELINESS_MAX_EXP = 9;
constexpr uint16_t LIVELINESS_MAX = 0x1 << LIVELINESS_MAX_EXP;

std::array<LivelinessCheck, NUM_CA_LAYERS> liveliness_checks;

void LivelinessCheck::reset() {
  min_cells_ = W * H;
}

bool LivelinessCheck::update(uint16_t num_cells) {
  min_cells_ = std::min(min_cells_, num_cells);

  if (num_cells > min_cells_ + LIVELINESS_DELTA) {
    min_cells_ += LIVELINESS_DELTA;

    liveliness_ += (
      (LIVELINESS_MAX - std::min(liveliness_, LIVELINESS_MAX))
    ) >> (LIVELINESS_MAX_EXP - LIVELINESS_INC_EXP);

    return true;
  } else if (num_cells > 0) {
    liveliness_ = std::max(1, liveliness_ - 1);
  } else {
    liveliness_ = 0;
  }

  return false;
}
