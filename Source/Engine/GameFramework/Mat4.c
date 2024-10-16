#include "GameFramework.h"
#include <math.h>

FMat4 FMat4Identity() {
  FMat4 m;
  memset(&m, 0, sizeof(FMat4));
  m.e[0][0] = m.e[1][1] = m.e[2][2] = m.e[3][3] = 1.f;
  return m;
}

FMat4 FMat4MakePerspective(float Fov, float Aspect, float ZNear, float ZFar) {
  Fov = (float)(Fov * (M_PI / 180.0));

  const float tanHalfFov = (float)tan(Fov / 2);
  FMat4 m = FMat4Identity();
  m.e[0][0] = 1 / (Aspect * tanHalfFov);
  m.e[1][1] = 1 / tanHalfFov;
  m.e[2][2] = (ZFar + ZNear) / (ZFar - ZNear);
  m.e[2][3] = -1;
  m.e[3][2] = -(2 * ZFar * ZNear) / (ZFar - ZNear);
  m.e[3][3] = 0.0f;
  return m;
}

FMat4 FMat4MakeOrthographic(float Left, float Right, float Top, float Bottom, float ZNear, float ZFar) {
  FMat4 m = FMat4Identity();
  m.e[0][0] = 2.0f / (Right - Left);
  m.e[1][1] = 2.0f / (Top - Bottom);
  m.e[2][2] = -2.0f / (ZFar - ZNear);
  m.e[3][0] = -(Right + Left) / (Right - Left);
  m.e[3][1] = -(Top + Bottom) / (Top - Bottom);
  m.e[3][2] = -(ZFar + ZNear) / (ZFar - ZNear);
  m.e[3][3] = 1.0f;
  return m;

  return m;
}

FMat4 FMat4MakeTranslation(FVector3 Location) {
  FMat4 m = FMat4Identity();
  m.e[3][0] = Location.x;
  m.e[3][1] = Location.y;
  m.e[3][2] = Location.z;
  return m;
}

FMat4 FMat4MakeRotation(FRotator Rotation) {
  FVector4 quat = {0};
  const float pitch = Rotation.pitch * (M_PI / 180.f);
  const float yaw = Rotation.yaw * (M_PI / 180.f);
  const float roll = Rotation.roll * (M_PI / 180.f);

  const float cy = cosf(yaw * 0.5f);
  const float sy = sinf(yaw * 0.5f);
  const float cp = cosf(pitch * 0.5f);
  const float sp = sinf(pitch * 0.5f);
  const float cr = cosf(roll * 0.5f);
  const float sr = sinf(roll * 0.5f);

  quat.w = cr * cp * cy + sr * sp * sy;
  quat.x = sr * cp * cy - cr * sp * sy;
  quat.y = cr * sp * cy + sr * cp * sy;
  quat.z = cr * cp * sy - sr * sp * cy;

  float x = quat.x;
  float y = quat.y;
  float z = quat.z;
  float w = quat.w;

  //TODO: Test
  FMat4 mat = FMat4Identity();
  mat.e[0][0] = 1 - 2 * (y * y + z * z);
  mat.e[0][1] = 2 * (x * y - z * w);
  mat.e[0][2] = 2 * (x * z + y * w);
  mat.e[0][3] = 0.0f;

  mat.e[1][0] = 2 * (x * y + z * w);
  mat.e[1][1] = 1 - 2 * (x * x + z * z);
  mat.e[1][2] = 2 * (y * z - x * w);
  mat.e[1][3] = 0.0f;

  mat.e[2][0] = 2 * (x * z - y * w);
  mat.e[2][1] = 2 * (y * z + x * w);
  mat.e[2][2] = 1 - 2 * (x * x + y * y);
  mat.e[2][3] = 0.0f;

  mat.e[3][0] = 0.0f;
  mat.e[3][1] = 0.0f;
  mat.e[3][2] = 0.0f;
  mat.e[3][3] = 1.0f;

  return mat;
}

FMat4 FMat4MakeScale(FVector3 Scale) {
  FMat4 m = FMat4Identity();
  m.e[0][0] = Scale.x;
  m.e[1][1] = Scale.y;
  m.e[2][2] = Scale.z;
  return m;
}

FMat4 FMat4Mul(FMat4 M1, FMat4 M2) {
  FMat4 m;

  for(int line = 0; line < 4; line++) {
    for(int column = 0; column < 4; column++) {
      m.e[column][line] =
          M1.e[0][line] * M2.e[column][0] +
          M1.e[1][line] * M2.e[column][1] +
          M1.e[2][line] * M2.e[column][2] +
          M1.e[3][line] * M2.e[column][3];
    }
  }

  return m;
}

