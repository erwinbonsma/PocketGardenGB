#include "BitGrid.h"

#include <cstring>

void BitGrid::reset() {
  memset(data_, 0, num_rows_ * units_per_row_ * BYTES_PER_UNIT);
}
