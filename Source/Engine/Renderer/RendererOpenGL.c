#ifdef RENDERER_OPENGL
#include <string.h>
#include "GT/Renderer.h"
#include "GT/Platform.h"
#include "GL/ApiGL.h"
#include "GT/Engine.h"
#include "GameFramework.h"

#define DEFAULT_TEXTURE_SIZE 32

static FShader SDefaultShader = 0;
static FMat4 SPerspectiveMatrix;
static FMat4 SOrthographicMatrix;
static struct {
  uint32 uProjID;
  uint32 uModelID;
  uint32 uColorID;
} SPrimitiveInfo;

static void InternalDrawPrimitive(FPrimitive Primitive, FVector3 Location, FVector3 Scale, FColor Color);
static FPrimitive InternalCreateQuad();
static FPrimitive InternalCreateCircle();

static cstring SDefaultVertSource =
    "#version 330 core\n"
    "layout(location = 0) in vec3 aVertPos;\n"
    "layout(location = 1) in vec2 aTexCoord;\n"
    "uniform mat4 uProj;\n"
    "uniform mat4 uModel;\n"
    "out vec2 vTexCoord;\n"
    "void main(){\n"
    " gl_Position = uProj * uModel * vec4(aVertPos, 1);\n"
    " vTexCoord = aTexCoord;\n"
    "}";

static cstring SDefaultFragSource =
    "#version 330 core\n"
    "uniform vec4 uColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
    "uniform sampler2D uMainTex;\n"
    "in vec2 vTexCoord;\n"
    "vec4 finalColor;\n"
    "out vec4 fragColor;\n"
    "void main() {\n"
    " finalColor = uColor * texture(uMainTex, vTexCoord);\n"
    " if (finalColor.a < 0.2)\n"
    "   discard;\n"
    " fragColor = finalColor;"
    "}";

void FRendererInitialize(ERendererApi Renderer) {
  bool bGraphic = false;
  switch(Renderer) {
    case E_OPENGL_VERSION_3_3: bGraphic = GEngine.graphicApi.OnInitOpenGL(3, 3); break;
    case E_OPENGL_VERSION_4_5: bGraphic = GEngine.graphicApi.OnInitOpenGL(4, 5); break;
    case E_OPENGL_VERSION_4_6: bGraphic = GEngine.graphicApi.OnInitOpenGL(4, 6); break;
  }

  if(bGraphic == false) {
    GT_LOG(LOG_FATAL, "API-GL: Uninitialized!");
    return;
  }

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  SDefaultShader = FShaderCreate(SDefaultVertSource, SDefaultFragSource);

  glUseProgram(SDefaultShader);
  SPrimitiveInfo.uProjID = glGetUniformLocation(SDefaultShader, "uProj");
  SPrimitiveInfo.uModelID = glGetUniformLocation(SDefaultShader, "uModel");
  SPrimitiveInfo.uColorID = glGetUniformLocation(SDefaultShader, "uColor");
  glUseProgram(0);

  GT_LOG(LOG_INFO, "API-GL: Created Default Shader Program");
}

void FRendererTerminate() {
}

void FSetClearColor(FColor Color) {
  glClearColor(FCOLOR_GL(Color));
}

void FClearBuffers() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void FSetViewport(FRect Viewport) {
  glViewport(Viewport.x, Viewport.y, Viewport.width, Viewport.height);
}

FMat4 FGetProjection(EProjectioType Projection) {
  static bool bValid = false;
  if(bValid == false) {
    SPerspectiveMatrix = FMat4MakePerspective(60, 1.7777, -1, 100.f);
    FRect screen = FGetScreenSpace();
    SOrthographicMatrix = FMat4MakeOrthographic(screen.x, screen.width, screen.y, screen.height, -10000, 10000.f);
  }

  switch(Projection) {
    case E_PROJECTION_PERSPECTIVE: return SPerspectiveMatrix; break;
    case E_PROJECTION_ORTHOGRAPHIC: return SOrthographicMatrix; break;
  }
}

// Shader Program
FShader FShaderDefault() {
  return SDefaultShader;
}

FShader FShaderCreate(cstring VertexSource, cstring FragmentSource) {
  char logBuffer[BUFFER_LOG_SIZE] = {""};
  uint32 vertexShader, fragmentShader, shaderProgram;
  int32 status = 0;

  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, (const char**)&VertexSource, 0);
  glCompileShader(vertexShader);
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
  if(!status) {
    glGetShaderInfoLog(vertexShader, BUFFER_LOG_SIZE, NULL, logBuffer);
    GT_LOG(LOG_ALERT, "Vertex: %s", logBuffer);
    glDeleteShader(vertexShader);
    return SDefaultShader;
  }

  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, (const char**)&FragmentSource, 0);
  glCompileShader(fragmentShader);
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
  if(!status) {
    glGetShaderInfoLog(fragmentShader, BUFFER_LOG_SIZE, NULL, logBuffer);
    GT_LOG(LOG_ALERT, "Fragment: %s", logBuffer);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return SDefaultShader;
  }

  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
  if(!status) {
    glGetProgramInfoLog(shaderProgram, BUFFER_LOG_SIZE, NULL, logBuffer);
    GT_LOG(LOG_ALERT, "Shader Link: %s", logBuffer);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteProgram(shaderProgram);
    return SDefaultShader;
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  //TODO:Create arenas for Shader
  return shaderProgram;
}

