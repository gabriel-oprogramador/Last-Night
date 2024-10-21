#include "Game.h"
#include "GameFramework.h"
#include "GT/Renderer.h"
/*#include "GT/Platform.h"*/

AGameState GGameState = {.enemyDeaths = 0, .playerHealt = 4};

static FTexture STextureLevel;
static AFloor SFloor;
static USprite* SPlayerSpr;
static USprite* SEnemySpr;
static USprite* SUiStartLevel;
static USprite* SUiTitle;

static void SpawnPlayer(USprite* Player);
static void SpawnEnemy();
static void AFloorInit(AFloor* Floor);
static void AFloorGenerate(AFloor* Floor);
static void AFloorInDoorCollision(USprite* Self, USprite Other);
static void AFloorEnemyDoorCollision(USprite* Self, USprite Other);

static void LevelInit() {
  FCaptureMouse(true);
  AFloorInit(&SFloor);
  USprite ui = USpriteCreate(FIND_ASSET("Images/Start-Level.png"));
  SUiStartLevel = USpriteAdd(&ui);
  USpriteSetLocation(SUiStartLevel, VEC3(FGetScreenWidth() / 2.f, FGetScreenHeight() / 2.f + 200.f, 0));
  USpriteSetScale(SUiStartLevel, VEC3_VALUE(0.5));
  USpriteInitAnimator(SUiStartLevel, 0, 0, 1, 1, 0);
  ui = USpriteCreate(FIND_ASSET("Images/Last-Night.png"));
  SUiTitle = USpriteAdd(&ui);
  USpriteSetLocation(SUiTitle, VEC3(FGetScreenWidth() / 2.f, FGetScreenHeight() / 2.f - 350.f, 0));
  USpriteSetScale(SUiTitle, VEC3_VALUE(0.5));
  USpriteInitAnimator(SUiTitle, 0, 0, 1, 1, 0);

  SPlayerSpr = PlayerCreate();
  SEnemySpr = EnemyCreate();
  SPlayerSpr->bActive = false;
}

static void LevelStart() {
}

