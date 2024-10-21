#include "Game.h"
#include "GameFramework.h"
#include "GT/Renderer.h"

static FTexture STexture;
static USprite* STrident;
static bool SbValid = false;
static bool SbUsedTrident = false;

static void EnemyTridentUpdate(USprite* Self);
static void EnemyOnCollision(USprite* Self, USprite* Other);

USprite* EnemyCreate() {
  if(SbValid == false) {
    SbValid = true;
    STexture = FTextureLoadFile(FIND_ASSET("Images/Enemy.png"));
  }
  USprite comp = USpriteCreate(NULL);
  USprite* enemySpr = USpriteAdd(&comp);
  USpriteSetTexture(enemySpr, STexture);
  USpriteSetLocation(enemySpr, VEC3(FGetScreenWidth() - 300.f, FGetScreenHeight() / 2.f, 0.f));
  USpriteSetScale(enemySpr, VEC3_VALUE(3));
  USpriteInitAnimator(enemySpr, 0, 0, 2, 7, 0.2f);
  USpriteSetAnchorType(enemySpr, E_ANCHOR_BOTTOM_CENTER);
  USpriteFlipX(enemySpr);

  enemySpr->bInvertible = true;
  enemySpr->bHidden = true;
  enemySpr->tag = E_TAG_ENEMY;
  enemySpr->collider.bCollisionEnable = false;
  enemySpr->collider.scale = VEC3(45, 69, 0);
  enemySpr->collider.offset = VEC3(0, 29, 0);
  enemySpr->collider.type = E_WORLD_DYNAMIC;
  enemySpr->collider.onCollision = (FOnCollisionCallback)&EnemyOnCollision;

  comp = USpriteCreate(FIND_ASSET("Images/Trident.png"));
  STrident = USpriteAdd(&comp);
  USpriteSetLocation(STrident, VEC3(FGetScreenWidth() - 300.f, FGetScreenHeight() / 2.f, 0.f));
  USpriteSetScale(STrident, VEC3_VALUE(2));
  USpriteInitAnimator(STrident, 0, 0, 2, 1, 0.2f);
  USpriteSetAnchorType(STrident, E_ANCHOR_CENTER_CENTER);
  USpriteFlipX(STrident);

  STrident->bHidden = true;
  STrident->bInvertible = true;
  STrident->tag = E_TAG_TRIDENT;
  STrident->collider.bDraw = true;
  STrident->collider.bCollisionEnable = false;
  STrident->collider.scale = VEC3(20, 20, 0);
  STrident->collider.offset = VEC3(0, 0, 0);
  STrident->collider.type = E_WORLD_DYNAMIC;
  STrident->collider.onCollision = NULL;
  STrident->movement.maxSpeed = 600;

  return enemySpr;
}

void EnemyReset(USprite* Self, FVector3 Location) {
  SbUsedTrident = false;
  STrident->bHidden = false;
  STrident->bActive = true;
  STrident->collider.bCollisionEnable = true;
  Self->bActive = true;
  Self->collider.bCollisionEnable = true;
  Self->bHidden = false;
}

void EnemyDeath(USprite* Self){
  Self->bHidden = true;
  Self->collider.bCollisionEnable = false;
  STrident->bHidden = true;
  STrident->collider.bCollisionEnable = false;
}

void EnemyController(USprite* Self) {
  float speed = USpriteGetVelocityLength(Self);
  uint32 animRow = E_ENEMY_STATE_IDLE;
  if(speed > 0.0f) {
    animRow += E_ENEMY_STATE_WALK;
  }

  if(Self->bActive == false) {
    animRow = E_ENEMY_STATE_DEAD + GGameState.enemyDeaths - 1;
  }

  Self->animator.currentRow = animRow;
}

void EnemyChasePlayer(USprite* Self, USprite* Player) {
  EnemyTridentUpdate(Self);
  if(Self->bActive == false) { return; }

  FVector3 enemyLoc = USpriteGetLocation(Self);
  FVector3 playerLoc = USpriteGetLocation(Player);

  float offset = 80.0f;

  FVector3 direction = {
      playerLoc.x - enemyLoc.x,
      playerLoc.y - enemyLoc.y,
      0};

  float distance = FVector3Length(direction);

  if(distance > offset * 3 && SbUsedTrident == false) {
    STrident->movement.direction = VEC3_ZERO;
    STrident->movement.direction.x = Self->movement.direction.x;
    FVector3 loc = Self->anchor.location;
    loc.x += Self->movement.direction.x * 33;
    loc.y -= 30.f;
    USpriteSetLocation(STrident, loc);
    SbUsedTrident = true;
  }

  if(distance > offset) {
    direction = FVector3Normalize(direction);
    USpriteAddMovement(Self, direction);
  }
}

static void EnemyTridentUpdate(USprite* Self) {
  if(STrident->collider.bCollisionEnable == false) {
    STrident->collider.bCollisionEnable = false;
    STrident->bHidden = true;
    return;
  }

  if(SbUsedTrident) {
    USpriteAddMovement(STrident, STrident->movement.direction);
  } else {
    FVector3 loc = Self->anchor.location;
    loc.x += Self->movement.direction.x * 33;
    loc.y -= 30.f;
    STrident->movement.direction = Self->movement.direction;
    USpriteSetLocation(STrident, loc);
  }
}

static void EnemyOnCollision(USprite* Self, USprite* Other) {
  if(Other->tag == E_TAG_PROJECTILE) {
    Other->bHidden = true;
    STrident->bHidden = true;
    STrident->collider.bCollisionEnable = false;
    Self->collider.bCollisionEnable = false;
    Self->animator.currentRow = E_ENEMY_STATE_DEAD;
    Self->bActive = false;
    GGameState.enemyDeaths++;
  }
}
