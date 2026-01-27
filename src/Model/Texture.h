#pragma once
#include "Utils/Utils.h"

class Texture {
private:

public:

	unsigned int handle;
	const char* path;
	string type;
	int width;
	int height;
	int nrChannels;

	unsigned char* imageData = nullptr;

	Texture();
	~Texture();
	void loadFromFile(const char* texturePath);
	void loadDDS(const char* texturePath);
	void loadSTBI(const char* texturePath);
	void bind(int location);

	void loadSTBI_Height16(const char* filepath);

	// Add helper to get pixel value
	float getPixelValue(int x, int y) const;

};