#include "Flower.h"

#include <Gamebuino-Meta.h>
#undef min
#undef max

#include <algorithm>

#include "Sfx.h"
#include "Utils.h"

constexpr int flower_grow_cycle = 90;

// The expand table is based on the strange bit-ordering used by the CA.
// The table below reverses the 4-bit nibbles. The table could be avoided
// by redefining the flowerData array, but that makes it harder to update
// or modify it.
const uint8_t reverse_nibble[] = {
  0x0, // 0x0 0000 - 0000
  0x8, // 0x1 0001 - 1000
  0x4, // 0x2 0010 - 0100
  0xc, // 0x3 0011 - 1100
  0x2, // 0x4 0100 - 0010
  0xa, // 0x5 0101 - 1010
  0x6, // 0x6 0110 - 0110
  0xe, // 0x7 0111 - 1110
  0x1, // 0x8 1000 - 0001
  0x9, // 0x9 1001 - 1001
  0x5, // 0xa 1010 - 0101
  0xd, // 0xb 1011 - 1101
  0x3, // 0xc 1100 - 0011
  0xb, // 0xd 1101 - 1011
  0x7, // 0xe 1110 - 0111
  0xf, // 0xf 1111 - 1111
};

// Flower definitions.
// There are 11 flowers, each of size 16x16 1-bit pixels
const uint8_t flowerData[] = {
  // Flower 0
  0x00, 0x00,
  0x00, 0x00,
  0x00, 0x00,
  0x00, 0x00,
  0x03, 0x80,
  0x04, 0x40,
  0x08, 0x20,
  0x08, 0x20,
  0x08, 0x20,
  0x04, 0x40,
  0x03, 0x80,
  0x00, 0x00,
  0x00, 0x00,
  0x00, 0x00,
  0x00, 0x00,
  0x00, 0x00,
  // Flower 1
  0x00, 0x00,
  0x00, 0x00,
  0x00, 0x00,
  0x01, 0x00,
  0x03, 0x80,
  0x07, 0xc0,
  0x0c, 0x60,
  0x1c, 0x70,
  0x0c, 0x60,
  0x07, 0xc0,
  0x03, 0x80,
  0x01, 0x00,
  0x00, 0x00,
  0x00, 0x00,
  0x00, 0x00,
  0x00, 0x00,
  // Flower 2
  0x00, 0x00,
  0x00, 0x00,
  0x00, 0x00,
  0x03, 0x80,
  0x04, 0x40,
  0x08, 0x20,
  0x11, 0x10,
  0x12, 0x90,
  0x11, 0x10,
  0x08, 0x20,
  0x04, 0x40,
  0x03, 0x80,
  0x00, 0x00,
  0x00, 0x00,
  0x00, 0x00,
  0x00, 0x00,
  // Flower 3
  0x00, 0x00,
  0x00, 0x00,
  0x01, 0x00,
  0x03, 0x80,
  0x07, 0xc0,
  0x08, 0x20,
  0x19, 0x30,
  0x3a, 0xb8,
  0x19, 0x30,
  0x08, 0x20,
  0x07, 0xc0,
  0x03, 0x80,
  0x01, 0x00,
  0x00, 0x00,
  0x00, 0x00,
  0x00, 0x00,
  // Flower 4
  0x00, 0x00,
  0x00, 0x00,
  0x03, 0x80,
  0x04, 0x40,
  0x04, 0x40,
  0x18, 0x30,
  0x21, 0x08,
  0x22, 0x88,
  0x21, 0x08,
  0x18, 0x30,
  0x04, 0x40,
  0x04, 0x40,
  0x03, 0x80,
  0x00, 0x00,
  0x00, 0x00,
  0x00, 0x00,
  // Flower 5
  0x00, 0x00,
  0x01, 0x00,
  0x03, 0x80,
  0x05, 0x40,
  0x04, 0x40,
  0x18, 0x30,
  0x21, 0x08,
  0x72, 0x9c,
  0x21, 0x08,
  0x18, 0x30,
  0x04, 0x40,
  0x05, 0x40,
  0x03, 0x80,
  0x01, 0x00,
  0x00, 0x00,
  0x00, 0x00,
  // Flower 6
  0x00, 0x00,
  0x03, 0x80,
  0x00, 0x00,
  0x05, 0x40,
  0x06, 0xc0,
  0x18, 0x30,
  0x49, 0x24,
  0x52, 0x94,
  0x49, 0x24,
  0x18, 0x30,
  0x06, 0xc0,
  0x05, 0x40,
  0x00, 0x00,
  0x03, 0x80,
  0x00, 0x00,
  0x00, 0x00,
  // Flower 7
  0x01, 0x00,
  0x01, 0x00,
  0x00, 0x00,
  0x05, 0x40,
  0x07, 0xc0,
  0x1b, 0xb0,
  0x0d, 0x60,
  0xde, 0xf6,
  0x0d, 0x60,
  0x1b, 0xb0,
  0x07, 0xc0,
  0x05, 0x40,
  0x00, 0x00,
  0x01, 0x00,
  0x01, 0x00,
  0x00, 0x00,
  // Flower 8
  0x00, 0x00,
  0x00, 0x00,
  0x02, 0x80,
  0x05, 0x40,
  0x00, 0x00,
  0x10, 0x10,
  0x20, 0x08,
  0x10, 0x10,
  0x20, 0x08,
  0x10, 0x10,
  0x00, 0x00,
  0x05, 0x40,
  0x02, 0x80,
  0x00, 0x00,
  0x00, 0x00,
  0x00, 0x00,
  // Flower 9
  0x00, 0x00,
  0x00, 0x00,
  0x03, 0x80,
  0x03, 0x80,
  0x00, 0x00,
  0x00, 0x00,
  0x30, 0x18,
  0x30, 0x18,
  0x30, 0x18,
  0x00, 0x00,
  0x00, 0x00,
  0x03, 0x80,
  0x03, 0x80,
  0x00, 0x00,
  0x00, 0x00,
  0x00, 0x00,
  // Flower 10
  0x00, 0x00,
  0x01, 0x00,
  0x02, 0x80,
  0x02, 0x80,
  0x01, 0x00,
  0x00, 0x00,
  0x30, 0x18,
  0x48, 0x24,
  0x30, 0x18,
  0x00, 0x00,
  0x01, 0x00,
  0x02, 0x80,
  0x02, 0x80,
  0x01, 0x00,
  0x00, 0x00,
  0x00, 0x00
};

