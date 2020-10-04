// Copyright (C) 2018 - Sebastien Alaiwan
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.

// EnergyCell entity

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
struct EnergyCell : Entity
{
  EnergyCell()
  {
    size = UnitSize * 0.5;
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
    r.effect = Effect::Blinking;
    r.orientation = Quaternion::fromEuler(0, yaw, pitch);
    view->sendActor(r);
  }

  virtual void tick() override
  {
    ++time;
    yaw += 0.0011;
    pitch += 0.0039;
  }

  void onCollide(Entity* other) override
  {
    if(dead)
      return;

    if(auto player = dynamic_cast<Player*>(other))
    {
      player->addEnergy(0.4);
      game->playSound(SND_BONUS);
      game->textBox("Got flashlight battery");
      dead = true;
    }
  }

  int time = 0;
  int type;
  char const* msg;

  float yaw = 0;
  float pitch = 0;
};
}

std::unique_ptr<Entity> makeEnergyCell()
{
  return make_unique<EnergyCell>();
}

#include "entity_factory.h"
static auto const reg1 = registerEntity("bonus", [] (IEntityConfig*) { return makeEnergyCell(); });

