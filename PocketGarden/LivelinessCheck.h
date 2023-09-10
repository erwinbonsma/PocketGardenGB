#pragma once

#include <Gamebuino-Meta.h>

#include "LifeCa.h"

class LivelinessCheck {
  const Gamebuino_Meta::TuneSpec* sfx_;
  uint16_t min_cells_;
  uint16_t liveliness_;

public:
  void init(const Gamebuino_Meta::TuneSpec* sfx) { sfx_ = sfx; }
  void reset();

  uint16_t liveliness() const { return liveliness_; }

  bool update(uint16_t num_cells);
};

extern std::array<LivelinessCheck, num_ca_layers> liveliness_checks;
