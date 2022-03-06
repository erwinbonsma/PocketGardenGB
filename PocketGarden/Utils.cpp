#include "Utils.h"

#include <Gamebuino-Meta.h>

int countBits(int val) {
  int num_bits = 0;

  while (val) {
    if (val & 0x1) {
      ++num_bits;
    }
    val >>= 1;
  }

  return num_bits;
}

uint16_t expand[16];

void init_expand() {
  for (int i = 0; i < 16; ++i) {
    uint16_t x = i;
    x = (x | x <<  6) & 0x0303;
    x = (x >> 1 | x << 4) & 0x1111;
    expand[i] = x;
  }
}

void assertFailed(const char *function, const char *file, int lineNo, const char *expression) {
  if (SerialUSB) {
    SerialUSB.println("=== ASSERT FAILED ===");
    SerialUSB.println(function);
    SerialUSB.println(file);
    SerialUSB.println(lineNo, DEC);
    SerialUSB.println(expression);
    SerialUSB.flush();
  }

  while (1) {
    if (gb.update()) {
      gb.display.clear();
      gb.display.setColor(INDEX_RED);
      gb.display.println("Assert failed:");
      gb.display.println(function);
      gb.display.println(file);
      gb.display.println(lineNo, DEC);
      gb.display.println(expression);
    }
  }
}
