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
  E_TAG_PROJECTILE,
} EGameTags;

typedef enum {
  E_PLAYER_STATE_NORMAL = 0,
  E_PLAYER_STATE_WALK = 1,
  E_PLAYER_STATE_CROUCH = 2,
  E_PLAYER_STATE_FIRE = 3,
  E_PLAYER_STATE_DEAD = 6,
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
  USprite* spacesSpr[MAX_SPACE];
  ESpaceType types[MAX_SPACE];
  uint32 inDoorIndex;
  uint32 outDoorIndex;
} AFloor;

ENGINE_API USprite* PlayerCreate();
ENGINE_API void PlayerController(USprite* Self);