// Gabriel => I didn't write this code and I'm still not worthy of understanding it!
FMat4 FMat4Inverse(FMat4 M) {
  double inv[16];
  float* me = (float*)M.e;

  //NOTE(marcio): our matrices are floats. But we calculate the determinant as doubles to enforce precision. I'm not really sure how effectit this is.
  inv[0] = me[5] * me[10] * me[15] -
      me[5] * me[11] * me[14] -
      me[9] * me[6] * me[15] +
      me[9] * me[7] * me[14] +
      me[13] * me[6] * me[11] -
      me[13] * me[7] * me[10];

  inv[4] = -me[4] * me[10] * me[15] +
      me[4] * me[11] * me[14] +
      me[8] * me[6] * me[15] -
      me[8] * me[7] * me[14] -
      me[12] * me[6] * me[11] +
      me[12] * me[7] * me[10];

  inv[8] = me[4] * me[9] * me[15] -
      me[4] * me[11] * me[13] -
      me[8] * me[5] * me[15] +
      me[8] * me[7] * me[13] +
      me[12] * me[5] * me[11] -
      me[12] * me[7] * me[9];

  inv[12] = -me[4] * me[9] * me[14] +
      me[4] * me[10] * me[13] +
      me[8] * me[5] * me[14] -
      me[8] * me[6] * me[13] -
      me[12] * me[5] * me[10] +
      me[12] * me[6] * me[9];

  inv[1] = -me[1] * me[10] * me[15] +
      me[1] * me[11] * me[14] +
      me[9] * me[2] * me[15] -
      me[9] * me[3] * me[14] -
      me[13] * me[2] * me[11] +
      me[13] * me[3] * me[10];

  inv[5] = me[0] * me[10] * me[15] -
      me[0] * me[11] * me[14] -
      me[8] * me[2] * me[15] +
      me[8] * me[3] * me[14] +
      me[12] * me[2] * me[11] -
      me[12] * me[3] * me[10];

  inv[9] = -me[0] * me[9] * me[15] +
      me[0] * me[11] * me[13] +
      me[8] * me[1] * me[15] -
      me[8] * me[3] * me[13] -
      me[12] * me[1] * me[11] +
      me[12] * me[3] * me[9];

  inv[13] = me[0] * me[9] * me[14] -
      me[0] * me[10] * me[13] -
      me[8] * me[1] * me[14] +
      me[8] * me[2] * me[13] +
      me[12] * me[1] * me[10] -
      me[12] * me[2] * me[9];

  inv[2] = me[1] * me[6] * me[15] -
      me[1] * me[7] * me[14] -
      me[5] * me[2] * me[15] +
      me[5] * me[3] * me[14] +
      me[13] * me[2] * me[7] -
      me[13] * me[3] * me[6];

  inv[6] = -me[0] * me[6] * me[15] +
      me[0] * me[7] * me[14] +
      me[4] * me[2] * me[15] -
      me[4] * me[3] * me[14] -
      me[12] * me[2] * me[7] +
      me[12] * me[3] * me[6];

  inv[10] = me[0] * me[5] * me[15] -
      me[0] * me[7] * me[13] -
      me[4] * me[1] * me[15] +
      me[4] * me[3] * me[13] +
      me[12] * me[1] * me[7] -
      me[12] * me[3] * me[5];

  inv[14] = -me[0] * me[5] * me[14] +
      me[0] * me[6] * me[13] +
      me[4] * me[1] * me[14] -
      me[4] * me[2] * me[13] -
      me[12] * me[1] * me[6] +
      me[12] * me[2] * me[5];

  inv[3] = -me[1] * me[6] * me[11] +
      me[1] * me[7] * me[10] +
      me[5] * me[2] * me[11] -
      me[5] * me[3] * me[10] -
      me[9] * me[2] * me[7] +
      me[9] * me[3] * me[6];

  inv[7] = me[0] * me[6] * me[11] -
      me[0] * me[7] * me[10] -
      me[4] * me[2] * me[11] +
      me[4] * me[3] * me[10] +
      me[8] * me[2] * me[7] -
      me[8] * me[3] * me[6];

  inv[11] = -me[0] * me[5] * me[11] +
      me[0] * me[7] * me[9] +
      me[4] * me[1] * me[11] -
      me[4] * me[3] * me[9] -
      me[8] * me[1] * me[7] +
      me[8] * me[3] * me[5];

  inv[15] = me[0] * me[5] * me[10] -
      me[0] * me[6] * me[9] -
      me[4] * me[1] * me[10] +
      me[4] * me[2] * me[9] +
      me[8] * me[1] * me[6] -
      me[8] * me[2] * me[5];

  double det = me[0] * inv[0] + me[1] * inv[4] + me[2] * inv[8] + me[3] * inv[12];

  if(det == 0)
    return FMat4Identity();

  det = 1.0 / det;

  // Stores the result on a new matrix
  FMat4 inverseMatrix;
  me = (float*)inverseMatrix.e;
  for(int32 i = 0; i < 16; i++)
    me[i] = (float)(inv[i] * det);

  return inverseMatrix;
}
