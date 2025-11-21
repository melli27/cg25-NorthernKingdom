#pragma once
#include "Utils/Utils.h"

class Texture {
private:

	int width;
	int height;
	int nrChannels;

public:

	GLuint handle;
	const char* path;
	string type;

	Texture();
	~Texture();
	void loadFromFile(const char* texturePath);
	void bind(int location);
	void unbind();

};