#include "Texture.h"

//std::unordered_map<std::string, std::shared_ptr<Texture>> Texture::textureMap = std::unordered_map<std::string, std::shared_ptr<Texture>>();

Texture::Texture(const char* texturePath)
{
	genTexture(texturePath);
}

void Texture::bind(unsigned int unit)
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, _handle);
}

void Texture::genTexture(const char* texturePath)
{
	glGenTextures(1, &_handle);
	glBindTexture(GL_TEXTURE_2D, _handle); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object

	int height, width, nrChannels;

	unsigned char* data = stbi_load(texturePath, &width, &height, &nrChannels, 0); // TODO: use DDSimage also here

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
	}
	stbi_image_free(data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);		// trilinear filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);					// bilinear filtering
}

