#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "GameFramework.h"
#include "GT/Platform.h"

FImage* FImageLoadFile(cstring AssetPath) {
  if(AssetPath == NULL){
    GT_LOG(LOG_ALERT, "Asset path is NULL!");
    return NULL;
  }

  int32 width, height, format;
  uchar* stbData = stbi_load(AssetPath, &width, &height, &format, 0);
  if(stbData == NULL) {
    GT_LOG(LOG_ALERT, "Asset not Loaded -> %s", AssetPath);
    return NULL;  // TODO: return default image;
  }
  uint64 imageSize = width * height * format;
  uint64 totalSize = sizeof(FImage) + imageSize;
  FImage* image = PMemCalloc(1, totalSize);
  image->width = width;
  image->height = height;
  image->format = format;
  image->data = (char*)image + sizeof(FImage);
  PMemCopy(image->data, stbData, imageSize);

  stbi_image_free(stbData);
  cstring path = strstr(AssetPath, "Content");
  path = (path != NULL) ? path : AssetPath;
  GT_LOG(LOG_INFO, "FImage loaded => Width:%d Height:%d Format:%d -> %s", width, height, format, path);
  return image;
}

void FImageUnload(FImage* Image) {
  if(Image == NULL) { return; }
  PMemFree(Image);
}
