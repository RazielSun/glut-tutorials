
#ifndef CUBEMAP_TEXTURE_H
#define CUBEMAP_TEXTURE_H

#include <string>
#include <GL/glew.h>
#include <ImageMagick-6/Magick++.h>

class CubemapTexture {
public:
	CubemapTexture(const std::string& dir,
		const std::string& posXf,
		const std::string& negXf,
		const std::string& posYf,
		const std::string& negYf,
		const std::string& posZf,
		const std::string& negZf);

	~CubemapTexture();

	bool Load();
	void Bind(GLenum textureUnit);

private:
	std::string m_fileName[6];
	GLuint m_texture;
};

#endif /* CUBEMAP_TEXTURE_H */