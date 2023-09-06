#include <Gamebuino-Meta.h>
#undef min
#undef max

#include <array>
#include <limits>

#include "CellCountHistory.h"
#include "CellMods.h"
#include "Flower.h"
#include "Images.h"
#include "LifeCa.h"
#include "LivelinessCheck.h"
#include "Music.h"
#include "Sfx.h"
#include "Utils.h"

#define DEVELOPMENT

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

constexpr int num_ui_languages = 1;
const MultiLang exit_hint_txt[1] = {
  {LANG_EN, "Hold B to exit"},
};
const MultiLang cooling_down_txt[1] = {
  {LANG_EN, "Cannot revive yet"},
};

constexpr int32_t VMAJOR = 1;
constexpr int32_t VMINOR = 1;

constexpr uint16_t SAVEINDEX_VMAJOR = 0;
constexpr uint16_t SAVEINDEX_VMINOR = 1;
constexpr uint16_t SAVEINDEX_LOSCORE = 2;
constexpr uint16_t SAVEINDEX_HISCORE = 3;

UpdateFunction updateFunction;
DrawFunction drawFunction;

constexpr int max_step_wait = 6;
constexpr int ini_step_wait = 5;
constexpr int num_view_modes = 6;

// How many frames to ignore key press after screen switch
constexpr int ignore_keys_wait = 2 * 30;

// How many frames to wait before starting a new game
constexpr int auto_play_wait = 15 * 30;

// How many frames to press A key to prematurely exit game
constexpr int exit_press_limit = 30;

// The number of frames popups are shown
constexpr int popup_duration = 60;

// How many steps to wait until revive is allowed again
constexpr int min_revive_wait = 32;

constexpr int lights_revive_ticks_bin_size = min_revive_wait / 8;
constexpr int lights_revive_cells_bin_size = 256 / 8;

uint8_t view_mode;
uint8_t target_step_wait;
uint8_t step_wait;
uint8_t revive_cooldown;

uint32_t num_steps;
uint32_t num_revives;
int revive_cell_delta;
uint32_t score;

// Store lo score signed (to avoid type-casts when saving). It will never get big.
int32_t lo_score = std::numeric_limits<int32_t>::max();
// Separate hi-score for auto-play (num_revives == 0) and interactive game
uint32_t hi_score[2];

constexpr int num_flowers = 14;
std::array<Flower, num_flowers> flowers;

CellCountHistory cell_count_history;
std::array<CellDecay, num_ca_layers> cell_decays;
std::array<CellMutation, num_ca_layers> cell_mutations;
std::array<LivelinessCheck, num_ca_layers> liveliness_checks;

bool show_lo_score() {
  bool auto_play = num_revives == 0;
  return (
    auto_play
    && lo_score != std::numeric_limits<int32_t>::max()
    && lo_score != hi_score[auto_play]
  );
}

bool show_hi_score() {
  bool auto_play = num_revives == 0;
  return !auto_play || hi_score[auto_play] != lo_score;
}

void load_hi_scores() {
  // Major version changes when storage format changes.
  // Minor version changes when changes impact scoring.
  // Either way, previously stored scores should be ignored.
  if (
    gb.save.get(SAVEINDEX_VMAJOR) != VMAJOR ||
    gb.save.get(SAVEINDEX_VMINOR) != VMINOR
  ) {
    gb.save.set(SAVEINDEX_VMAJOR, VMAJOR);
    gb.save.set(SAVEINDEX_VMINOR, VMINOR);

    gb.save.set(SAVEINDEX_LOSCORE, lo_score);
    for (int i = 0; i < 2; ++i ) {
      gb.save.set(SAVEINDEX_HISCORE, (int32_t)hi_score[i]);
    }
  } else {
    lo_score = gb.save.get(SAVEINDEX_LOSCORE);
    for (int i = 0; i < 2; ++i ) {
      hi_score[i] = (uint32_t)gb.save.get(SAVEINDEX_HISCORE + i);
    }
  }
}

