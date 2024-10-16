#include "GameFramework.h"

FVector3 FVector3Add(FVector3 Self, FVector3 Other) {
  Self.x += Other.x;
  Self.y += Other.y;
  Self.z += Other.z;
  return Self;
}

FVector3 FVector3Sub(FVector3 Self, FVector3 Other) {
  Self.x -= Other.x;
  Self.y -= Other.y;
  Self.z -= Other.z;
  return Self;
}

FVector3 FVector3Mul(FVector3 Self, FVector3 Other) {
  Self.x *= Other.x;
  Self.y *= Other.y;
  Self.z *= Other.z;
  return Self;
}

FVector3 FVector3Div(FVector3 Self, FVector3 Other) {
  Self.x /= Other.x;
  Self.y /= Other.y;
  Self.z /= Other.z;
  return Self;
}

FVector3 FVector3AddS(FVector3 Self, float Scale) {
  Self.x += Scale;
  Self.y += Scale;
  Self.z += Scale;
  return Self;
}

FVector3 FVector3SubS(FVector3 Self, float Scale) {
  Self.x -= Scale;
  Self.y -= Scale;
  Self.z -= Scale;
  return Self;
}

FVector3 FVector3MulS(FVector3 Self, float Scale) {
  Self.x *= Scale;
  Self.y *= Scale;
  Self.z *= Scale;
  return Self;
}

FVector3 FVector3DivS(FVector3 Self, float Scale) {
  Self.x /= Scale;
  Self.y /= Scale;
  Self.z /= Scale;
  return Self;
}

FVector3 FVector3Normalize(FVector3 Self) {
  float length = FVector3Length(Self);
  if(length == 0) {
    return VEC3_ZERO;
  }
  float invLength = 1.0f / length;
  return VEC3(Self.x * invLength, Self.y * invLength, Self.z * invLength);
}

FVector3 FVector3Cross(FVector3 Self, FVector3 Other) {
  FVector3 retVal = {
      .x = Self.y * Other.z - Self.z * Other.y,
      .y = Self.z * Other.x - Self.x * Other.z,
      .z = Self.x * Other.y - Self.y * Other.x,
  };
  return retVal;
}

float FVector3Length(FVector3 Self) {
  return sqrtf(Self.x * Self.x + Self.y * Self.y + Self.z * Self.z);
}

float FVector3Dot(FVector3 Self, FVector3 Other) {
  return Self.x * Other.x + Self.y * Other.y + Self.z * Other.z;
}
