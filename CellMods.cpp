#include "CellMods.h"

#include "Utils.h"

void CellFinder::init(uint8_t target_layer) {
  target_layer_index_ = target_layer;
}

void CellFinder::reset() {
  target_identified_ = false;
}

bool CellFinder::findTarget(const LifeCa& ca) {
  int num_attempts = max_cell_find_attempts;

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

void CellFinder::update() {
  if (!target_identified_) {
    findTarget(cas[target_layer_index_]);
  }
}

void CellDecay::clearTargetArea(LifeCa& ca) {
  for (int dx = -1; dx <= 1; ++dx) {
    for (int dy = -1; dy <= 1; ++dy) {
      ca.clear(
        (target_pos_.x + dx + ca_width) % ca_width,
        (target_pos_.y + dy + ca_height) % ca_height
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

  ++destroy_count_;
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

  destroy_count_ = 0;
}

void CellDecay::update() {
  CellFinder::update();
  if (!target_identified_) {
    return;
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
    if (++count_ == num_decay_death_ticks) {
      destroyTarget();
    }
  } else {
    target_identified_ = false;
  }
}
