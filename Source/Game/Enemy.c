#include "Game.h"
#include "GameFramework.h"
#include "GT/Renderer.h"

static FTexture STexture;
static bool SbValid;

static void AEnemyOnCollision(USprite* Self, USprite* Other);

AEnemy AEnemyCreate() {
  if(SbValid == false) {
    STexture = FTextureLoadFile((FIND_ASSET("Images/Enemy.png")));
    SbValid = true;
  }

  AEnemy enemy = {0};
  enemy.speed = 470;
  enemy.bActive = true;
  USprite spr = USpriteCreate(NULL);
  int32 id = USpriteAdd(&spr);
  enemy.sprite = USpriteGet(id);

  USpriteSetTexture(enemy.sprite, STexture);
  USpriteSetAnchorType(enemy.sprite, E_ANCHOR_BOTTOM_CENTER);
  USpriteSetLocation(enemy.sprite, VEC3(FGetScreenWidth() - 300.f, FGetScreenHeight() / 2.f, 0.f));
  USpriteSetScale(enemy.sprite, VEC3(-3, 3, 3));
  USpriteInitAnimator(enemy.sprite, 0, 0, 2, 3, 0.2f);

  enemy.sprite->tag = E_TAG_ENEMY;
  enemy.sprite->collider.bDraw = true;
  enemy.sprite->collider.bCollisionEnable = true;
  enemy.sprite->collider.onCollision = (FOnCollisionCallback)&AEnemyOnCollision;
  enemy.sprite->collider.type = E_WORLD_DYNAMIC;
  enemy.sprite->collider.scale = VEC3(40, 70, 1);
  enemy.sprite->collider.offset = VEC3(0, 30, 0);
  return enemy;
}

void AEnemyUpdate(AEnemy* Self, float DeltaTime) {
}

void AEnemyDestroy(AEnemy* Self) {
}

static void AEnemyOnCollision(USprite* Self, USprite* Other){
  if(Other->tag == E_TAG_PROJECTILE){
    Self->collider.bCollisionEnable = false;
    Self->animator.currentRow = 2;
    Other->bHidden = true;
  }
}
