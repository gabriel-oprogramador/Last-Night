#include "Game.h"
#include "GameFramework.h"
#include "GT/Renderer.h"
/*#include "GT/Platform.h"*/

static USprite* SPlayerSpr;
static FTexture STextureLevel;
static AFloor SFloors[MAX_FLOOR];
static uint32 SCurretFloor = 0;

static void SpawnPlayer(USprite* Player);
static void AFloorInit(AFloor* Floor);
static void AFloorGenerate(AFloor* Floor);

static void LevelInit() {
  FCaptureMouse(true);
  AFloorInit(&SFloors[SCurretFloor]);
  SPlayerSpr = PlayerCreate();
}

static void LevelStart() {
}

static void LevelUpdate(float DeltaTime) {
  PlayerController(SPlayerSpr);

  if(FIsKeyPressed(KEY_TAB)) {
    AFloorGenerate(&SFloors[SCurretFloor]);
    SpawnPlayer(SPlayerSpr);
  }

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

static void AFloorInit(AFloor* Floor) {
  STextureLevel = FTextureLoadFile(FIND_ASSET("Images/Props.png"));
  USprite spr = USpriteCreate(NULL);
  spr.collider.bDraw = true;
  USpriteSetScale(&spr, VEC3_VALUE(3));
  USpriteSetTexture(&spr, STextureLevel);
  USpriteInitAnimator(&spr, 0, 0, 1, 3, 0.f);
  USpriteSetAnchorType(&spr, E_ANCHOR_LEFT_BOTTOM);

  float totalWidth = (spr.spriteRect.width * 2.f) * MAX_SPACE;
  float initialPos = (FGetScreenWidth() / 2.f) - (totalWidth / 2.f);
  float posX = initialPos;

  for(int32 j = 0; j < MAX_SPACE; j++) {
    Floor->types[j] = E_SPACE_WALL;
    Floor->spacesSpr[j] = USpriteAdd(&spr);
    USpriteSetLocation(Floor->spacesSpr[j], VEC3(posX, FGetScreenHeight() / 2.f, 0.f));
    posX += spr.spriteRect.width * 2.f;
  }
}

static void AFloorGenerate(AFloor* Floor) {
  USprite* spr = NULL;
  srand(time(NULL));

  for(int32 c = 0; c < MAX_SPACE; c++) {
    spr = Floor->spacesSpr[c];
    Floor->types[c] = rand() % E_SPACE_ENEMY_DOOR;
    switch((uint32)Floor->types[c]) {
      case E_SPACE_WALL: spr->animator.currentRow = E_PROP_WALL; break;
      case E_SPACE_CLOSED_DOOR: spr->animator.currentRow = E_PROP_CLOSED_DOOR; break;
      case E_SPACE_ENEMY_DOOR: spr->animator.currentRow = E_PROP_CLOSED_DOOR; break;
      case E_SPACE_IN_DOOR: spr->animator.currentRow = E_PROP_CLOSED_DOOR; break;
      case E_SPACE_OUT_DOOR: spr->animator.currentRow = E_PROP_CLOSED_DOOR; break;
    }
  }
  Floor->inDoorIndex = rand() % MAX_SPACE;
  do {
    Floor->outDoorIndex = rand() % MAX_SPACE;
  } while(Floor->outDoorIndex == Floor->inDoorIndex);

  Floor->types[Floor->inDoorIndex] = E_SPACE_IN_DOOR;
  spr = Floor->spacesSpr[Floor->inDoorIndex];
  spr->animator.currentRow = E_PROP_OPEN_DOOR;

  Floor->types[Floor->outDoorIndex] = E_SPACE_OUT_DOOR;
  spr = Floor->spacesSpr[Floor->outDoorIndex];
  spr->animator.currentRow = E_PROP_OPEN_DOOR;
}

static void SpawnPlayer(USprite* Player) {
  USprite* doorSpr = SFloors[SCurretFloor].spacesSpr[SFloors[SCurretFloor].outDoorIndex];
  FVector3 spawner = doorSpr->transform.location;
  spawner.x += doorSpr->spriteRect.width;
  USpriteSetLocation(SPlayerSpr, spawner);
}
