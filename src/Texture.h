#pragma once
#include "Utils/Utils.h"

class Texture {
private:

public:

	unsigned int handle; //TODO maybe GLuint
	const char* path;
	string type;

	int width;
	int height;
	int nrChannels;

	Texture();
	void loadFromFile(const char* texturePath);
	void bind(int location);
	//void unbind();

};