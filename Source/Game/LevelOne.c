#include "Game.h"
#include "GameFramework.h"

static USprite* SPlayerSpr;

static void LevelInit() {
  FCaptureMouse(true);
  SPlayerSpr = PlayerCreate();
}

static void LevelStart() {
}

static void LevelUpdate(float DeltaTime) {
  PlayerController(SPlayerSpr);

  USpriteUpdateArena();
  USpriteRenderArena();
}

static void LevelStop() {
}

FLevel FLevelOneOpen() {
  FLevel lv = {0};
  lv.OnInit = &LevelInit;
  lv.OnStart = &LevelStart;
  lv.OnUpdate = &LevelUpdate;
  lv.OnStop = &LevelStop;
  lv.OnInit();
  return lv;
}
