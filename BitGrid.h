constexpr int bitsPerUnit = 32;
constexpr int unitsPerRow = (80 + bitsPerUnit - 1) / bitsPerUnit;
constexpr int bitGridSize = 64 * unitsPerRow;

class BitGrid {
  uint32_t _data[bitGridSize];

public:
  bool get(int x, int y) {
    return (_data[indexOf(x, y)] >> (x % bitsPerUnit)) & 0x1;
  }
  bool set(int x, int y) {
    _data[indexOf(x, y)] |= (0x1 << (x % bitsPerUnit));
  }
  bool clear(int x, int y) {
    _data[indexOf(x, y)] &= ~(0x1 << (x % bitsPerUnit));
  }
  
private:
  int indexOf(int x, int y) {
    return x/bitsPerUnit + y*unitsPerRow;
  }
};
