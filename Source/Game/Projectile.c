#include "Game.h"
#include "GT/Renderer.h"
#include "GameFramework.h"

static FTexture STexture = {0};
static bool SbValid = false;

AProjectile AProjectileCreate() {
  if(SbValid == false) {
    STexture = FTextureLoadFile((FIND_ASSET("Images/Projectile.png")));
    SbValid = true;
  }
  AProjectile proj = {0};
  proj.bActive = false;
  proj.speed = 700.f;
  proj.direction = VEC3(1, 0, 0);
  USprite spr = USpriteCreate(NULL);
  int32 id = USpriteAdd(&spr);
  proj.sprite = USpriteGet(id);
  USpriteSetTexture(proj.sprite, STexture);
  USpriteSetScale(proj.sprite, VEC3_VALUE(3));
  USpriteSetAnchorType(proj.sprite, E_ANCHOR_CENTER_CENTER);
  USpriteInitAnimator(proj.sprite, 0, 0, 2, 1, 0.2f);
  proj.sprite->bHidden = true;
  proj.sprite->tag = E_TAG_PROJECTILE;
  proj.sprite->collider.type = E_WORLD_DYNAMIC;
  proj.sprite->collider.scale = VEC3_VALUE(5);
  proj.sprite->collider.bCollisionEnable = true;
  proj.sprite->collider.bDraw = false;
  return proj;
}

void AProjectileUpdate(AProjectile* Self, float DeltaTime) {
  FVector3 location = USpriteGetLocation(Self->sprite);
  if(Self->sprite->bHidden == false) {
    location.x += Self->direction.x * Self->speed * DeltaTime;
    USpriteSetLocation(Self->sprite, location);
    if(location.x < 0 || location.x > FGetScreenWidth()) {
      Self->bActive = false;
      Self->sprite->bHidden = true;
    }
  }
}

void AProjectileDestroy(AProjectile* Self) {
}
