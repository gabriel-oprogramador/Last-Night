#pragma once
#include "Components.h"

#define MAX_SPACE 10
#define MAX_FLOOR 10

typedef struct {
  void (*OnInit)();
  void (*OnStart)();
  void (*OnUpdate)(float);
  void (*OnStop)();
} FLevel;

typedef enum {
  E_TAG_NONE,
  E_TAG_PLAYER,
  E_TAG_ENEMY,
  E_TAG_IN_DOOR,
  E_TAG_PROJECTILE,
  E_TAG_ENEMY_DOOR,
  E_TAG_TRIDENT,
} EGameTags;

typedef enum {
  E_PLAYER_STATE_NORMAL = 0,
  E_PLAYER_STATE_WALK = 1,
  E_PLAYER_STATE_CROUCH = 2,
  E_PLAYER_STATE_FIRE = 3,
  E_PLAYER_STATE_DEAD = 6,
} EPlayerAnimState;

typedef enum {
  E_ENEMY_STATE_IDLE = 0,
  E_ENEMY_STATE_WALK = 1,
  E_ENEMY_STATE_DEAD = 2,
} EEnemyAnimState;

typedef enum {
  E_PROP_WALL,
  E_PROP_CLOSED_DOOR,
  E_PROP_CLOSED_ENEMY,
  E_PROP_OPEN_DOOR,
  E_PROP_MAX,
} EPropType;

typedef struct {
  USprite* spacesSpr[MAX_SPACE];
  uint32 inDoorIndex;
  uint32 outDoorIndex;
  uint32 enemyDoorIndex;
} AFloor;

typedef struct{
  uint32 enemyDeaths;
  uint32 playerHealt;
} AGameState;

ENGINE_API AGameState GGameState;

ENGINE_API USprite* PlayerCreate();
ENGINE_API void PlayerController(USprite* Self);

ENGINE_API USprite* EnemyCreate();
ENGINE_API void EnemyReset(USprite* Self, FVector3 Location);
ENGINE_API void EnemyDeath(USprite* Self);
ENGINE_API void EnemyController(USprite* Self);
ENGINE_API void EnemyChasePlayer(USprite* Self, USprite* Player);
