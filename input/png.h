#include "graphics.h"

#ifndef PNG_H
#define PNG_H

class PNG {
	int width;
	int height;
	int channels;
	uint32_t *img;

public:
	PNG(const char *imagePath);
	~PNG();

	uint32_t *GetImgData(int *width = NULL, int *height = NULL);
	void Draw(Scene2D *scene, int startX, int startY);
};

#endif
