#include "GameFramework.h"

FVector2 FVector2Add(FVector2 Self, FVector2 Other) {
  Self.x += Other.x;
  Self.y += Other.y;
  return Self;
}

FVector2 FVector2Sub(FVector2 Self, FVector2 Other) {
  Self.x -= Other.x;
  Self.y -= Other.y;
  return Self;
}

FVector2 FVector2Mul(FVector2 Self, FVector2 Other) {
  Self.x *= Other.x;
  Self.y *= Other.y;
  return Self;
}

FVector2 FVector2Div(FVector2 Self, FVector2 Other) {
  Self.x /= Other.x;
  Self.y /= Other.y;
  return Self;
}

FVector2 FVector2AddS(FVector2 Self, float Scale) {
  Self.x += Scale;
  Self.y += Scale;
  return Self;
}

FVector2 FVector2SubS(FVector2 Self, float Scale) {
  Self.x -= Scale;
  Self.y -= Scale;
  return Self;
}

FVector2 FVector2MulS(FVector2 Self, float Scale) {
  Self.x *= Scale;
  Self.y *= Scale;
  return Self;
}

FVector2 FVector2DivS(FVector2 Self, float Scale) {
  Self.x /= Scale;
  Self.y /= Scale;
  return Self;
}

FVector2 FVector2Normalize(FVector2 Self) {
  float length = FVector2Length(Self);
  if(length == 0.0f) { return VEC2_ZERO; }
  return VEC2(Self.x / length, Self.y / length);
}

float FVector2Length(FVector2 Self) {
  return sqrtf(Self.x * Self.x + Self.y * Self.y);
}

float FVector2Dot(FVector2 Self, FVector2 Other) {
  return Self.x * Other.x + Self.y * Other.y;
}

float FVector2Cross(FVector2 Self, FVector2 Other) {
  return Self.x * Other.y - Self.y * Other.x;
}

FVector2 FVector2ClampField(FVector2 Self, float Min, float Max) {
  Self.x = FClamp(Self.x, Min, Max);
  Self.y = FClamp(Self.y, Min, Max);
  return Self;
}

FVector2 FVector2ClampLength(FVector2 Self, float Min, float Max) {
  float length = FVector2Length(Self);
  if(length < Min) {
    Self = FVector2Normalize(Self);
    Self = FVector2MulS(Self, Min);
  } else if(length > Max) {
    Self = FVector2Normalize(Self);
    Self = FVector2MulS(Self, Max);
  }
  return Self;
}

FVector2 FVector2Abs(FVector2 Self) {
  Self.x = FAbs(Self.x);
  Self.y = FAbs(Self.y);
  return Self;
}
