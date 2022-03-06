#ifndef UTILS_H
#define UTILS_H

#include <cstdint>

constexpr int W = 80;
constexpr int H = 64;

constexpr int num_ca_layers = 4;

typedef void (*UpdateFunction)();
typedef void (*DrawFunction)();

// Defined in .ino file. Declared here for convenience
extern UpdateFunction updateFunction;
extern DrawFunction drawFunction;

int countBits(int val);

extern uint16_t expand[16];
void init_expand();

void assertFailed(const char *function, const char *file, int lineNo, const char *expression);

#define assertTrue(condition) \
if (!(condition)) { \
  assertFailed(__func__, __FILE__, __LINE__, #condition); \
}

#endif
