#include "Components.h"
#include "GT/Renderer.h"
#include "GameFramework.h"
#include "GT/Platform.h"
#include "GL/ApiGL.h"

#define MAX_ARENA_SIZE 32

static FPrimitive SQuad;
static FShader SShaderID;
static bool SbQuadIsValid = false;
static struct {
  uint32 uProjID;
  uint32 uModelID;
  uint32 uColorID;
  uint32 uAnimCoordID;
} SQuadInfo;

static struct {
  USprite sprites[MAX_ARENA_SIZE];
  int32 freeList[MAX_ARENA_SIZE];
  uint32 numSprites;
  uint32 numfreeSlots;
} SSpriteManager;

void USpriteCreateArena() {
  SSpriteManager.numSprites = 0;
  SSpriteManager.numfreeSlots = 0;
  PMemSet(SSpriteManager.sprites, -1, sizeof(SSpriteManager.sprites));
  PMemSet(SSpriteManager.freeList, -1, sizeof(SSpriteManager.freeList));
}

void USpriteUpdateArena() {
  float dt = FGetDeltaTime();
  for(int32 c = 0; c < MAX_ARENA_SIZE; c++) {
    USprite* spr = &SSpriteManager.sprites[c];
    UTransform* transform = &spr->transform;
    UAnimator* animator = &spr->animator;
    FVector3 finalLocation = VEC3_ZERO;
    FVector3 finalScale = VEC3_ONE;

    if(spr->id == -1) { continue; }

    if(spr->bAnimation) {
      animator->elapsedTime += dt;

      if(animator->elapsedTime >= animator->frameTime) {
        animator->currentFrame++;
        animator->elapsedTime = 0.f;

        if(animator->currentFrame >= animator->totalFrames) {
          if(animator->bLoop) {
            animator->currentFrame = 0;
          } else {
            animator->currentFrame = animator->totalFrames - 1;
          }
        }
      }
    }

    if(transform->bDirty) {
      FVector3 newLocation = FVector3Add(spr->transform.location, spr->movement.velocity);
      FVector3 direction = (FVector3Length(spr->movement.velocity) > 0.f) ? FVector3Normalize(spr->movement.velocity) : spr->movement.direction;
      USpriteSetLocation(spr, newLocation);

      if(spr->bInvertible) {
        FVector3 scale = FVector3Abs(spr->transform.scale);
        spr->transform.scale.x = scale.x * (direction.x >= 0 ? 1.f : -1.f);
        spr->transform.scale.y = scale.y * (direction.y >= 0 ? 1.f : -1.f);
        spr->transform.scale.z = 1.f;
      }

      finalScale.x = spr->transform.scale.x * spr->animator.frameWidth;
      finalScale.y = spr->transform.scale.y * spr->animator.frameHeight;
      finalLocation = spr->anchor.location;

      FMat4 mTransform = FMat4MakeTranslation(finalLocation);
      FMat4 mRotation = FMat4MakeRotation(transform->rotation);
      FMat4 mScale = FMat4MakeScale(finalScale);
      transform->model = FMat4Mul(mTransform, FMat4Mul(mRotation, mScale));
      spr->collider.location = FVector3Add(spr->anchor.location, spr->collider.offset);
      spr->movement.direction = direction;
      spr->movement.velocity = VEC3_ZERO;
      transform->bDirty = false;
    }
  }
  USpriteCheckCollisionArena();
}

void USpriteCheckCollisionArena() {
  for(int32 c = 0; c < SSpriteManager.numSprites; c++) {
    USprite* self = &SSpriteManager.sprites[c];

    if(self->collider.bCollisionEnable == false) { continue; }

    if(self->collider.type == E_WORLD_STATIC || self->id == -1) {
      continue;
    }

    for(int32 d = c + 1; d < SSpriteManager.numSprites; d++) {
      USprite* other = &SSpriteManager.sprites[d];
      if(other->collider.bCollisionEnable == false) { continue; }

      if(USpriteIsCollisionSprite(self, other)) {
        if(self->collider.onCollision != NULL) { self->collider.onCollision(self, other); }
        if(other->collider.onCollision != NULL) { other->collider.onCollision(other, self); }
      }
    }
  }
}

