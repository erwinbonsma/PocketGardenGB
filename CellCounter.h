#include <array>

class LifeCa;

class CellCounter {
  std::array<uint8_t, 256> lookup_;

public:
  CellCounter();

  int countCells(const LifeCa& ca) const;
};