FShader FShaderLoad(cstring VertexPath, cstring FragmentPath) {
  cstring vs = strstr(VertexPath, CONTENT_DIR);
  cstring fs = strstr(FragmentPath, CONTENT_DIR);
  vs = (vs == NULL) ? VertexPath : vs;
  fs = (fs == NULL) ? FragmentPath : fs;

  char* vsBuffer = NULL;
  char* fsBuffer = NULL;

  if(!PReadTextFile(VertexPath, &vsBuffer, NULL, 0)) {
    GT_LOG(LOG_ALERT, "Vertex Shader did not load!");
    return SDefaultShader;
  }

  if(!PReadTextFile(FragmentPath, &fsBuffer, NULL, 0)) {
    GT_LOG(LOG_ALERT, "API-GL: Fragment Shader did not load!");
    PMemFree(vsBuffer);
    return SDefaultShader;
  }

  FShader shaderProgram = FShaderCreate(vsBuffer, fsBuffer);

  PMemFree(vsBuffer);
  PMemFree(fsBuffer);

  if(shaderProgram == SDefaultShader) {
    GT_LOG(LOG_INFO, "API-GL: Use Default Shader Program");
    return SDefaultShader;
  }

  GT_LOG(LOG_INFO, "API-GL: Loaded Shader Program => %s | %s", vs, fs);
  return shaderProgram;
}

void FShaderFree(FShader Shader) {
  glDeleteProgram(Shader);
}

FPrimitive FGetPrimitive(EPrimitiveShape Shape) {
  FPrimitive retVal = {0};
  switch(Shape) {
    case E_SHAPE_QUAD: retVal = InternalCreateQuad(); break;
    case E_SHAPE_CIRCLE: retVal = InternalCreateCircle(); break;
  }

  return retVal;
}

FTexture FTextureLoadFile(cstring AssetPath) {
  if(AssetPath == NULL) {
    return FTextureDefault();
  }

  FImage* image = FImageLoadFile(AssetPath);
  FTexture txd = {
      .id = 0,
      .width = image->width,
      .height = image->height,
      .format = image->format,
      .mipmap = 1,
  };

  glGenTextures(1, &txd.id);
  glBindTexture(GL_TEXTURE_2D, txd.id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  GLenum format = txd.format == 4 ? GL_RGBA : GL_RGB;
  glTexImage2D(GL_TEXTURE_2D, 0, format, txd.width, txd.height, 0, format, GL_UNSIGNED_BYTE, image->data);
  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);
  FImageUnload(image);
  return txd;
}

void FTextureUnload(FTexture Texture) {
  glDeleteTextures(1, &Texture.id);
}

FTexture FTextureDefault() {
  static FTexture texture = {0};
  static bool bValid = false;
  if(bValid) {
    return texture;
  }

  const uchar grayLight[3] = {200, 200, 200};
  const uchar grayDark[3] = {100, 100, 100};

  uchar checkerboard[32 * 32 * 3];

  for(int32 y = 0; y < 32; ++y) {
    for(int32 x = 0; x < 32; ++x) {
      int32 index = (y * 32 + x) * 3;
      if(((x / 8) % 2) == ((y / 8) % 2)) {
        checkerboard[index] = grayLight[0];
        checkerboard[index + 1] = grayLight[1];
        checkerboard[index + 2] = grayLight[2];
      } else {
        checkerboard[index] = grayDark[0];
        checkerboard[index + 1] = grayDark[1];
        checkerboard[index + 2] = grayDark[2];
      }
    }
  }

  GLuint textureID;
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_2D, textureID);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 32, 32, 0, GL_RGB, GL_UNSIGNED_BYTE, checkerboard);

  glBindTexture(GL_TEXTURE_2D, 0);

  texture.id = textureID;
  texture.width = DEFAULT_TEXTURE_SIZE;
  texture.height = DEFAULT_TEXTURE_SIZE;
  texture.format = 3;
  texture.mipmap = 1;

  return texture;
}