void USpriteRenderArena() {
  /*float dt = FGetDeltaTime();*/

  for(int32 c = 0; c < MAX_ARENA_SIZE; c++) {
    USprite* spr = &SSpriteManager.sprites[c];
    if(spr->bHidden == true) { continue; }

    UAnimator* anim = &spr->animator;
    FColor color = COLOR_WHITE;

    float frameWidth = 1.f / spr->animator.totalFrames;
    float frameHeight = 1.f / spr->animator.totalRows;
    float uMin = 0.f;
    float uMax = 1.f;
    float vMin = 0.f;
    float vMax = 1.f;

    if(spr->bAnimation == true) {
      int32 column = anim->currentFrame;
      int32 row = anim->currentRow;
      uMin = column * frameWidth;
      uMax = uMin + frameWidth;
      vMin = row * frameHeight;
      vMax = vMin + frameHeight;
    } else {
      int32 column = anim->currentFrame;
      int32 row = anim->currentRow;
      uMin = column;
      uMax = uMin + frameWidth;
      vMin = row * frameHeight;
      vMax = vMin + frameHeight;
    }

    glUseProgram(SShaderID);
    glBindVertexArray(SQuad.objectID);

    glUniform4fv(SQuadInfo.uColorID, 1, (const float*)&color);
    glUniform4f(SQuadInfo.uAnimCoordID, uMin, vMin, uMax, vMax);
    glUniformMatrix4fv(SQuadInfo.uProjID, 1, GL_FALSE, (const float*)FGetProjection(E_PROJECTION_ORTHOGRAPHIC).e);
    glUniformMatrix4fv(SQuadInfo.uModelID, 1, GL_FALSE, (const float*)spr->transform.model.e);

    /*glDisable(GL_BLEND);*/
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, spr->texture.id);
    glDrawElements(GL_TRIANGLES, SQuad.indices, GL_UNSIGNED_INT, NULL);
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);

    // For Debug...
    FColor quadColor = COLOR_YELLOW;
    quadColor.a = 0.33f;
    if(spr->collider.bDraw) {
      FVector3 size = FVector3MulS(spr->transform.scale, 2.5f);
      FDrawQuad(spr->collider.location, spr->collider.scale, quadColor);
      FDrawCircle(spr->collider.location, size, COLOR_PURPLE);
      FDrawCircle(spr->transform.location, size, COLOR_RED);
    }
  }
}

void USpriteDestroyArena() {
}

USprite* USpriteAdd(USprite* Self) {
  uint32 index = 0;
  if(SSpriteManager.numfreeSlots > 0) {
    index = SSpriteManager.freeList[--SSpriteManager.numfreeSlots];
  } else if(SSpriteManager.numSprites < MAX_ARENA_SIZE) {
    index = SSpriteManager.numSprites++;
  } else {
    GT_LOG(LOG_ALERT, "Full USprite Arena Max size is %d", MAX_ARENA_SIZE);
    return NULL;
  }

  USprite* ptr = &SSpriteManager.sprites[index];
  PMemCopy(ptr, Self, sizeof(USprite));
  ptr->id = index;
  return ptr;
}

void USpriteRemove(USprite* Self) {
  int32 id = Self->id;
  if(id > 0 && id < SSpriteManager.numSprites) {
    SSpriteManager.freeList[SSpriteManager.numfreeSlots++] = id;
    SSpriteManager.sprites[id] = (USprite){0};
    SSpriteManager.sprites[id].id = -1;
  }
}

void USpriteRemoveID(int32 SpriteID) {
  if(SpriteID > 0 && SpriteID < SSpriteManager.numSprites) {
    SSpriteManager.freeList[SSpriteManager.numfreeSlots++] = SpriteID;
    SSpriteManager.sprites[SpriteID] = (USprite){0};
    SSpriteManager.sprites[SpriteID].id = -1;
  }
}

USprite* USpriteGet(int32 SpriteID) {
  if(SpriteID < 0 && SpriteID >= MAX_ARENA_SIZE) {
    GT_LOG(LOG_ERROR, "USprite Index invalid");
    return NULL;
  }

  return &SSpriteManager.sprites[SpriteID];
}

