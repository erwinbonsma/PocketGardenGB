#pragma once

#include <cstdint>

constexpr int W = 80;
constexpr int H = 64;

typedef void (*UpdateFunction)();
typedef void (*DrawFunction)();

// Defined in .ino file. Declared here for convenience
extern UpdateFunction update_function;
extern DrawFunction draw_function;

int countBits(int val);

extern uint16_t expand[16];
void initExpand();

template <class Iter>
class Range {
  Iter b;
  Iter e;
public:
  Range(Iter b, Iter e) : b(b), e(e) {}

  Iter begin() { return b; }
  Iter end() { return e; }
};

template <class Container>
Range<typename Container::iterator>
make_range(Container& c, int b, int e) {
  return Range<typename Container::iterator> (c.begin() + b, c.begin() + e);
}


void assertFailed(const char *function, const char *file, int lineNo, const char *expression);

#define assertTrue(condition) \
if (!(condition)) { \
  assertFailed(__func__, __FILE__, __LINE__, #condition); \
}
