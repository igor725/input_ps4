#include "graphics.h"

#ifndef PNG_H
#define PNG_H

class PNG {
  int32_t   width;
  int32_t   height;
  int32_t   channels;
  uint32_t* img;

  public:
  PNG(const char* imagePath);
  ~PNG();

  uint32_t* GetImgData(int32_t* width = nullptr, int32_t* height = nullptr);
  void      Draw(Scene2D* scene, int32_t startX, int32_t startY);
};

#endif
