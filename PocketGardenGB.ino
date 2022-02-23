#include <Gamebuino-Meta.h>

#include "BitGrid.h"
#include "Utils.h"

UpdateFunction updateFunction;
DrawFunction drawFunction;

uint32_t data[H * 3];
BitGrid bit_grid(data, 3 * 32, H);
int cx, cy;

LifeCa ca;

void testUpdate() {
  if (gb.buttons.pressed(BUTTON_LEFT)) {
    cx = (cx + W - 1) % W;
  }
  if (gb.buttons.pressed(BUTTON_RIGHT)) {
    cx = (cx + 1) % W;
  }
  if (gb.buttons.pressed(BUTTON_UP)) {
    cy = (cy + H - 1) % H;
  }
  if (gb.buttons.pressed(BUTTON_DOWN)) {
    cy = (cy + 1) % H;
  }
  if (gb.buttons.pressed(BUTTON_A)) {
    if (bit_grid.get(cx, cy)) {
      bit_grid.clear(cx, cy);
    } else {
      bit_grid.set(cx, cy);
    }
  }
  if (gb.buttons.pressed(BUTTON_B)) {
    bit_grid.reset();
  }

  ca.step();
}

void testDraw() {
  gb.display.clear();

  ca.draw();
}

void setup() {
  gb.begin();

  cx = 0;
  cy = 0;
  updateFunction = testUpdate;
  drawFunction = testDraw;

  ca.reset();
}

void loop() {
  while(!gb.update());

  updateFunction();
  drawFunction();
}
