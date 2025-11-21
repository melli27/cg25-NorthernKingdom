#include "Texture.h"

Texture::Texture()
{
}

Texture::~Texture()
{
	unbind();
	glDeleteTextures(1, &handle);
}

void Texture::loadFromFile(const char* texturePath)
{
	path = texturePath;
	glGenTextures(1, &handle);
	glBindTexture(GL_TEXTURE_2D, handle); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object

	unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0); // TODO: use DDSimage also here

	if (data)
	{
		GLenum format;
		if (nrChannels == 1) {
			format = GL_RED;
		}
		else if (nrChannels == 3) {
			format = GL_RGB;
		}
		else if (nrChannels == 4) {
			format = GL_RGBA;
		}
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		std::cout << "Loaded texture of size " << height << " x " << width << std::endl;
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
		std::cout << "Failed to load texture at path: " << path << std::endl;
	}
	stbi_image_free(data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);		// trilinear filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);					// bilinear filtering
}

void Texture::bind(int location)
{
	glActiveTexture(GL_TEXTURE0 + location);
	glBindTexture(GL_TEXTURE_2D, handle);
}

void Texture::unbind()
{
	//TODO
}
