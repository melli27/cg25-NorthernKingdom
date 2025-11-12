/*
* Copyright 2020 Vienna University of Technology.
* Institute of Computer Graphics and Algorithms.
* This file is part of the ECG Lab Framework and must not be redistributed.
*/
#pragma once

#include <string>
#include "Utils/Utils.h"

/*!
 * 2D texture
 */
class Texture
{
protected:
	GLuint _handle;
	bool _init;

public:
	/*!
	 * Creates a texture from a file
	 * @param file: path to the texture file (a DSS image)
	 */
	Texture(const char* texturePath);
	~Texture();

	void genTexture(const char* texturePath);

	/*!
	 * Activates the texture unit and binds this texture
	 * @param unit: the texture unit
	 */
	void bind(unsigned int unit);
};
