#include "Game.h"
#include "GameFramework.h"

static AProjectile SProjectile = {0};
static float SDt = 0;

static void APlayerController(APlayer* Self, float DeltaTime);
static void APlayerOnCollision(USprite* Self, USprite* Other);

APlayer APlayerCreate() {
  APlayer player = {0};
  player.healt = 100.f;
  player.speed = 512.f;
  player.bEquiped = false;
  player.direction = VEC3(1, 0, 0);
  USprite spr = USpriteCreate(FIND_ASSET("Images/Player.png"));
  uint32 id = USpriteAdd(&spr);
  player.sprite = USpriteGet(id);

  USpriteSetLocation(player.sprite, VEC3(300.f, FGetScreenHeight() / 2.f, 0.f));
  USpriteSetScale(player.sprite, VEC3_VALUE(3));
  USpriteInitAnimator(player.sprite, 0, 0, 2, 7, 0.2f);
  USpriteSetAnchorType(player.sprite, E_ANCHOR_BOTTOM_CENTER);
  player.sprite->collider.bCollisionEnable = true;
  player.sprite->collider.onCollision = (FOnCollisionCallback)&APlayerOnCollision;
  player.sprite->collider.bDraw = true;
  player.sprite->collider.scale = VEC3(40, 100, 0);
  player.sprite->collider.type = E_WORLD_DYNAMIC;
  player.sprite->tag = E_TAG_PLAYER;

  SProjectile = AProjectileCreate();

  return player;
}

void APlayerUpdate(APlayer* Self, float DeltaTime) {
  SDt = DeltaTime;
  APlayerController(Self, DeltaTime);
  AProjectileUpdate(&SProjectile, DeltaTime);
}

void APlayerDestroy(APlayer* Self) {
}

static void InternalAddMovement(APlayer* Self) {
  FVector3 Direction = FVector3Normalize(Self->direction);
  FVector3 location = USpriteGetLocation(Self->sprite);
  location.x += Direction.x * Self->speed * SDt;
  location.y += Direction.y * Self->speed * SDt;
  USpriteSetLocation(Self->sprite, location);
  if(Direction.y == 0) {
    FVector3 scale = USpriteGetScale(Self->sprite);
    scale.x = fabsf(scale.x) * Direction.x;
    USpriteSetScale(Self->sprite, scale);
  }
}

static void InternalFire(APlayer* Self) {
  if(SProjectile.sprite->bHidden == true) {
    SProjectile.sprite->bHidden = false;
    SProjectile.direction = Self->direction;
    FVector3 spawnPos = Self->sprite->anchor.location;
    spawnPos.x += Self->direction.x * 65;
    if(Self->bCrouch) { spawnPos.y += 30.f; }
    USpriteSetLocation(SProjectile.sprite, spawnPos);
  }
}

static void APlayerController(APlayer* Self, float DeltaTime) {
  Self->sprite->animator.currentRow = E_PLAYER_NORMAL_IDLE;

  if(FIsKeyRepeat(KEY_MOUSE_RIGHT)) {
    Self->sprite->animator.currentRow = E_PLAYER_FIRE_IDLE;
  }
  if(FIsKeyPressed(KEY_MOUSE_LEFT) && Self->sprite->animator.currentRow == E_PLAYER_FIRE_IDLE) {
    InternalFire(Self);
  }

  if(FIsKeyRepeat(KEY_A)) {
    Self->direction = VEC3(-1, 0, 0);
    InternalAddMovement(Self);
    Self->sprite->animator.currentRow++;
  } else if(FIsKeyRepeat(KEY_D)) {
    Self->direction = VEC3(1, 0, 0);
    InternalAddMovement(Self);
    Self->sprite->animator.currentRow++;
  } else if(FIsKeyRepeat(KEY_LEFT_CONTROL)) {
    Self->bCrouch = true;
    Self->sprite->animator.currentRow += 2;
  } else {
    Self->bCrouch = false;
  }
}

static void APlayerOnCollision(USprite* Self, USprite* Other) {
  /*if(Other->tag == E_TAG_ENEMY) {*/
    /*FVector3 direction = FVector3Sub(Self->collider.location, Other->collider.location);*/
    /*direction = FVector3Normalize(direction);*/
    /*GT_LOGTEMP(LOG_ALERT, "Direction => X:%.2f Y:%.2f Z:%.2f", direction.x, direction.y, direction.z);*/
    /*[>float distance = Self->collider.location.x<]*/
    /*FVector3 location = USpriteGetLocation(Self);*/
    /*location.x += direction.x */
    /*USpriteSetLocation(Self, location);*/
  /*}*/
}
