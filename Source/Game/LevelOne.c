#include "Game.h"
#include "GameFramework.h"

static APlayer SPlayer;
static AEnemy SEnemy[2];

static void LevelInit() {
  FCaptureMouse(true);
  SPlayer = APlayerCreate();
  SEnemy[0] = AEnemyCreate();
  SEnemy[1] = AEnemyCreate();

  USpriteSetLocation(SEnemy[1].sprite, VEC3(FGetScreenWidth()/2, FGetScreenHeight()/2, 2));
}

static void LevelStart() {
}

static void LevelUpdate(float DeltaTime) {
  float dt = DeltaTime;

  // Update...
  APlayerUpdate(&SPlayer, dt);

  USpriteUpdateArena();
  USpriteRenderArena();
}

static void LevelStop() {
  APlayerDestroy(&SPlayer);
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