void populate(uint8_t* buf, int len) {
  // Populate array
  for (int i = len; --i >= 0; ) {
    buf[i] = i;
  }

  // Randomly shuffle elements
  for (int i = 0; i < len; ++i) {
    int src = i + random(len - i);
    int tmp = buf[i];
    buf[i] = buf[src];
    buf[src] = tmp;
  }
}

void Flower::init() {
  cycle_ = 0;
  grow_step_ = random(flower_grow_cycle);
  populate(colors_, num_flower_colors);
  populate(sprites_, num_flower_frames);
}

void Flower::update() {
  if (random(256) < 128) {
    if (++grow_step_ == flower_grow_cycle) {
      grow_step_ = 0;
      ++cycle_;

      gb.sound.fx(aliveSfx[colors_[
        cycle_ % num_flower_colors
      ]]);
    }
  }
}

void Flower::draw(int x, int y) {
  // Only support aligned drawing into uint16_t destination buffer
  assertTrue(x % 4 == 0);

  // Draw three layers
  for (int i = std::min(3, (int)cycle_); --i >= 0; ) {
    int sprite_index = sprites_[(cycle_ - i) % num_flower_frames];
    int color_shift = colors_[(cycle_ - i) % num_flower_colors];

    const uint8_t *src_p = &flowerData[sprite_index * 32];
    uint16_t *dst_p = &gb.display._buffer[(W * y + x) / 4];
    const uint8_t *end_p = src_p + 32;
    while (src_p < end_p) {
      *dst_p |= expand[reverse_nibble[*src_p >> 4 & 0xf]] << color_shift;
      dst_p += 1;
      *dst_p |= expand[reverse_nibble[*src_p >> 0 & 0xf]] << color_shift;
      dst_p += 1;
      src_p += 1;

      *dst_p |= expand[reverse_nibble[*src_p >> 4 & 0xf]] << color_shift;
      dst_p += 1;
      *dst_p |= expand[reverse_nibble[*src_p >> 0 & 0xf]] << color_shift;
      dst_p += W / 4 - 3;
      src_p += 1;
    }
  }
}
