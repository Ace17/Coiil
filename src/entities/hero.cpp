// Copyright (C) 2018 - Sebastien Alaiwan
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.

#include <algorithm>

#include "base/scene.h"
#include "base/util.h"

#include "collision_groups.h"
#include "entities/move.h"
#include "entities/player.h"
#include "entity.h"
#include "hero.h"
#include "models.h"
#include "sounds.h"
#include "toggle.h"

auto const JUMP_SPEED = 0.012;
auto const WALK_SPEED = 0.005f;
auto const MAX_HORZ_SPEED = 0.02f;
auto const HURT_DELAY = 500;
auto const STAIR_CLIMB = 0.5;

static auto const NORMAL_SIZE = Size(0.7, 0.7, 0.7);

template<typename T>
T lerp(T a, T b, float alpha)
{
  return a * (1 - alpha) + b * alpha;
}

struct Hero : Player, Damageable
{
  Hero()
  {
    size = NORMAL_SIZE;
  }

  void enter() override
  {
    respawnPoint = pos;
  }

  virtual void onDraw(View* view) const override
  {
    auto r = Actor(pos, MDL_INVRECT);
    r.scale = size;
    r.focus = true;
    r.orientation = orientation;

    if(0) // hide debug box
      view->sendActor(r);

    view->setCameraPos(pos + size * 0.5, orientation);
  }

  void think(Control const& c) override
  {
    control = c;
  }

  float health() override
  {
    return clamp(life / 31.0f, 0.0f, 1.0f);
  }

  virtual void addUpgrade(int upgrade) override
  {
    upgrades |= upgrade;
    blinking = 2000;
    life = 31;
  }

  void computeVelocity(Control c)
  {
    airMove(c);

    vel.x = clamp(vel.x, -MAX_HORZ_SPEED, MAX_HORZ_SPEED);
    vel.y = clamp(vel.y, -MAX_HORZ_SPEED, MAX_HORZ_SPEED);
    vel.z = clamp(vel.z, -MAX_HORZ_SPEED, MAX_HORZ_SPEED);
  }

  void airMove(Control c)
  {
    auto const forward = orientation.rotate(Vector3f(1, 0, 0));
    auto const left = orientation.rotate(Vector3f(0, 1, 0));

    Vector wantedVel = Vector(0, 0, 0);

    if(c.backward)
      wantedVel -= forward * WALK_SPEED;

    if(c.forward)
      wantedVel += forward * WALK_SPEED;

    if(c.left)
      wantedVel += left * WALK_SPEED;

    if(c.right)
      wantedVel -= left * WALK_SPEED;

    if(c.run)
      wantedVel = wantedVel * 2.0;

    vel = lerp(vel, wantedVel, 0.005);

    if(abs(vel.x) < 0.00001)
      vel.x = 0;

    if(abs(vel.y) < 0.00001)
      vel.y = 0;

    if(abs(vel.z) < 0.00001)
      vel.z = 0;
  }

  virtual void tick() override
  {
    decrement(blinking);
    decrement(hurtDelay);

    if(hurtDelay || life <= 0)
      control = Control {};

    const auto deltaVert = control.look_vert;
    const auto deltaHorz = control.look_horz;

    auto const up = Vector3f(0, 0, 1);
    orientation = orientation * Quaternion::rotation(up, -deltaHorz * 0.3);

    auto const left = Vector3f(0, 1, 0);
    orientation = orientation * Quaternion::rotation(left, deltaVert * 0.3);

    orientation = orientation.normalized();

    if(decrement(respawnDelay))
    {
      pos = respawnPoint;
      life = 31;
      blinking = 2000;
    }

    computeVelocity(control);

    physics->moveBody(this, Vector3f(0, 0, STAIR_CLIMB));
    slideMove(physics, this, vel);
    physics->moveBody(this, Vector3f(0, 0, -STAIR_CLIMB));

    decrement(debounceLanding);
    decrement(debounceUse);

    if(control.use && debounceUse == 0)
    {
      debounceUse = 200;

      // look in front of us for a body to switch,
      // and switch it.
      auto const forward = orientation.rotate(Vector3f(1, 0, 0));
      Box box = getBox();
      auto body = physics->traceBox(box, forward, this).blocker;

      if(auto switchable = dynamic_cast<Switchable*>(body))
        switchable->onSwitch();
    }

    if(control.restart)
      onDamage(10000);

    collisionGroup = CG_PLAYER;

    if(!blinking)
      collisionGroup |= CG_SOLIDPLAYER;
  }

  virtual void onDamage(int amount) override
  {
    if(life <= 0)
      return;

    if(!blinking)
    {
      life -= amount;

      if(life < 0)
      {
        die();
        return;
      }

      hurtDelay = HURT_DELAY;
      blinking = 2000;
      game->playSound(SND_HURT);
    }
  }

  void die()
  {
    game->playSound(SND_DIE);
    respawnDelay = 2000;
    game->textBox("game over");
  }

  int debounceLanding = 0;
  int debounceUse = 0;
  Quaternion orientation = Quaternion::fromEuler(0, 0, 0);
  Toggle jumpbutton, firebutton;
  int hurtDelay = 0;
  int life = 31;
  Control control {};

  int respawnDelay = 0;
  Vector respawnPoint;
  Vector vel;

  int upgrades = 0;
};

std::unique_ptr<Player> makeHero()
{
  return make_unique<Hero>();
}

