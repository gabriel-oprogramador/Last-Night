#include "Game.h"
#include "Components.h"
#include "GameFramework.h"

static void PlayerFire(USprite* Self);
static void ProjectileUpdate();
static void PlayerOnCollision(USprite* Self, USprite* Other);

static USprite* SProjSpr;

USprite* PlayerCreate() {
  USprite comp = USpriteCreate(FIND_ASSET("Images/Player.png"));
  USprite* playerSpr = USpriteAdd(&comp);

  USpriteSetLocation(playerSpr, VEC3(300.f, FGetScreenHeight() / 2.f, 0.f));
  USpriteSetScale(playerSpr, VEC3_VALUE(3));
  USpriteInitAnimator(playerSpr, 0, 0, 2, 7, 0.2f);
  USpriteSetAnchorType(playerSpr, E_ANCHOR_BOTTOM_CENTER);

  playerSpr->tag = E_TAG_PLAYER;
  playerSpr->bInvertible = true;
  playerSpr->movement.maxSpeed = 512;

  playerSpr->collider.bDraw = true;
  playerSpr->collider.bCollisionEnable = true;
  playerSpr->collider.type = E_WORLD_DYNAMIC;
  playerSpr->collider.scale = VEC3(29, 69, 0);
  playerSpr->collider.offset = VEC3(0, 29, 0);
  playerSpr->collider.onCollision = (FOnCollisionCallback)&PlayerOnCollision;

  comp = USpriteCreate(FIND_ASSET("Images/Projectile.png"));
  SProjSpr = USpriteAdd(&comp);
  USpriteSetLocation(SProjSpr, VEC3(300.f, FGetScreenHeight() / 2.f, 0.f));
  USpriteSetScale(SProjSpr, VEC3_VALUE(1));
  USpriteInitAnimator(SProjSpr, 0, 0, 2, 1, 0.2f);
  USpriteSetAnchorType(SProjSpr, E_ANCHOR_CENTER_CENTER);

  SProjSpr->bActive = false;
  SProjSpr->bHidden = true;
  SProjSpr->bInvertible = true;
  SProjSpr->collider.bDraw = true;
  SProjSpr->tag = E_TAG_PROJECTILE;
  SProjSpr->movement.maxSpeed = 550.f;
  SProjSpr->collider.bCollisionEnable = true;
  SProjSpr->collider.type = E_WORLD_DYNAMIC;
  SProjSpr->collider.scale = VEC3(5, 2, 0);
  SProjSpr->collider.offset = VEC3(0, 0, 0);

  return playerSpr;
}

void PlayerController(USprite* Self) {
  uint32 animRow = E_PLAYER_STATE_NORMAL;

  if(FIsKeyRepeat(KEY_MOUSE_RIGHT)) {
    animRow = E_PLAYER_STATE_FIRE;
  }

  if(FIsKeyPressed(KEY_MOUSE_LEFT) && animRow == E_PLAYER_STATE_FIRE) {
    PlayerFire(Self);
  }

  if(FIsKeyRepeat(KEY_A)) {
    USpriteAddMovement(Self, VEC3(-1, 0, 0));
    Self->movement.direction.x = -1;
  }
  if(FIsKeyRepeat(KEY_D)) {
    USpriteAddMovement(Self, VEC3(1, 0, 0));
    Self->movement.direction.x = 1;
  }

  if(FIsKeyRepeat(KEY_SPACE)) {
    USpriteStopMovement(Self);
    animRow += E_PLAYER_STATE_CROUCH;
    Self->collider.scale.y = 50.f;
    Self->collider.offset.y = 45.f;
    Self->transform.bDirty = true;
  } else if(FIsKeyRelease(KEY_SPACE)) {
    Self->collider.scale.y = 69.f;
    Self->collider.offset.y = 29.f;
    Self->transform.bDirty = true;
  }

  if(USpriteGetVelocityLength(Self) > 0.0f) {
    animRow += E_PLAYER_STATE_WALK;
  }

  if(Self->bActive == false) {
    animRow = E_PLAYER_STATE_DEAD;
  }
  Self->animator.currentRow = animRow;
  ProjectileUpdate();
  FVector3 location = USpriteGetLocation(Self);
  if(location.x - Self->spriteRect.width / 2.f < 0) {
    location.x = 0 + Self->spriteRect.width / 2.f;
    USpriteSetLocation(Self, location);
  } else if(location.x + Self->spriteRect.width / 2.f > FGetScreenWidth()) {
    location.x = FGetScreenWidth() - Self->spriteRect.width / 2.f;
    USpriteSetLocation(Self, location);
  }
}

static void PlayerFire(USprite* Self) {
  if(SProjSpr->bActive == false) {
    FVector3 spawnLocation = Self->anchor.location;
    SProjSpr->bHidden = false;
    SProjSpr->bActive = true;
    SProjSpr->movement.direction = Self->movement.direction;
    spawnLocation.x += Self->movement.direction.x * 64;
    spawnLocation.y += (Self->animator.currentRow == E_PLAYER_STATE_FIRE + E_PLAYER_STATE_CROUCH) ? 33.f : 2.f;
    USpriteSetLocation(SProjSpr, spawnLocation);
  }
}

static void ProjectileUpdate() {
  FVector3 position = VEC3_ZERO;
  if(SProjSpr->bActive) {
    position = USpriteGetLocation(SProjSpr);
    USpriteAddMovement(SProjSpr, SProjSpr->movement.direction);
    if(position.x <= 0 || position.x >= FGetScreenWidth()) {
      SProjSpr->bActive = false;
      SProjSpr->bHidden = true;
    }
  }
}

static void PlayerOnCollision(USprite* Self, USprite* Other) {
  if(Other->tag == E_TAG_ENEMY || Other->tag == E_TAG_TRIDENT) {
    GGameState.playerHealt--;
    Self->bActive = false;
    Other->bActive = false;
    EnemyDeath(Other);
  }
}
