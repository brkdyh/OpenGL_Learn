#include "Texture.h"
#include <string.h>
#include <Magick++.h>
#include <iostream>
#include <glew.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <sstream>
//#include "EasyLog.h"

using namespace Magick; 
using namespace std;

std::string m_fileName;
Blob m_blob;
Magick::Image *m_pImage;
GLenum m_textureTarget;
GLuint m_textureObj;


	Texture::Texture(GLenum TextureTarget, const std::string &FileName)
	{
		m_fileName = FileName;
		m_textureTarget = TextureTarget;
	};

	bool Texture::Load()
	{
		try {
			m_pImage = new Magick::Image(m_fileName);
			m_pImage->write(&m_blob, "RGBA");
			m_pImage->size();

			glGenTextures(1, &m_textureObj);
			glBindTexture(m_textureTarget, m_textureObj);
			glTexParameterf(m_textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(m_textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			glTexImage2D(m_textureTarget, 0, GL_RGBA, m_pImage->columns(), m_pImage->rows(), 0, GL_RGBA, GL_UNSIGNED_BYTE, m_blob.data());
			//std::cout << "loading texture Info'" << m_fileName << "'" << std::endl;
			ostringstream oss;
			oss << "loading texture Info'" << m_fileName << "'";
			//EasyLog::Inst()->Log(oss.str());
			return true;
		}
		catch (Magick::Error& Error) {
			//std::cout << "Error loading texture '" << m_fileName << "': " << Error.what() << std::endl;
			ostringstream oss;
			oss << "Error loading texture '" << m_fileName << "': " << Error.what();
			//EasyLog::Inst()->Log(oss.str());
			return false;

		}
	}

	void Texture::Bind(GLenum TextureUnit)
	{
		//glActiveTexture(TextureUnit);
		glBindTexture(m_textureTarget, m_textureObj);
		//std::cout << "Bind Sucess!" << std::endl;
	}