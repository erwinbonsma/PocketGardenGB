#include <Gamebuino-Meta.h>

#include "BitGrid.h"
#include "Utils.h"

UpdateFunction updateFunction;
DrawFunction drawFunction;

BitGrid bitGrid;
int cx, cy;

void testUpdate() {
  if (gb.buttons.pressed(BUTTON_LEFT)) {
    cx = (cx + 79) % 80;
  }
  if (gb.buttons.pressed(BUTTON_RIGHT)) {
    cx = (cx + 1) % 80;
  }
  if (gb.buttons.pressed(BUTTON_UP)) {
    cy = (cy + 63) % 64;
  }
  if (gb.buttons.pressed(BUTTON_DOWN)) {
    cy = (cy + 1) % 64;
  }
  if (gb.buttons.pressed(BUTTON_A)) {
    bitGrid.clear(cx, cy);
  }
  if (gb.buttons.pressed(BUTTON_B)) {
    bitGrid.set(cx, cy);
  }
}

void testDraw() {
  gb.display.clear();

  gb.display.setColor(WHITE);
  for (int y = 0; y < 64; ++y) {
    for (int x = 0; x < 80; ++x) {
      if (bitGrid.get(x, y)) {
        gb.display.drawPixel(x, y);
      }
    }
  }

  gb.display.drawPixel(cx, cy, YELLOW);
}

void setup() {
  gb.begin();

  cx = 0;
  cy = 0;
  updateFunction = testUpdate;
  drawFunction = testDraw;
}    

void loop() {
  while(!gb.update());

  updateFunction();
  drawFunction();
}
