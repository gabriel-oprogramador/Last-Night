#include "Game.h"
#include "Components.h"
#include "GameFramework.h"

/*static uint32 SAnimState = 0;*/

static struct {
  bool bCrouch;
  bool bEquiped;
  bool bDead;
} SAnimState;

static USprite* SProjectile;

USprite* PlayerCreate() {
  USprite spr = USpriteCreate(FIND_ASSET("Images/Player.png"));
  USpriteSetScale(&spr, VEC3_VALUE(3));
  USpriteSetLocation(&spr, VEC3(0 + 300, FGetScreenHeight() / 2, 0));
  USpriteInitAnimator(&spr, 0, 0, 2, 7, 0.2f);
  USpriteSetAnchorType(&spr, E_ANCHOR_BOTTOM_CENTER);
  spr.animator.bLoop = true;

  spr.bInvertible = true;
  spr.collider.bDraw = false;
  spr.collider.type = E_WORLD_DYNAMIC;
  spr.collider.scale = VEC3(35, 70, 1);
  spr.collider.offset = VEC3(0, 26, 0);
  spr.collider.bCollisionEnable = true;
  spr.movement.maxSpeed = 512.f;
  USprite* player = USpriteAdd(&spr);

  spr = USpriteCreate(FIND_ASSET("Images/Projectile.png"));
  USpriteSetScale(&spr, VEC3_VALUE(1));
  USpriteSetLocation(&spr, VEC3(0 + 300, FGetScreenHeight() / 2, 0));
  USpriteInitAnimator(&spr, 0, 0, 2, 1, 0.2f);
  USpriteSetAnchorType(&spr, E_ANCHOR_CENTER_CENTER);
  spr.animator.bLoop = true;

  spr.bHidden = true;
  spr.bInvertible = true;
  spr.collider.bDraw = true;
  spr.collider.type = E_WORLD_DYNAMIC;
  spr.collider.scale = VEC3(5, 3, 1);
  spr.collider.offset = VEC3(0, 0, 0);
  spr.collider.bCollisionEnable = true;
  spr.movement.maxSpeed = 600.f;
  SProjectile = USpriteAdd(&spr);

  return player;
}

static void PlayerAnimController(USprite* Self) {
  float speed = FVector3Length(Self->movement.velocity);
  uint32 row = 0;

  uint32 base = (SAnimState.bEquiped) ? E_PLAYER_STATE_FIRE : E_PLAYER_STATE_NORMAL;
  uint32 overlay = 0;

  if(SAnimState.bCrouch) {
    overlay = E_PLAYER_STATE_CROUCH;
  }

  if(speed > 0.1f) {
    row = base + E_PLAYER_STATE_WALK;
  } else {
    row = base + overlay;
  }

  if(SAnimState.bDead) {
    row = E_PLAYER_STATE_DEAD;
    USpriteStopMovement(Self);
  }

  Self->animator.currentRow = row;
}

static void ProjectileUpdate() {
  if(SProjectile->bHidden == false) {
    FVector3 location = USpriteGetLocation(SProjectile);
    USpriteAddMovement(SProjectile, SProjectile->movement.direction);
    if(location.x <= 0 || location.x >= FGetScreenWidth()) { SProjectile->bHidden = true; }
  }
}

static void PlayerFire(USprite* Self) {
  if(SProjectile->bHidden == true) {
    SProjectile->bHidden = false;
    FVector3 spawnLocation = Self->anchor.location;
    SProjectile->movement.direction = Self->movement.direction;
    spawnLocation.x += Self->movement.direction.x * 64.f;
    spawnLocation.y += 2;
    if(SAnimState.bCrouch) { spawnLocation.y += 30.f; }
    USpriteSetLocation(SProjectile, spawnLocation);
  }
}

void PlayerController(USprite* Self) {
  if(FIsKeyRepeat(KEY_MOUSE_RIGHT)) {
    SAnimState.bEquiped = true;
  }
  if(FIsKeyRelease(KEY_MOUSE_RIGHT)) {
    SAnimState.bEquiped = false;
  }

  if(FIsKeyPressed(KEY_MOUSE_LEFT) && SAnimState.bEquiped) {
    PlayerFire(Self);
  }

  if(FIsKeyRepeat(KEY_A)) {
    USpriteAddMovement(Self, VEC3(-1, 0, 0));
  }
  if(FIsKeyRepeat(KEY_D)) {
    USpriteAddMovement(Self, VEC3(1, 0, 0));
  }
  if(FIsKeyRepeat(KEY_SPACE)) {
    SAnimState.bCrouch = true;
    USpriteStopMovement(Self);
  }
  if(FIsKeyRelease(KEY_SPACE)) {
    SAnimState.bCrouch = false;
  }

  // TODO: Death test
  if(FIsKeyPressed(KEY_F)) {
    SAnimState.bDead = !SAnimState.bDead;
  }

  PlayerAnimController(Self);
  ProjectileUpdate();
}
