#include <Gamebuino-Meta.h>

#include "BitGrid.h"
#include "Utils.h"
#include "LifeCa.h"

UpdateFunction updateFunction;
DrawFunction drawFunction;

int cx, cy;
bool paused = true;

LifeCa cas[4];

void displayCpuLoad() {
  uint8_t cpu_load = gb.getCpuLoad();
  gb.display.setColor(cpu_load < 80 ? INDEX_GREEN : (cpu_load < 100 ? INDEX_YELLOW : INDEX_RED));
  gb.display.setCursor(1, 58);
  gb.display.printf("%d", cpu_load);
}

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
    LifeCa &ca = cas[0];
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
    for (int i = 0; i < 4; ++i) {
      cas[i].step();
    }
  }
}

void testDraw() {
  gb.display.clear();

  if (paused) {
    LifeCa &ca = cas[0];
    for (int y = 0; y < H; ++y) {
      for (int x = 0; x < W; ++x) {
        if (ca.get(x, y)) {
          gb.display.drawPixel(x, y, WHITE);
        }
      }
    }
    gb.display.drawPixel(cx, cy, YELLOW);
  } else {
    for (int i = 0; i < 4; ++i) {
      cas[i].draw(i);
    }
  }

  displayCpuLoad();
  //gb.display.printf("steps = %d", ca.numSteps());
}

void setup() {
  gb.begin();

  init_expand();

  cx = 0;
  cy = 0;
  updateFunction = testUpdate;
  drawFunction = testDraw;

  //ca.reset();
  for (int i = 0; i < 4; ++i) {
    cas[i].randomize();
  }
}

void loop() {
  while(!gb.update());

  updateFunction();
  drawFunction();
}
