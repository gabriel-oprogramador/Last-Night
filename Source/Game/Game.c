#include "GT/GameModule.h"
#include "GameFramework.h"
#include "Game.h"

extern FLevel FLevelOneOpen();

static FLevel currentLevel;

void GameInit() {
  currentLevel = FLevelOneOpen();
}

void GameStart() {
  currentLevel.OnStart();
}

void GameUpdate(float DeltaTime) {
  if(FIsKeyPressed(KEY_ESCAPE)) {
    FCloseWindow();
  }

  if(FIsKeyPressed(KEY_F11)) {
    FToggleFullscreen();
  }

  currentLevel.OnUpdate(DeltaTime);
}

void GameStop() {
  currentLevel.OnStop();
}
