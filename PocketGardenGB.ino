#include <Gamebuino-Meta.h>
#undef min
#undef max

#include <array>
#include <limits>

#include "CellCounter.h"
#include "CellCountHistory.h"
#include "CellMods.h"
#include "Flower.h"
#include "Images.h"
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

constexpr int max_step_wait = 8; //6;
constexpr int num_view_modes = 6;

// How many frames to ignore key press after screen switch
constexpr int ignore_keys_wait = 2 * 30;

// How many frames to wait before starting a new game
constexpr int auto_play_wait = 15 * 30;

// How many frames to press A key to prematurely exit game
constexpr int exit_press_limit = 30;

// How many steps to wait until revive is allowed again
constexpr int min_revive_wait = 30;

int cx, cy;
bool paused = false;
uint8_t view_mode;
uint8_t step_wait;
uint8_t revive_cooldown;

uint32_t num_steps;
uint32_t num_revives;
uint32_t score;
uint32_t lo_score = std::numeric_limits<uint32_t>::max();
// Separate hi-score for auto-play (num_revives == 0) and interactive game
uint32_t hi_score[2];

constexpr int num_flowers = 14;
std::array<Flower, num_flowers> flowers;

CellCountHistory cell_count_history;
std::array<CellDecay, num_ca_layers> cell_decays;
std::array<CellMutation, num_ca_layers> cell_mutations;

void gameOver(bool ignore_lo_score = false);

void displayCpuLoad() {
  uint8_t cpu_load = gb.getCpuLoad();
  gb.display.setColor(cpu_load < 80 ? Color::green : (cpu_load < 100 ? Color::yellow : Color::red));
  gb.display.setCursor(1, 58);
  gb.display.printf("%d", cpu_load);

  if (revive_cooldown != 0) {
    gb.display.printf(" revived!");
  }
}

void revive() {
  // Init data pointers
  std::array<uint32_t*, num_ca_layers> ps;
  int layer = 0;
  for (auto& ca : cas) {
    ps[layer] = &ca.data_[units_per_row_ca];
    ++layer;
  }

  // Iterate over grid
  for (int i = units_per_row_ca * ca_height; --i >= 0; ) {
    uint32_t mask = (*ps[0]&*ps[1] | *ps[1]&*ps[2] | *ps[2]&*ps[3] | *ps[3]&*ps[0]);
    for (auto& p : ps) {
      *p |= mask;
      ++p;
    }
  }
}

void titleUpdate() {
  if (gb.buttons.pressed(BUTTON_A) || gb.buttons.pressed(BUTTON_B)) {
    startGame();
  }

  for (auto& flower : flowers) {
    flower.update();
  }
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
    if (paused) {
      LifeCa &ca = cas[0];
      if (ca.get(cx, cy)) {
        ca.clear(cx, cy);
      } else {
        ca.set(cx, cy);
      }
    }
  }
  if (gb.buttons.held(BUTTON_A, exit_press_limit)) {
    gameOver(true);
  }
  if (gb.buttons.pressed(BUTTON_B)) {
    if (!revive_cooldown) {
      revive();
      revive_cooldown = min_revive_wait;
      ++num_revives;
      cell_count_history.countCells();
      return;
    }
    //paused = !paused;
  }

  if (!paused) {
    if (gb.frameCount % (1 << step_wait) == 0) {
      int layer = 0;
      int history_index = num_steps % history_len;
      int total_cells = 0;
      for (auto& ca : cas) {
        if (cell_count_history.numCells(layer)) {
          ca.step();
          cell_decays[layer].update();
          cell_mutations[layer].update();
        }

        ++layer;
      }
      ++num_steps;

      if (cell_count_history.countCells() == 0) {
        gameOver();
      }
      if (revive_cooldown > 0) {
        --revive_cooldown;
      }
    }
  }
}

void gameOverUpdate() {
  ++num_steps;

  if (num_steps == auto_play_wait) {
    startGame();
  }

  if (num_steps > ignore_keys_wait) {
    if (gb.buttons.pressed(BUTTON_A) || gb.buttons.pressed(BUTTON_B)) {
      startGame();
    }
  }

  for (auto& flower : flowers) {
    flower.update();
  }
}

