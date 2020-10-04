#pragma once

#include "entity.h"

struct Player : Entity
{
  virtual void think(Control const& s) = 0;
  virtual float health() = 0;
  virtual void addUpgrade(int upgrade) = 0;
  virtual void addEnergy(float amount) = 0;
};

enum
{
  UPGRADE_KEY1 = 2,
  UPGRADE_KEY2 = 4,
  UPGRADE_KEY3 = 8,
  UPGRADE_KEY4 = 16,
};

