#include "CellMods.h"

#include <Gamebuino-Meta.h>

#include "Utils.h"

constexpr int MAX_CELL_FIND_ATTEMPTS = 16;
constexpr int NUM_DECAY_DEATH_TICKS = 16;
constexpr int AVG_MUTATION_PERIOD = 512;

void CellFinder::init(uint8_t target_layer) {
  target_layer_index_ = target_layer;
}

void CellFinder::reset() {
  target_identified_ = false;
}

bool CellFinder::findTarget(const LifeCa& ca) {
  int num_attempts = MAX_CELL_FIND_ATTEMPTS;

  while (num_attempts >= 0) {
    int x = random(W);
    int y = random(H);

    if (ca.get(x, y)) {
      target_pos_.x = x;
      target_pos_.y = y;
      target_identified_ = true;
      return true;
    }

    --num_attempts;
  }

  return false;
}

bool CellFinder::update() {
  if (!target_identified_) {
    findTarget(cas[target_layer_index_]);
  }
  return false;
}

void CellDecay::clearTargetArea(LifeCa& ca) {
  for (int dx = -1; dx <= 1; ++dx) {
    for (int dy = -1; dy <= 1; ++dy) {
      ca.clear(
        (target_pos_.x + dx + CA_WIDTH) % CA_WIDTH,
        (target_pos_.y + dy + CA_HEIGHT) % CA_HEIGHT
      );
    }
  }
}

void CellDecay::destroyTarget() {
  int layer = 0;
  for (auto &ca : cas) {
    if (
      // Always clear target layer
      layer == target_layer_index_
      || (
        // Also clear static cells in other layers
        mask_ & (0x1 << layer)
        && (
          // As long as layer is a direct neighbour
          layer % 2 != target_layer_index_ % 2
          // Or connected via a static layer
          || countBits(mask_) > 2
        )
      )
    ) {
      clearTargetArea(ca);
    }

    ++layer;
  }

  ++decay_count_;
  target_identified_ = false;
}

bool CellDecay::findTarget(const LifeCa& ca) {
  if (CellFinder::findTarget(ca)) {
    mask_ = 0xf;
    count_ = 1;
    return true;
  }

  return false;
}

void CellDecay::reset() {
  CellFinder::reset();

  decay_count_ = 0;
}

bool CellDecay::update() {
  CellFinder::update();
  if (!target_identified_) {
    return false;
  }

  int layer = 0;
  for (auto &ca : cas) {
    if (
      mask_ & 0x1 << layer
      && !ca.get(target_pos_.x, target_pos_.y)
    ) {
      mask_ &= ~(0x1 << layer);
    }
    ++layer;
  }

  if (mask_ & 0x1 << target_layer_index_) {
    if (++count_ == NUM_DECAY_DEATH_TICKS) {
      destroyTarget();
      return true;
    }
  } else {
    target_identified_ = false;
  }

  return false;
}


void CellMutation::mutateTarget() {
  LifeCa &ca = cas[target_layer_index_];
  int offset = random(4);
  for (int i = 0; i < 4; ++i) {
    int dir = (i + offset) % 4;
    int dx = 2 * (dir % 2) - 1;
    int dy = 2 * (dir / 2) - 1;
    int x = (target_pos_.x + dx + CA_WIDTH) % CA_WIDTH;
    int y = (target_pos_.y + dy + CA_HEIGHT) % CA_HEIGHT;

    if (!ca.get(x, y)) {
      ca.set(x, y);
      ++mutation_count_;
      return;
    }
  }
}

void CellMutation::reset() {
  CellFinder::reset();

  mutate_ = false;
  mutation_count_ = 0;
}

bool CellMutation::update() {
  if (random(AVG_MUTATION_PERIOD) == 0) {
    mutate_ = true;
  }

  if (mutate_) {
    CellFinder::update();
    if (target_identified_) {
      mutateTarget();
      target_identified_ = false;
      mutate_ = false;

      return true;
    }
  }

  return false;
}
