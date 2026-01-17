#include "Texture.h"

Texture::Texture()
{
}

void Texture::loadFromFile(const char* texturePath)
{
	path = texturePath;
	glGenTextures(1, &handle);
	glBindTexture(GL_TEXTURE_2D, handle); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object

	// Check file extension
	const char* ext = strrchr(texturePath, '.');
	if (ext && strcmp(ext, ".dds") == 0) {
		loadDDS(texturePath);
	}
	else {
		loadSTBI(texturePath);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);		// trilinear filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);					// bilinear filtering
}

void Texture::loadSTBI(const char* filepath) {
	unsigned char* data = stbi_load(filepath, &width, &height, &nrChannels, 0);

	if (data) {
		GLenum format;
		if (nrChannels == 1) format = GL_RED;
		else if (nrChannels == 3) format = GL_RGB;
		else if (nrChannels == 4) format = GL_RGBA;

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		std::cout << "Loaded texture from: " << filepath << " size: " << height << "x" << width
			<< " with " << nrChannels << " channels" << std::endl;
	}
	else {
		std::cout << "Failed to load texture: " << filepath << std::endl;
	}

	stbi_image_free(data);
}


void Texture::loadDDS(const char* filepath) {
	FILE* f = fopen(filepath, "rb");
	if (!f) {
		std::cout << "Failed to open DDS file: " << filepath << std::endl;
		return;
	}

	// Read header (4 byte magic + 124 byte header)
	unsigned char header[128];
	fread(header, 1, 128, f);

	// Verify DDS signature "DDS "
	if (memcmp(header, "DDS ", 4) != 0) {
		std::cout << "Invalid DDS file signature" << std::endl;
		fclose(f);
		return;
	}

	// Extract dimensions and mipmap count
	height = (header[12]) | (header[13] << 8) | (header[14] << 16) | (header[15] << 24);
	width = (header[16]) | (header[17] << 8) | (header[18] << 16) | (header[19] << 24);
	unsigned int mipMapCount = (header[28]) | (header[29] << 8) | (header[30] << 16) | (header[31] << 24);

	if (mipMapCount < 1) mipMapCount = 1;

	// Determine format and block size
	GLenum format;
	unsigned int blockSize;

	if (header[84] == 'D') {
		switch (header[87]) {
		case '1': // DXT1
			format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			blockSize = 8;
			break;
		case '3': // DXT3
			format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			blockSize = 16;
			break;
		case '5': // DXT5
			format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			blockSize = 16;
			break;
		default:
			std::cout << "Unsupported DDS format" << std::endl;
			fclose(f);
			return;
		}
	}
	else {
		std::cout << "Unsupported DDS compression" << std::endl;
		fclose(f);
		return;
	}

	// Get file size and read pixel data
	fseek(f, 0, SEEK_END);
	long fileSize = ftell(f);
	fseek(f, 128, SEEK_SET); // Skip header

	unsigned int dataSize = fileSize - 128;
	unsigned char* buffer = new unsigned char[dataSize];
	fread(buffer, 1, dataSize, f);
	fclose(f);

	// Upload all mipmap levels
	unsigned int offset = 0;
	unsigned int w = width;
	unsigned int h = height;

	for (unsigned int i = 0; i < mipMapCount; i++) {
		if (w == 0 || h == 0) break;

		unsigned int size = ((w + 3) / 4) * ((h + 3) / 4) * blockSize;
		glCompressedTexImage2D(GL_TEXTURE_2D, i, format, w, h, 0, size, buffer + offset);

		offset += size;
		w /= 2;
		h /= 2;
	}

	delete[] buffer;
	std::cout << "Loaded DDS texture " << width << "x" << height
		<< " with " << mipMapCount << " mipmaps" << std::endl;
}

void Texture::bind(int location)
{
	glActiveTexture(GL_TEXTURE0 + location);
	glBindTexture(GL_TEXTURE_2D, handle);
}
