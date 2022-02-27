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
