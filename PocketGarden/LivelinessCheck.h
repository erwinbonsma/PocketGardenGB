#ifndef LIVELINESSCHECK_H
#define LIVELINESSCHECK_H

#include <Gamebuino-Meta.h>
#undef min
#undef max

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

#endif
