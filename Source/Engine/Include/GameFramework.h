#pragma once
#include "CoreMinimal.h"

/*==================/ Input Manager /===================*/
ENGINE_API bool FIsKeyPressed(uchar KeyCode);
ENGINE_API bool FIsKeyRelease(uchar KeyCode);
ENGINE_API bool FIsKeyRepeat(uchar KeyCode);
ENGINE_API float FGetMouseX();
ENGINE_API float FGetMouseY();
ENGINE_API FVector2 FGetMousePos();
ENGINE_API void FSetMousePos(FVector2 NewPosition);
ENGINE_API FVector2 FGetMouseScreenPos();
ENGINE_API void FSetMouseScreenPos(FVector2 NewPosition);
ENGINE_API void FCaptureMouse(bool bCapture);

/*==================/ Timer Manager /===================*/
ENGINE_API double FGetDeltaTime();
ENGINE_API uint32 FGetFrameRate();
ENGINE_API void FSetTargetFPS(uint32 Target);

/*==================/ Window Manager /===================*/
ENGINE_API void FInitWindow(int32 Width, int32 Height, cstring Title);
ENGINE_API void FCloseWindow();
ENGINE_API FVector2 FGetMonitorSize();
ENGINE_API FVector2 FGetWindowSize();
ENGINE_API void FSetWindowSize(FVector2 Size);
ENGINE_API void FSetWindowMode(EWindowMode Mode);
ENGINE_API EWindowMode FGetWindowMode();
ENGINE_API void FToggleFullscreen();
ENGINE_API FRect FGetScreenSpace();
ENGINE_API FVector2 FGetScreenSize();
ENGINE_API float FGetScreenWidth();
ENGINE_API float FGetScreenHeight();

/*==================/ Asset Manager /===================*/
ENGINE_API FImage* FImageLoadFile(cstring AssetPath);
ENGINE_API void FImageUnload(FImage* Image);

/*==================/ Matrix Manager /===================*/
ENGINE_API FMat4 FMat4Identity();
ENGINE_API FMat4 FMat4MakePerspective(float Fov, float Aspect, float ZNear, float ZFar);
ENGINE_API FMat4 FMat4MakeOrthographic(float Left, float Right, float Top, float Bottom, float ZNear, float ZFar);
ENGINE_API FMat4 FMat4MakeTranslation(FVector3 Location);
ENGINE_API FMat4 FMat4MakeRotation(FRotator Rotation);
ENGINE_API FMat4 FMat4MakeScale(FVector3 Scale);
ENGINE_API FMat4 FMat4Mul(FMat4 M1, FMat4 M2);
ENGINE_API FMat4 FMat4Inverse(FMat4 M);

/*==================/ Vector 2D Manager /===================*/
ENGINE_API FVector2 FVector2Add(FVector2 Self, FVector2 Other);
ENGINE_API FVector2 FVector2Sub(FVector2 Self, FVector2 Other);
ENGINE_API FVector2 FVector2Mul(FVector2 Self, FVector2 Other);
ENGINE_API FVector2 FVector2Div(FVector2 Self, FVector2 Other);
ENGINE_API FVector2 FVector2AddS(FVector2 Self, float Scale);
ENGINE_API FVector2 FVector2SubS(FVector2 Self, float Scale);
ENGINE_API FVector2 FVector2MulS(FVector2 Self, float Scale);
ENGINE_API FVector2 FVector2DivS(FVector2 Self, float Scale);
ENGINE_API FVector2 FVector2Normalize(FVector2 Self);
ENGINE_API float FVector2Length(FVector2 Self);
ENGINE_API float FVector2Dot(FVector2 Self, FVector2 Other);
ENGINE_API float FVector2Cross(FVector2 Self, FVector2 Other);

/*==================/ Vector 3D Manager /===================*/
ENGINE_API FVector3 FVector3Add(FVector3 Self, FVector3 Other);
ENGINE_API FVector3 FVector3Sub(FVector3 Self, FVector3 Other);
ENGINE_API FVector3 FVector3Mul(FVector3 Self, FVector3 Other);
ENGINE_API FVector3 FVector3Div(FVector3 Self, FVector3 Other);
ENGINE_API FVector3 FVector3AddS(FVector3 Self, float Scale);
ENGINE_API FVector3 FVector3SubS(FVector3 Self, float Scale);
ENGINE_API FVector3 FVector3MulS(FVector3 Self, float Scale);
ENGINE_API FVector3 FVector3DivS(FVector3 Self, float Scale);
ENGINE_API FVector3 FVector3Normalize(FVector3 Self);
ENGINE_API FVector3 FVector3Cross(FVector3 Self, FVector3 Other);
ENGINE_API float FVector3Length(FVector3 Self);
ENGINE_API float FVector3Dot(FVector3 Self, FVector3 Other);