void titleDraw() {
  gb.display.clear();

  gb.display.drawImage(22, 20, titleImage);
  gb.display.setCursor(22, 40);
  gb.display.setColor(Color::brown);
  gb.display.print("by Eriban");

  int x = 0;
  int y = 0;
  for (int i = 0; i < 14; ++i) {
    flowers[i].draw(x*16, y*16);
    if (x==4) {
      x = 0;
      ++y;
    } else if (y % 3) {
      x = 4;
    } else {
      ++x;
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
    cell_count_history.plot();
  }

  gb.display.setColor(INDEX_WHITE);
  gb.display.setCursor(1, 1);
  if (view_mode < 4) {
    gb.display.printf("%d/%d/%d",
      cell_decays[view_mode].decayCount(),
      cell_mutations[view_mode].mutationCount(),
      cell_count_history.numCells(view_mode)
    );
  }

  if (paused) {
    gb.display.drawPixel(cx, cy, YELLOW);
  }

  displayCpuLoad();
}

void gameOverDraw() {
  gb.display.clear();

  gb.display.drawImage(27, 4, gameOverImage);
  for (int i = 0; i < 2; ++i) {
    flowers[i].draw(8 + i * 48, 4);
  }

  uint32_t total_decays = 0;
  for (const auto& decay : cell_decays) {
    total_decays += decay.decayCount();
  }
  uint32_t total_mutations = 0;
  for (const auto& mutation : cell_mutations) {
    total_mutations += mutation.mutationCount();
  }

  gb.display.setColor(Color::brown);
  uint8_t y = 26;
  gb.display.setCursor(18, y);
  gb.display.printf("Decays%8d", total_decays);
  y+= 6;

  gb.display.setCursor(6, y);
  gb.display.printf("Mutations%8d", total_mutations);
  y+= 6;

  if (num_revives > 0) {
    gb.display.setCursor(14, y);
    gb.display.printf("Revives%8d", num_revives);
    y+= 6;
  }

  gb.display.setCursor(22, y);
  gb.display.printf("Score%8d", score);
  y+= 10;

  bool auto_play = num_revives == 0;
  if (
    auto_play
    && lo_score != std::numeric_limits<uint32_t>::max()
    && lo_score != hi_score[auto_play]
  ) {
    gb.display.setColor(score == lo_score ? Color::red : Color::brown);
    gb.display.setCursor(10, y);
    gb.display.printf("Lo-score%8d", lo_score);
    y+= 6;
  }

  if (!auto_play || hi_score[auto_play] > lo_score) {
    gb.display.setColor(score == hi_score[auto_play] ? Color::green : Color::brown);
    gb.display.setCursor(10, y);
    gb.display.printf("Hi-score%8d", hi_score[auto_play]);
  }
}

void showTitle() {
  updateFunction = titleUpdate;
  drawFunction = titleDraw;

  num_steps = 0;
}

void startGame() {
  updateFunction = gameUpdate;
  drawFunction = gameDraw;

  int layer = 0;
  for (auto& ca : cas) {
    //ca.reset();
    ca.randomize();
    cell_decays[layer].reset();
    cell_mutations[layer].reset();
    ++layer;
  }
  cell_count_history.reset();

  view_mode = 4;
  num_steps = 0;
  num_revives = 0;
}

void gameOver(bool ignore_lo_score) {
  updateFunction = gameOverUpdate;
  drawFunction = gameOverDraw;

  score = num_steps;
  if (!ignore_lo_score) {
    lo_score = std::min(lo_score, score);
  }
  bool auto_play = num_revives == 0;
  hi_score[auto_play] = std::max(hi_score[auto_play], score);
  num_steps = 0;
}

void setup() {
  gb.begin();
  gb.setFrameRate(30);
  gb.display.setPalette(caColorPalette);

  init_expand();

  cx = 0;
  cy = 0;
  step_wait = 0;

  for (int i = 0; i < num_ca_layers; ++i) {
    cell_decays[i].init(i);
    cell_mutations[i].init(i);
  }

  for (auto& flower : flowers) {
    flower.init();
  }

  showTitle();
}

void loop() {
  while(!gb.update());

  updateFunction();
  drawFunction();
}
