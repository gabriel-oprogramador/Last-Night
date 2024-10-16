#pragma once
#include "CoreMinimal.h"

ENGINE_API void FRendererInitialize(ERendererApi Renderer);
ENGINE_API void FRendererTerminate();
ENGINE_API void FSetClearColor(FColor Color);
ENGINE_API void FClearBuffers();
ENGINE_API void FSetViewport(FRect Viewport);

ENGINE_API FMat4 FGetProjection(EProjectioType Projection);

// Shader Program
ENGINE_API FShader FShaderDefault();
ENGINE_API FShader FShaderLoad(cstring VertexPath, cstring FragmentPath);
ENGINE_API FShader FShaderCreate(cstring VertexSource, cstring FragmentSource);
ENGINE_API void FShaderFree(FShader Shader);

// Texture
ENGINE_API FTexture FTextureLoadFile(cstring AssetPath);
ENGINE_API void FTextureUnload(FTexture Texture);
ENGINE_API FTexture FTextureDefault();
ENGINE_API FTexture FTextureWhite();

// Primitives
ENGINE_API void FDrawCircle(FVector3 Location, FVector3 Scale, FColor Color);
ENGINE_API void FDrawQuad(FVector3 Location, FVector3 Scale, FColor Color);
ENGINE_API FPrimitive FGetPrimitive(EPrimitiveShape Shape);