static void LevelUpdate(float DeltaTime) {
  if(FIsKeyPressed(KEY_F) && SPlayerSpr->bActive == false) {
    SUiStartLevel->bHidden = true;
    SPlayerSpr->bActive = true;
    AFloorGenerate(&SFloor);
    SpawnPlayer(SPlayerSpr);
    SEnemySpr->bHidden = true;
  }

  if(SEnemySpr->bHidden == false) {
    EnemyChasePlayer(SEnemySpr, SPlayerSpr);
  }
  PlayerController(SPlayerSpr);
  EnemyController(SEnemySpr);
  USpriteUpdateArena();
  USpriteRenderArena();

  if(GGameState.playerHealt <= 0) {
    SUiStartLevel->bHidden = false;
    GGameState.playerHealt = 4;
    GGameState.enemyDeaths = 0;
  }
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

// TODO:Fix bug with the width and height of the React sprite that returns half the real size, hence the 2 * width!
static void AFloorInit(AFloor* Floor) {
  srand(time(NULL));
  STextureLevel = FTextureLoadFile(FIND_ASSET("Images/Props.png"));
  USprite spr = USpriteCreate(NULL);
  spr.collider.bDraw = true;
  spr.tag = E_TAG_NONE;
  USpriteSetScale(&spr, VEC3_VALUE(3));
  USpriteSetTexture(&spr, STextureLevel);
  USpriteInitAnimator(&spr, 0, 0, 1, 4, 0.f);
  USpriteSetAnchorType(&spr, E_ANCHOR_LEFT_BOTTOM);
  spr.collider.type = E_WORLD_STATIC;
  spr.collider.bCollisionEnable = false;
  spr.collider.bDraw = false;

  float totalWidth = (spr.spriteRect.width * 2.f) * MAX_SPACE;
  float initialPos = (FGetScreenWidth() / 2.f) - (totalWidth / 2.f);
  float posX = initialPos;

  for(int32 j = 0; j < MAX_SPACE; j++) {
    Floor->spacesSpr[j] = USpriteAdd(&spr);
    USpriteSetLocation(Floor->spacesSpr[j], VEC3(posX, FGetScreenHeight() / 2.f, 0.f));
    posX += spr.spriteRect.width * 2.f;
  }
}

static void AFloorGenerate(AFloor* Floor) {
  USprite* spr = NULL;

  for(int32 c = 0; c < MAX_SPACE; c++) {
    spr = Floor->spacesSpr[c];
    spr->animator.currentRow = rand() % (E_PROP_CLOSED_DOOR + 1);
    spr->collider.bDraw = false;
    spr->collider.bCollisionEnable = false;
    spr->collider.onCollision = NULL;
    spr->collider.scale = VEC3(50, 100, 0);
    spr->collider.debugColor = COLOR_WHITE;
    spr->bActive = false;
    spr->tag = E_TAG_NONE;
  }

  Floor->outDoorIndex = rand() % MAX_SPACE;
  do {
    Floor->inDoorIndex = rand() % MAX_SPACE;
  } while(Floor->inDoorIndex == Floor->outDoorIndex);
  do {
    Floor->enemyDoorIndex = rand() % MAX_SPACE;
  } while(Floor->enemyDoorIndex == Floor->inDoorIndex || Floor->enemyDoorIndex == Floor->outDoorIndex);

  spr = Floor->spacesSpr[Floor->outDoorIndex];
  spr->animator.currentRow = E_PROP_OPEN_DOOR;

  spr = Floor->spacesSpr[Floor->inDoorIndex];
  spr->animator.currentRow = E_PROP_OPEN_DOOR;
  spr->collider.bDraw = true;
  spr->collider.bCollisionEnable = true;
  spr->collider.onCollision = (FOnCollisionCallback)&AFloorInDoorCollision;
  spr->collider.debugColor = COLOR_GREEN;
  spr->bActive = true;
  spr->tag = E_TAG_IN_DOOR;

  spr = Floor->spacesSpr[Floor->enemyDoorIndex];
  spr->animator.currentRow = E_PROP_CLOSED_ENEMY;
  spr->collider.bDraw = true;
  spr->collider.bCollisionEnable = true;
  spr->collider.scale = VEC3(150, 90, 0);
  spr->collider.onCollision = (FOnCollisionCallback)&AFloorEnemyDoorCollision;
  spr->collider.debugColor = COLOR_RED;
  spr->bActive = true;
  spr->tag = E_TAG_ENEMY_DOOR;
  SpawnEnemy();
}

static void SpawnPlayer(USprite* Player) {
  USprite* doorSpr = SFloor.spacesSpr[SFloor.outDoorIndex];
  FVector3 spawner = doorSpr->transform.location;
  spawner.x += doorSpr->spriteRect.width;
  USpriteSetLocation(Player, spawner);
}

static void SpawnEnemy() {
  USprite* doorSpr = SFloor.spacesSpr[SFloor.enemyDoorIndex];
  FVector3 spawner = doorSpr->transform.location;
  spawner.x += doorSpr->spriteRect.width;
  USpriteSetLocation(SEnemySpr, spawner);
}

static void AFloorInDoorCollision(USprite* Self, USprite Other) {
  if(Other.tag == E_TAG_PLAYER) {
    if(FIsKeyPressed(KEY_W)) {
      AFloorGenerate(&SFloor);
      SpawnPlayer(SPlayerSpr);
      EnemyDeath(SEnemySpr);
      if(GGameState.enemyDeaths >= 5) {
        GGameState.playerHealt = 0;
      }
    }
  }
}

static void AFloorEnemyDoorCollision(USprite* Self, USprite Other) {
  if(Other.tag == E_TAG_PLAYER) {
    Self->collider.bCollisionEnable = false;
    Self->animator.currentRow = E_PROP_OPEN_DOOR;
    Self->collider.debugColor = COLOR_CYAN;
    EnemyReset(SEnemySpr, VEC3_ZERO);
  }
}