USprite USpriteCreate(cstring TexturePath) {
  if(SbQuadIsValid == false) {
    SQuad = FGetPrimitive(E_SHAPE_QUAD);
    SShaderID = FShaderLoad(FIND_ASSET("Shaders/Sprite.vert"), FIND_ASSET("Shaders/Sprite.frag"));
    SQuadInfo.uProjID = glGetUniformLocation(SShaderID, "uProj");
    SQuadInfo.uModelID = glGetUniformLocation(SShaderID, "uModel");
    SQuadInfo.uColorID = glGetUniformLocation(SShaderID, "uColor");
    SQuadInfo.uAnimCoordID = glGetUniformLocation(SShaderID, "uAnimCoord");
    SbQuadIsValid = true;
  }

  USprite self = {0};
  self.transform.location = VEC3_ZERO;
  self.transform.rotation = ROT_ZERO;
  self.transform.scale = VEC3_ONE;
  self.transform.bDirty = true;
  self.bHidden = false;
  self.bAnimation = false;
  self.movement.velocity = VEC3_ZERO;
  self.movement.direction = VEC3(1, 0, 0);
  self.movement.maxSpeed = 300.f;
  self.animator.currentFrame = 0;
  self.animator.currentRow = 0;
  self.animator.totalFrames = 1;
  self.animator.totalRows = 1;
  self.animator.bLoop = true;
  self.animator.frameTime = 0.f;
  self.animator.elapsedTime = 0.f;
  USpriteLoadTexture(&self, TexturePath);
  return self;
}

void USpriteDestroy(USprite* Self) {
}

void USpriteUpdate(USprite* Self, float DeltaTime) {
}

void USpriteRender(USprite* Self, float DeltaTime) {
}

void USpriteSetTexture(USprite* Self, FTexture Texture) {
  Self->texture = Texture;
  Self->animator.currentFrame = 0;
  Self->animator.currentRow = 0;
  Self->animator.totalFrames = 1;
  Self->animator.totalRows = 1;
  Self->spriteRect.x = 0;
  Self->spriteRect.y = 0;
  Self->spriteRect.width = Self->transform.scale.x * Texture.width;
  Self->spriteRect.height = Self->transform.scale.y * Texture.height;
}

void USpriteLoadTexture(USprite* Self, cstring TexturePath) {
  USpriteSetTexture(Self, FTextureLoadFile(TexturePath));
}

// Getter and Setters //===========================================//
FVector3 USpriteGetLocation(USprite* Self) {
  return Self->transform.location;
}

void USpriteSetLocation(USprite* Self, FVector3 NewLocation) {
  Self->transform.bDirty = true;
  Self->transform.location = NewLocation;
  Self->anchor.location = FVector3Add(Self->transform.location, Self->anchor.offset);
}

FRotator USPriteGetRotation(USprite* Self) {
  return Self->transform.rotation;
}

void USpriteSetRotation(USprite* Self, FRotator NewRotation) {
  Self->transform.bDirty = true;
  Self->transform.rotation = NewRotation;
}

FVector3 USpriteGetScale(USprite* Self) {
  return Self->transform.scale;
}

void USpriteSetScale(USprite* Self, FVector3 NewScale) {
  Self->transform.bDirty = true;
  Self->transform.scale = NewScale;
  Self->spriteRect.width = Self->transform.scale.x * Self->texture.width;
  Self->spriteRect.height = Self->transform.scale.y * Self->texture.height;
}

FVector3 USpriteGetSize(USprite* Self) {
  return VEC3(Self->spriteRect.width, Self->spriteRect.height, 0.f);
}

void USpriteInitAnimator(USprite* Self, uint32 Frame, uint32 Row, uint32 Frames, uint32 Rows, float Time) {
  Self->bAnimation = (Time > 0.00f) ? true : false;
  Self->animator.currentFrame = Frame;
  Self->animator.currentRow = Row;
  Self->animator.totalFrames = Frames;
  Self->animator.totalRows = Rows;
  Self->animator.frameTime = Time;
  Self->animator.elapsedTime = 0.0f;
  Self->animator.frameWidth = (float)Self->texture.width / Self->animator.totalFrames;
  Self->animator.frameHeight = (float)Self->texture.height / Self->animator.totalRows;
  Self->spriteRect.width = (Self->transform.scale.x * Self->animator.frameWidth);
  Self->spriteRect.height = (Self->transform.scale.y * Self->animator.frameHeight);
}

