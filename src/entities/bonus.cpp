// Copyright (C) 2018 - Sebastien Alaiwan
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.

// KeyItem entity

#include <algorithm>
#include <cmath> // sin

#include "base/scene.h"
#include "base/util.h"

#include "collision_groups.h" // CG_PLAYER
#include "entities/player.h"
#include "entity.h"
#include "models.h"
#include "sounds.h"

namespace
{
struct KeyItem : Entity
{
  KeyItem(int modelAction_, int type_, char const* msg_)
  {
    modelAction = modelAction_;
    type = type_;
    msg = msg_;

    size = Size(2, 2, 2);
    solid = false;
    collisionGroup = 0;
    collidesWith = CG_PLAYER | CG_SOLIDPLAYER;
  }

  virtual void onDraw(View* view) const override
  {
    auto r = Actor(pos, MDL_BONUS);
    r.action = 1;
    r.ratio = 0;
    r.scale = size;
    view->sendActor(r);
  }

  virtual void tick() override
  {
    ++time;
  }

  void onCollide(Entity* other) override
  {
    if(dead)
      return;

    if(auto player = dynamic_cast<Player*>(other))
    {
      player->addUpgrade(type);
      game->playSound(SND_BONUS);
      game->textBox(msg);
      dead = true;
    }
  }

  int time = 0;
  int modelAction;
  int type;
  char const* msg;
};
}

std::unique_ptr<Entity> makeBonus(int visual, int upgradeType, char const* msg)
{
  return make_unique<KeyItem>(visual, upgradeType, msg);
}

