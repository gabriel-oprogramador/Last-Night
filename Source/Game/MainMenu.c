#include "Game.h"
#include "GameFramework.h"

static void LevelInit() {
}

static void LevelStart() {
}

static void LevelUpdate(float DeltaTime) {
}

static void LevelStop() {
}

FLevel FMainMenuOpen() {
  FLevel level = {0};
  level.OnInit = &LevelInit;
  level.OnStart = &LevelStart;
  level.OnUpdate = &LevelUpdate;
  level.OnStop = &LevelStop;
  return level;
}
