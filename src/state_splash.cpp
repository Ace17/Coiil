// Copyright (C) 2018 - Sebastien Alaiwan
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.

// splash menu

#include "base/scene.h"
#include "base/view.h"
#include <memory>

#include "models.h" // MDL_SPLASH
#include "sounds.h" // SND_PAUSE
#include "state_machine.h"
#include "toggle.h"

static const float LIGHT_ON = 10;
static const float LIGHT_OFF = 0;

template<typename T>
T blend(T a, T b, float alpha)
{
  return a * (1 - alpha) + b * alpha;
}

struct SplashState : Scene
{
  SplashState(View* view_) : view(view_)
  {
    view->setAmbientLight(LIGHT_ON);
  }

  ////////////////////////////////////////////////////////////////
  // Scene: Game, seen by the engine

  Scene* tick(Control c) override
  {
    auto const FADE_TIME = 200;

    view->playMusic(1);

    if(!activated)
    {
      if(c.fire || c.jump || c.dash || c.start || c.use)
      {
        activated = true;
        view->playSound(SND_START);
      }
    }

    if(activated)
    {
      ++time;
      const float alpha = std::min(FADE_TIME, time) / float(FADE_TIME);
      const float light = blend(LIGHT_ON, LIGHT_OFF, alpha);
      view->setAmbientLight(light);

      if(time > FADE_TIME * 3 / 2)
        return createPlayingState(view);
    }

    return this;
  }

  void draw() override
  {
    const auto t = time * 0.001;
    view->setCameraPos(Vector3f(0, 0, 0), Quaternion::fromEuler(0, 0, 0));
    view->sendActor(Actor(Vector3f(1.5, 0, -t * t * 8), MDL_SPLASH));
  }

private:
  View* const view;
  bool activated = false;
  int time = 0;
};

Scene* createSplashState(View* view)
{
  return new SplashState(view);
}

