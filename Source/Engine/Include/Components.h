#pragma once
#include "CoreMinimal.h"

typedef void (*FOnCollisionCallback)(void*, void*);

typedef struct {
  FOnCollisionCallback onCollision;
  EWorldType type;
  FVector3 location;
  FVector3 offset;
  FVector3 scale;
  bool bCollisionEnable;
  bool bDraw;
} UBoxCollider;

typedef struct {
  bool bLoop;
  int32 currentFrame;
  int32 currentRow;
  int32 totalFrames;
  int32 totalRows;
  float frameTime;
  float elapsedTime;
  float frameWidth;
  float frameHeight;
} UAnimator;

typedef struct {
  FVector3 location;
  FRotator rotation;
  FVector3 scale;
  FMat4 model;
  bool bDirty;
} UTransform;

typedef struct{
  FVector3 direction;
  FVector3 velocity;
  FVector3 forward;
  FVector3 right;
  FVector3 up;
  float maxSpeed;
} UMovement;

typedef struct {
  UTransform transform;
  UAnimator animator;
  UBoxCollider collider;
  UMovement movement;
  FTexture texture;
  FAnchor anchor;
  FRect spriteRect;
  uint32 tag;
  int32 id;
  bool bInvertible;
  bool bAnimation;
  bool bHidden;
} USprite;

ENGINE_API void USpriteCreateArena();
ENGINE_API void USpriteUpdateArena();
ENGINE_API void USpriteCheckCollisionArena();
ENGINE_API void USpriteRenderArena();
ENGINE_API void USpriteDestroyArena();

ENGINE_API USprite* USpriteAdd(USprite* Self);
ENGINE_API void USpriteRemove(USprite* Self);
ENGINE_API void USpriteRemoveID(int32 SpriteID);
ENGINE_API USprite* USpriteGet(int32 SpriteID);

ENGINE_API USprite USpriteCreate(cstring TexturePath);
ENGINE_API void USpriteDestroy(USprite* Self);
ENGINE_API void USpriteUpdate(USprite* Self, float DeltaTime);
ENGINE_API void USpriteRender(USprite* Self, float DeltaTime);

ENGINE_API void USpriteSetTexture(USprite* Self, FTexture Texture);
ENGINE_API void USpriteLoadTexture(USprite* Self, cstring TexturePath);

ENGINE_API FVector3 USpriteGetLocation(USprite* Self);
ENGINE_API void USpriteSetLocation(USprite* Self, FVector3 NewLocation);

ENGINE_API FRotator USPriteGetRotation(USprite* Self);
ENGINE_API void USpriteSetRotation(USprite* Self, FRotator NewRotation);

ENGINE_API FVector3 USpriteGetScale(USprite* Self);
ENGINE_API void USpriteSetScale(USprite* Self, FVector3 NewScale);

ENGINE_API FVector3 USpriteGetSize(USprite* Self);

ENGINE_API void USpriteInitAnimator(USprite* Self, uint32 Frame, uint32 Row, uint32 TotalFrames, uint32 TotalRows, float Time);

ENGINE_API void USpriteAddAnchorOffset(USprite* Self, FVector3 Offset);
ENGINE_API void USpriteSetAnchorType(USprite* Self, EAnchorType Type);
ENGINE_API FVector3 USpriteGetAnchorLacation(USprite* Self);

ENGINE_API bool USpriteIsCollisionSprite(USprite* Self, USprite* Other);
ENGINE_API bool USpriteIsCollisionPointer(USprite* Self, FVector2 Pointer);

ENGINE_API void USpriteAddMovement(USprite* Self, FVector3 Direction);
ENGINE_API void USpriteStopMovement(USprite* Self);
