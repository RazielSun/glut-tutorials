
#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>
#include <ImageMagick-6/Magick++.h>

class Texture {
public:
	Texture(GLenum textureTarget, const std::string& fileName);
	bool Load();
	void Bind(GLenum textureUnit);

private:
	GLenum m_textureTarget;
	std::string m_fileName;
	GLuint m_textureObj;
    Magick::Image m_image;
    Magick::Blob m_blob;
};

#endif /* TEXTURE_H */