void USpriteAddAnchorOffset(USprite* Self, FVector3 Offset) {
  Self->anchor.offset = FVector3Add(Self->anchor.offset, Offset);
  Self->anchor.location = FVector3Add(Self->transform.location, Self->anchor.offset);
}

void USpriteSetAnchorType(USprite* Self, EAnchorType Type) {
  FVector3 offset = {0};
  FRect rectSpt = Self->spriteRect;
  switch((uint32)Type) {
    case E_ANCHOR_CENTER_CENTER: {
      offset = VEC3_ZERO;
      USpriteAddAnchorOffset(Self, offset);
    } break;

    case E_ANCHOR_LEFT_TOP: {
      offset = VEC3(rectSpt.width, rectSpt.height, 0);
      USpriteAddAnchorOffset(Self, offset);
    } break;
    case E_ANCHOR_LEFT_CENTER: {
      offset = VEC3(rectSpt.width, 0, 0);
      USpriteAddAnchorOffset(Self, offset);
    } break;
    case E_ANCHOR_LEFT_BOTTOM: {
      offset = VEC3(rectSpt.width, -rectSpt.height, 0);
      USpriteAddAnchorOffset(Self, offset);
    } break;

    case E_ANCHOR_RIGHT_TOP: {
      offset = VEC3(-rectSpt.width, rectSpt.height, 0);
      USpriteAddAnchorOffset(Self, offset);
    } break;
    case E_ANCHOR_RIGHT_BOTTOM: {
      offset = VEC3(-rectSpt.width, -rectSpt.height, 0);
      USpriteAddAnchorOffset(Self, offset);
    } break;
    case E_ANCHOR_RIGHT_CENTER: {
      offset = VEC3(-rectSpt.width, 0, 0);
      USpriteAddAnchorOffset(Self, offset);
    } break;

    case E_ANCHOR_TOP_CENTER: {
      offset = VEC3(0, rectSpt.height, 0);
      USpriteAddAnchorOffset(Self, offset);
    } break;
    case E_ANCHOR_BOTTOM_CENTER: {
      offset = VEC3(0, -rectSpt.height, 0);
      USpriteAddAnchorOffset(Self, offset);
    } break;
  }
  Self->anchor.type = Type;
}

FVector3 USpriteGetAnchorLacation(USprite* Self) {
  return Self->anchor.location;
}

bool USpriteIsCollisionSprite(USprite* Self, USprite* Other) {
  UBoxCollider* boxSelf = &Self->collider;
  UBoxCollider* boxOther = &Other->collider;

  float selfLeft = boxSelf->location.x - boxSelf->scale.x;
  float selfRight = boxSelf->location.x + boxSelf->scale.x;
  float selfTop = boxSelf->location.y - boxSelf->scale.y;
  float selfBottom = boxSelf->location.y + boxSelf->scale.y;

  float otherLeft = boxOther->location.x - boxOther->scale.x;
  float otherRight = boxOther->location.x + boxOther->scale.x;
  float otherTop = boxOther->location.y - boxOther->scale.y;
  float otherBottom = boxOther->location.y + boxOther->scale.y;

  if(selfLeft < otherRight && selfRight > otherLeft &&
      selfTop < otherBottom && selfBottom > otherTop) {
    return true;
  }
  return false;
}

bool USpriteIsCollisionPointer(USprite* Self, FVector2 Pointer) {
  UBoxCollider* collider = &Self->collider;

  float left = collider->location.x - (collider->scale.x);
  float right = collider->location.x + (collider->scale.x);
  float top = collider->location.y - (collider->scale.y);
  float bottom = collider->location.y + (collider->scale.y);

  if(Pointer.x >= left && Pointer.x <= right &&
      Pointer.y >= top && Pointer.y <= bottom) {
    return true;
  }

  return false;
}

void USpriteAddMovement(USprite* Self, FVector3 Direction) {
  Direction = FVector3Normalize(Direction);
  FVector3 velocity = FVector3MulS(Direction, Self->movement.maxSpeed * FGetDeltaTime());
  velocity = FVector3Add(Self->movement.velocity, velocity);
  Self->movement.velocity = FVector3ClampLength(velocity, 0.f, Self->movement.maxSpeed);
  Self->transform.bDirty = true;
}

void USpriteStopMovement(USprite* Self){
  Self->movement.velocity = VEC3_ZERO;
}
