#define STBI_ASSERT(x)
#define STB_IMAGE_IMPLEMENTATION
#include "png.h"

#include "graphics.h"
#include "log.h"

#include <stb/stb_image.h>

PNG::PNG(const char* imagePath) {
  this->img = (uint32_t*)stbi_load(imagePath, &this->width, &this->height, &this->channels, STBI_rgb_alpha);

  if (this->img == NULL) {
    DEBUGLOG << "Failed to load image '" << imagePath << "': " << stbi_failure_reason();
    return;
  }
}

PNG::~PNG() {
  if (this->img != NULL) stbi_image_free(this->img);
}

uint32_t* PNG::GetImgData(int32_t* width, int32_t* height) {
  if (width != NULL) *width = this->width;
  if (height != NULL) *height = this->height;
  return this->img;
}

void PNG::Draw(Scene2D* scene, int32_t startX, int32_t startY) {
  // Don't draw non-existant images
  if (this->img == NULL) return;

  // Iterate the bitmap and draw the pixels
  for (int32_t yPos = 0; yPos < this->height; yPos++) {
    for (int32_t xPos = 0; xPos < this->width; xPos++) {
      // Decode the 32-bit color
      uint32_t encodedColor = this->img[(yPos * this->width) + xPos];

      // Get new pixel coordinates to account for start coordinates
      int32_t x = startX + xPos;
      int32_t y = startY + yPos;

      // Re-encode the color
      uint8_t r = (uint8_t)(encodedColor >> 0);
      uint8_t g = (uint8_t)(encodedColor >> 8);
      uint8_t b = (uint8_t)(encodedColor >> 16);

      Color color = {r, g, b};

      // Do some bounds checking to make sure the pixel is actually inside the
      // frame buffer
      if (xPos < 0 || yPos < 0 || xPos >= this->width || yPos >= this->height) continue;

      scene->DrawPixel(x, y, color);
    }
  }
}