FTexture FTextureWhite() {
  static bool bValid = false;
  static FTexture texture = {0};

  if(bValid == true) {
    return texture;
  }

  GLuint textureID;
  glGenTextures(1, &textureID);

  glBindTexture(GL_TEXTURE_2D, textureID);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  uchar whiteTextureData[4] = {255, 255, 255, 255};

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, whiteTextureData);

  glBindTexture(GL_TEXTURE_2D, 0);

  texture.id = textureID;
  texture.width = 1;
  texture.height = 1;
  texture.format = 4;
  texture.mipmap = 1;
  bValid = true;

  return texture;
}

static void InternalDrawPrimitive(FPrimitive Primitive, FVector3 Location, FVector3 Scale, FColor Color) {
  glUseProgram(SDefaultShader);

  FMat4 mTransform = FMat4MakeTranslation(Location);
  FMat4 mScale = FMat4MakeScale(Scale);
  FMat4 mModel = FMat4Mul(mTransform, mScale);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, FTextureWhite().id);
  glUniform4fv(SPrimitiveInfo.uColorID, 1, (const float*)&Color);
  glUniformMatrix4fv(SPrimitiveInfo.uProjID, 1, GL_FALSE, (const float*)FGetProjection(E_PROJECTION_ORTHOGRAPHIC).e);
  glUniformMatrix4fv(SPrimitiveInfo.uModelID, 1, GL_FALSE, (const float*)mModel.e);

  glBindVertexArray(Primitive.objectID);
  glDrawElements(GL_TRIANGLES, Primitive.indices, GL_UNSIGNED_INT, NULL);
  glBindVertexArray(0);
  glUseProgram(0);
}

void FDrawCircle(FVector3 Location, FVector3 Scale, FColor Color) {
  FPrimitive circle = FGetPrimitive(E_SHAPE_CIRCLE);
  InternalDrawPrimitive(circle, Location, Scale, Color);
}

void FDrawQuad(FVector3 Location, FVector3 Scale, FColor Color) {
  FPrimitive quad = FGetPrimitive(E_SHAPE_QUAD);
  InternalDrawPrimitive(quad, Location, Scale, Color);
}

// Intenal Functions
static FPrimitive InternalCreateQuad() {
  static bool bCreated = false;
  static FPrimitive primitive = {0};

  if(bCreated) {
    return primitive;
  }

  uint32 VAO, VBO, EBO;

  // clang-format off
  float vertices[] = {
    /*  Position     */  /*Tex Coord*/
       1.f,  1.f,  0.f,    1.f, 1.f,   // Top Right
       1.f, -1.f,  0.f,    1.f, 0.f,   // Bottom Right
      -1.f, -1.f,  0.f,    0.f, 0.f,   // Bottom Left
      -1.f,  1.f,  0.f,    0.f, 1.f,   // Top Left
  };

  int32 indices[] = {0, 1, 2, 2, 3, 0};
  // clang-format on

  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), NULL);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  bCreated = true;
  primitive.objectID = VAO;
  primitive.indices = 6;
  primitive.shape = E_SHAPE_QUAD;

  return primitive;
}

static FPrimitive InternalCreateCircle() {
  static bool bCreated = false;
  static FPrimitive primitive = {0};

  if(bCreated) {
    return primitive;
  }

  uint32 VAO, VBO, EBO, bCircle;
  const uint32 SEGMENTS = 100;
  const float RADIUS = 0.5;

  // clang-format off
  float vertices[(SEGMENTS + 1) * 2];
  float texCoord[(SEGMENTS + 1) * 2];
  int32 indices[SEGMENTS * 3];

  vertices[0] = 0.f;
  vertices[1] = 0.f;
  texCoord[0] = 0.5f;
  texCoord[1] = 0.5f;

  for(int32 i = 0; i <= SEGMENTS; i ++){
    float theta = 2.0f * M_PI * (float)i / (float)SEGMENTS;
    vertices[2 * (i + 1)] = RADIUS * cosf(theta);     // X
    vertices[2 * (i + 1) + 1] = RADIUS * sinf(theta); // Y

    texCoord[2 * (i + 1)] = 0.5f + 0.5f * cosf(theta);
    texCoord[2 * (i + 1) + 1] = 0.5f + 0.5f * sinf(theta);

    if(i < SEGMENTS){
      indices[i * 3] = 0;
      indices[i * 3 + 1] = i  + 1;
      indices[i * 3 + 2] = (i + 1) % SEGMENTS + 1;
    }
  }
  // clang-format on

  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

  glGenBuffers(1, &bCircle);
  glBindBuffer(GL_ARRAY_BUFFER, bCircle);
  glBufferData(GL_ARRAY_BUFFER, sizeof(texCoord), texCoord, GL_STATIC_DRAW);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  bCreated = true;
  primitive.objectID = VAO;
  primitive.indices = SEGMENTS * 3;
  primitive.shape = E_SHAPE_CIRCLE;

  return primitive;
}

#endif  // USE_OPENGL