void switch_view_mode(int delta) {
  bool skip_combined_view = cell_count_history.numEmptyLayers() >= num_ca_layers - 1;
  do {
    view_mode = (view_mode + num_view_modes + delta) % num_view_modes;
  } while (
    // Skip empty layers
    (view_mode < num_ca_layers && cell_count_history.numCells(view_mode) == 0)
    // Skip combined view when only one layer remains
    || (skip_combined_view && view_mode == num_ca_layers)
  );
}

void gameOver(bool ignore_lo_score = false);
void startGame();

void displayCpuLoad() {
  uint8_t cpu_load = gb.getCpuLoad();
  gb.display.setColor(cpu_load < 80 ? Color::green : (cpu_load < 100 ? Color::yellow : Color::red));
  gb.display.setCursor(1, 58);
  gb.display.printf("%d", cpu_load);
}

// Use LEDs to indicate how successful a revive was, and that next revive cannot yet be started
void showReviveCooldown() {
  int num_leds = (revive_cooldown + lights_revive_ticks_bin_size - 1) / lights_revive_ticks_bin_size;

  int num_green_leds = (revive_cell_delta + lights_revive_cells_bin_size - 1) / lights_revive_cells_bin_size;
  for (int i = num_leds; --i >= 0; ) {
    gb.lights.drawPixel(
      i % 2, i / 2, i < num_green_leds ? Color::green : Color::red
    );
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

void updateGarden() {
  int layer = 0;
  int history_index = num_steps % history_len;
  int total_cells = 0;
  for (auto& ca : cas) {
    uint16_t cell_count = cell_count_history.numCells(layer);
    if (cell_count) {
      bool visible = view_mode >= num_ca_layers || layer == view_mode;

      ca.step();

      if (liveliness_checks[layer].update(cell_count) && visible) {
//        gb.sound.fx(aliveSfx[layer]);
      } else if (liveliness_checks[layer].liveliness() < 100) {
        if (cell_decays[layer].update() && visible) {
//          gb.sound.fx(decaySfx);
        }
        cell_mutations[layer].update();
      }
    }

    ++layer;
  }
  ++num_steps;
}

void gameUpdate() {
  if (gb.buttons.pressed(BUTTON_LEFT)) {
    switch_view_mode(-1);
  }
  if (gb.buttons.pressed(BUTTON_RIGHT)) {
    switch_view_mode(1);
  }
  if (gb.buttons.pressed(BUTTON_UP)) {
    target_step_wait = std::max(target_step_wait - 1, 0);
  }
  if (gb.buttons.pressed(BUTTON_DOWN)) {
    target_step_wait = std::min(target_step_wait + 1, max_step_wait);
  }
  if (gb.buttons.pressed(BUTTON_B)) {
    gb.gui.popup(exit_hint_txt, popup_duration);
  } else if (gb.buttons.held(BUTTON_B, exit_press_limit)) {
    gameOver(true);
  }

  if (gb.buttons.pressed(BUTTON_A)) {
    if (!revive_cooldown) {
      int cells_before = cell_count_history.totalCells();

      revive();
      ++num_revives;

      revive_cell_delta = cell_count_history.countCells() - cells_before;
      assertTrue(revive_cell_delta >= 0);
      if (!revive_cell_delta) {
//        gb.sound.fx(decaySfx);
      }

      revive_cooldown = min_revive_wait;

      // Exit here to skip CA update (to reduce CPU load)
      return;
    }
  }

  if (
    gb.buttons.pressed(BUTTON_LEFT) || gb.buttons.pressed(BUTTON_RIGHT) ||
    gb.buttons.pressed(BUTTON_UP) || gb.buttons.pressed(BUTTON_DOWN) ||
    gb.buttons.pressed(BUTTON_A)
  ) {
    gb.gui.hidePopup();
  }

  if (gb.frameCount % (1 << step_wait) != 0) return;

  if (target_step_wait != step_wait && gb.frameCount % 32 == 0) {
    if (step_wait < target_step_wait) {
      ++step_wait;
    } else {
      --step_wait;
    }
  }

  updateGarden();

  if (cell_count_history.countCells() == 0) {
    gameOver();
  }
  if (revive_cooldown > 0) {
    --revive_cooldown;
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

  for (auto& flower : make_range(flowers, 0, 2)) {
    flower.update();
  }
}

void titleDraw() {
  gb.display.clear();

  gb.display.drawImage(22, 20, titleImage);
  gb.display.setCursor(22, 40);
  gb.display.setColor(Color::darkgray);
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
  gb.lights.clear();

  if (view_mode <= num_ca_layers) {
    int layer = 0;
    for (const auto& ca : cas) {
      if (view_mode == num_ca_layers || layer == view_mode) {
        ca.draw(layer);
      }
      ++layer;
    }
  } else {
    cell_count_history.plot();
  }

  showReviveCooldown();

#ifdef DEVELOPMENT
  if (view_mode < num_ca_layers) {
    gb.display.setColor(INDEX_WHITE);
    gb.display.setCursor(1, 1);
    gb.display.printf("%d/%d/%d/%d",
      cell_decays[view_mode].decayCount(),
      cell_mutations[view_mode].mutationCount(),
      cell_count_history.numCells(view_mode),
      liveliness_checks[view_mode].liveliness()
    );
  }

  displayCpuLoad();
#endif
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

  if (show_lo_score()) {
    gb.display.setColor(score == lo_score ? Color::red : Color::brown);
    gb.display.setCursor(10, y);
    gb.display.printf("Lo-score%8d", lo_score);
    y+= 6;
  }

  bool auto_play = num_revives == 0;
  if (show_hi_score()) {
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
    liveliness_checks[layer].reset();
    ++layer;
  }
  cell_count_history.reset();

  view_mode = 4;
  num_steps = 0;
  num_revives = 0;
  step_wait = ini_step_wait;
}

void gameOver(bool ignore_lo_score) {
  updateFunction = gameOverUpdate;
  drawFunction = gameOverDraw;

  score = num_steps;
  bool improved_lo_score = false;
  if (!ignore_lo_score) {
    if (score < lo_score) {
      lo_score = (int32_t)score;
      gb.save.set(SAVEINDEX_LOSCORE, lo_score);
      improved_lo_score = true;
    }
  }

  bool auto_play = num_revives == 0;
  bool improved_hi_score = false;
  if (score > hi_score[auto_play]) {
    hi_score[auto_play] = score;
    gb.save.set(SAVEINDEX_HISCORE + auto_play, (int32_t)score);
    improved_hi_score = true;
  }

  if (improved_hi_score && show_hi_score()) {
    gb.sound.playSong(levelHiSong);
  } else if (improved_lo_score && show_lo_score()) {
    gb.sound.playSong(levelLoSong);
  } else {
    gb.sound.fx(gameOverSfx);
  }

  num_steps = 0;

  // Stop showing "Hold B to exit" pop-up
  gb.gui.hidePopup();

  // Clear lights
  gb.lights.clear();
}

void setup() {
  gb.begin();
  gb.setFrameRate(30);
  gb.display.setPalette(caColorPalette);

  init_expand();

  target_step_wait = 0;

  for (int i = 0; i < num_ca_layers; ++i) {
    cell_decays[i].init(i);
    cell_mutations[i].init(i);
    liveliness_checks[i].init(aliveSfx[i]);
  }

  for (auto& flower : flowers) {
    flower.init();
  }

  load_hi_scores();

  showTitle();

  gb.sound.playSong(gardenOfLifeSong, true);
}

void loop() {
  while(!gb.update());

  updateFunction();
  drawFunction();
}
