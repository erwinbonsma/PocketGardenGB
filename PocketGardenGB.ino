#include <Gamebuino-Meta.h>
#undef min
#undef max

#include <array>

#include "CellCounter.h"
#include "CellCountHistory.h"
#include "CellMods.h"
#include "Utils.h"
#include "LifeCa.h"

// Color palette chosen such that blends of CA cells is somewhat logical.
// - black is no cells, and white is all cells
// - The core layers colors are: darkblue, purple, green, darkgray
//   - These are the dark variants of the primary colors R, G and B plus a brightness
// - Mixing two colors:
//   - Mixing with darkgray makes the other color "lighter"
// - Mixing three colors:
//   - Results in four bright colors
//   - The resulting color is a lighter variant of the blend of two of the three colors
const Color caColorPalette[16] = {
  Color::black,
  Color::darkblue,     //  1: Layer 1
  Color::purple,       //  2: Layer 2
  Color::pink,         //  3: Layer 1+2
  Color::green,        //  4: Layer 3
  Color::brown,        //  5: Layer 1+3
  Color::orange,       //  6: Layer 2+3
  Color::gray,         //  7: Layer 1+2+3
  Color::darkgray,     //  8: Layer 4
  Color::blue,         //  9: Layer 1+4
  Color::red,          // 10: Layer 2+4
  Color::lightblue,    // 11: Layer 1+2+4
  Color::lightgreen,   // 12: Layer 3+4
  Color::beige,        // 13: Layer 1+3+4
  Color::yellow,       // 14: Layer 2+3+4
  Color::white,
};

UpdateFunction updateFunction;
DrawFunction drawFunction;

constexpr int max_step_wait = 6;
constexpr int num_view_modes = 6;

int cx, cy;
bool paused = false;
int view_mode = 4;
int step_wait = 5;
int num_steps = 0;

CellCounter cell_counter;
std::array<CellDecay, num_ca_layers> cell_decays;

void displayCpuLoad() {
  uint8_t cpu_load = gb.getCpuLoad();
  gb.display.setColor(cpu_load < 80 ? INDEX_GREEN : (cpu_load < 100 ? INDEX_YELLOW : INDEX_RED));
  gb.display.setCursor(1, 58);
  gb.display.printf("%d", cpu_load);
}

void gameUpdate() {
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
      int total_cells = 0;
      for (auto& ca : cas) {
        ca.step();

        int cell_count = cell_counter.countCells(ca);
        cell_counts[layer][history_index] = cell_count;
        if (cell_count > 0) {
          cell_decays[layer].update();
          total_cells += cell_count;
        }

        ++layer;
      }
      ++num_steps;
    }
  }
}

void gameDraw() {
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

  gb.display.setColor(INDEX_WHITE);
  gb.display.setCursor(1, 1);
  if (view_mode < 4) {
    gb.display.printf("%d/%d",
      cell_decays[view_mode].destroyCount(),
      cell_counts[view_mode][(num_steps - 1) % history_len]
    );
  }

  if (paused) {
    gb.display.drawPixel(cx, cy, YELLOW);
  }

  displayCpuLoad();
}

void startGame() {
  updateFunction = gameUpdate;
  drawFunction = gameDraw;

  int layer = 0;
  for (auto& ca : cas) {
    //ca.reset();
    ca.randomize();
    cell_decays[layer].reset();
    ++layer;
  }
}

void setup() {
  gb.begin();
  gb.setFrameRate(30);
  gb.display.setPalette(caColorPalette);

  init_expand();

  cx = 0;
  cy = 0;

  int layer = 0;
  for (auto& cell_decay : cell_decays) {
    cell_decay.init(layer);
    ++layer;
  }

  startGame();
}

void loop() {
  while(!gb.update());

  updateFunction();
  drawFunction();
}
