#pragma once
#include "Components.h"

#define MAX_SPACE 7
#define MAX_FLOOR 6

typedef struct {
  void (*OnInit)();
  void (*OnStart)();
  void (*OnUpdate)(float);
  void (*OnStop)();
} FLevel;

typedef enum{
  E_TAG_NONE,
  E_TAG_PLAYER,
  E_TAG_ENEMY,
  E_TAG_PROJECTILE,
} EGameTags;

typedef enum {
  E_PLAYER_NORMAL_IDLE,
  E_PLAYER_NORMAL_WALK,
  E_PLAYER_NORMAL_CROUCH,
  E_PLAYER_FIRE_IDLE,
  E_PLAYER_FIRE_WALK,
  E_PLAYER_FIRE_CROUCH,
} EPlayerAnimState;

typedef enum {
  E_PROP_WALL,
  E_PROP_OPEN_DOOR,
  E_PROP_CLOSED_DOOR,
  E_PROP_MAX,
} EPropType;

typedef enum {
  E_SPACE_WALL,
  E_SPACE_CLOSED_DOOR,
  E_SPACE_IN_DOOR,
  E_SPACE_OUT_DOOR,
  E_SPACE_ENEMY_DOOR,
  E_SPACE_MAX
} ESpaceType;

typedef struct {
  USprite spacesSpr[MAX_SPACE];
  ESpaceType types[MAX_SPACE];
} AFloor;

typedef struct {
  USprite* sprite;
  FVector3 direction;
  float healt;
  float speed;
  bool bCrouch;
  bool bEquiped;
} APlayer;

typedef struct{
  USprite* sprite;
  float speed;
  bool bActive;
} AEnemy;

typedef struct {
  USprite* sprite;
  FVector3 direction;
  float speed;
  bool bActive;
} AProjectile;

ENGINE_API APlayer APlayerCreate();
ENGINE_API void APlayerUpdate(APlayer* Self, float DeltaTime);
ENGINE_API void APlayerDestroy(APlayer* Self);

ENGINE_API AEnemy AEnemyCreate();
ENGINE_API void AEnemyUpdate(AEnemy* Self, float DeltaTime);
ENGINE_API void AEnemyDestroy(AEnemy* Self);

ENGINE_API AProjectile AProjectileCreate();
ENGINE_API void AProjectileUpdate(AProjectile* Self, float DeltaTime);
ENGINE_API void AProjectileDestroy(AProjectile* Self);
