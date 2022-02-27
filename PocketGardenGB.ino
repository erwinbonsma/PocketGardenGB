#include <Gamebuino-Meta.h>
#undef min
#undef max

#include <array>

#include "CellCounter.h"
#include "CellCountHistory.h"
#include "Utils.h"
#include "LifeCa.h"

UpdateFunction updateFunction;
DrawFunction drawFunction;

constexpr int max_step_wait = 6;
constexpr int num_view_modes = 6;

int cx, cy;
bool paused = true;
int view_mode = 4;
int step_wait = 5;
int num_steps = 0;

std::array<LifeCa, 4> cas;

CellCounter cell_counter;

void displayCpuLoad() {
  uint8_t cpu_load = gb.getCpuLoad();
  gb.display.setColor(cpu_load < 80 ? INDEX_GREEN : (cpu_load < 100 ? INDEX_YELLOW : INDEX_RED));
  gb.display.setCursor(1, 58);
  gb.display.printf("%d", cpu_load);
}

void testUpdate() {
  if (gb.buttons.pressed(BUTTON_LEFT)) {
    if (paused) {
      cx = (cx + W - 1) % W;
    } else {
      view_mode = (view_mode + num_view_modes - 1) % num_view_modes;
    }
  }
  if (gb.buttons.pressed(BUTTON_RIGHT)) {
    if (paused) {
      cx = (cx + 1) % W;
    } else {
      view_mode = (view_mode + 1) % num_view_modes;
    }
  }
  if (gb.buttons.pressed(BUTTON_UP)) {
    if (paused) {
      cy = (cy + H - 1) % H;
    } else {
      step_wait = std::max(step_wait - 1, 0);
    }
  }
  if (gb.buttons.pressed(BUTTON_DOWN)) {
    if (paused) {
      cy = (cy + 1) % H;
    } else {
      step_wait = std::min(step_wait + 1, max_step_wait);
    }
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
    if (gb.frameCount % (1 << step_wait) == 0) {
      int layer = 0;
      int history_index = num_steps % history_len;
      for (auto& ca : cas) {
        ca.step();
        cell_counts[layer][history_index] = cell_counter.countCells(ca);

        ++layer;
      }
      ++num_steps;
    }
  }
}

void testDraw() {
  gb.display.clear();

  if (view_mode <= 4) {
    int layer = 0;
    for (const auto& ca : cas) {
      if (view_mode == 4 || layer == view_mode) {
        ca.draw(layer);
      }
      ++layer;
    }
  } else {
    plotCellCounts(num_steps);
  }

  if (paused) {
    gb.display.drawPixel(cx, cy, YELLOW);
  }

  displayCpuLoad();
}

void setup() {
  gb.begin();
  gb.setFrameRate(30);

  init_expand();

  cx = 0;
  cy = 0;
  updateFunction = testUpdate;
  drawFunction = testDraw;

  //ca.reset();
  for (auto& ca : cas) {
    ca.randomize();
  }
}

void loop() {
  while(!gb.update());

  updateFunction();
  drawFunction();
}
