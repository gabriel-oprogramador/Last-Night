#include "GameFramework.h"

float FAbs(float Value) {
  return (Value < 0) ? -Value : Value;
}

float FClamp(float Value, float Min, float Max) {
  float retVal = 0;
  retVal = (Value < Min) ? Min : Value;
  retVal = (Value > Max) ? Max : Value;
  return retVal;
}
