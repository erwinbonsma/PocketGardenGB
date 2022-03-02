#ifndef CELLMODS_H
#define CELLMODS_H

#include <cstdint>

#include "LifeCa.h"

constexpr int max_cell_find_attempts = 32;
constexpr int num_decay_death_ticks = 16;
constexpr int avg_mutation_period = 512;

struct GridPos {
  uint8_t x;
  uint8_t y;
};

class CellFinder {

public:
  uint8_t targetLayerIndex() { return target_layer_index_; }
  bool isTargetIdentified() { return target_identified_; }

  void init(uint8_t target_layer);
  virtual void reset();

protected:
  uint8_t target_layer_index_;
  GridPos target_pos_;
  bool target_identified_;

  virtual bool findTarget(const LifeCa& ca);

  virtual void update();
};

class CellDecay : public CellFinder {
  uint16_t decay_count_;
  uint8_t mask_;
  uint8_t count_;

  void clearTargetArea(LifeCa& ca);
  void destroyTarget();

  bool findTarget(const LifeCa& ca) override;

public:
  uint16_t decayCount() const { return decay_count_; }
  uint8_t mask() const { return mask_; }

  void reset() override;
  void update() override;
};

class CellMutation : public CellFinder {
  bool mutate_;
  uint16_t mutation_count_;

  void mutateTarget();

public:
  uint16_t mutationCount() const { return mutation_count_; }

  void reset() override;
  void update() override;
};

#endif
