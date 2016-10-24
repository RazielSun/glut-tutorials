
#include <iostream>
#include "utils.h"
#include "cubemap_texture.h"

static const GLenum types[6] = {  GL_TEXTURE_CUBE_MAP_POSITIVE_X,
                                  GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
                                  GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
                                  GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
                                  GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
                                  GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };

CubemapTexture::CubemapTexture(const std::string& dir,
		const std::string& posXf,
		const std::string& negXf,
		const std::string& posYf,
		const std::string& negYf,
		const std::string& posZf,
		const std::string& negZf)
{
	std::string::const_iterator it = dir.end();
    it--;
    std::string baseDir = (*it == '/') ? dir : dir + "/";
    
    m_fileNames[0] = baseDir + posXf;
    m_fileNames[1] = baseDir + negXf;
    m_fileNames[2] = baseDir + posYf;
    m_fileNames[3] = baseDir + negYf;
    m_fileNames[4] = baseDir + posZf;
    m_fileNames[5] = baseDir + negZf;

	m_texture = 0;
}

CubemapTexture::~CubemapTexture()
{
	if (m_texture != 0) {
        glDeleteTextures(1, &m_texture);
    }
}

bool CubemapTexture::Load()
{
	glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);

    Magick::Image* pImage = NULL;
    Magick::Blob blob;

    for (unsigned int i = 0 ; i < ARRAY_SIZE_IN_ELEMENTS(types) ; i++) {
        pImage = new Magick::Image(m_fileNames[i]);
        
        try {            
            pImage->write(&blob, "RGBA");
        }
        catch (Magick::Error& Error) {
            std::cout << "Error loading texture '" << m_fileNames[i] << "': " << Error.what() << std::endl;
            delete pImage;
            return false;
        }

        glTexImage2D(types[i], 0, GL_RGB, pImage->columns(), pImage->rows(), 0, GL_RGBA, GL_UNSIGNED_BYTE, blob.data());
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);           
        
        delete pImage;
    }    
      
    return true;
}

void CubemapTexture::Bind(GLenum textureUnit)
{
	glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);
}
