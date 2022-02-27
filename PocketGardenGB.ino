#include <Gamebuino-Meta.h>

#include "BitGrid.h"
#include "Utils.h"
#include "LifeCa.h"

UpdateFunction updateFunction;
DrawFunction drawFunction;

int cx, cy;
bool paused = true;

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
    if (ca.get(cx, cy)) {
      ca.clear(cx, cy);
    } else {
      ca.set(cx, cy);
    }
  }
  if (gb.buttons.pressed(BUTTON_B)) {
    paused = !paused;
    //ca.step();
  }

  if (!paused) {
    ca.step();
  }
}

void testDraw() {
  gb.display.clear();

  if (paused) {
    for (int y = 0; y < H; ++y) {
      for (int x = 0; x < W; ++x) {
        if (ca.get(x, y)) {
          gb.display.drawPixel(x, y, WHITE);
        }
      }
    }
  } else {
    ca.draw();
  }

  gb.display.drawPixel(cx, cy, YELLOW);
  gb.display.printf("steps = %d", ca.numSteps());

}

void setup() {
  gb.begin();

  init_expand();

  cx = 0;
  cy = 0;
  updateFunction = testUpdate;
  drawFunction = testDraw;

  //ca.reset();
  ca.randomize();
}

void loop() {
  while(!gb.update());

  updateFunction();
  drawFunction();
}
