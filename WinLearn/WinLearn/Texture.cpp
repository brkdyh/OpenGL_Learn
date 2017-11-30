#include "Texture.h"
#include <string.h>
#include <Magick++.h>
#include <iostream>
#include <glew.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <sstream>
#include <TgaProcesser.h>;
using namespace std;

	Texture::Texture(GLenum TextureTarget, const std::string &FileName,const std::string &fileType)
	{
		m_fileName = FileName;
		m_fileName = m_fileName.append(".");
		m_fileName = m_fileName.append(fileType);
		m_textureTarget = TextureTarget;
	};

	bool Texture::Load()
	{
		if (texErrorState == 1 || bLoaded)
		{
			return false;
		}

		try
		{
			tgaFile = (TGAFILE*)malloc(sizeof(TGAFILE));
			LoadTGAFile(m_fileName, tgaFile);

			glGenTextures(1, &m_textureObj);
			glBindTexture(m_textureTarget, m_textureObj);
			glTexParameterf(m_textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(m_textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			glTexImage2D(m_textureTarget, 0, GL_RGBA, tgaFile->imageWidth, tgaFile->imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, tgaFile->imageData);
			//glTexImage2D(m_textureTarget, 0, GL_RGBA, m_pImage->columns(), m_pImage->rows(), 0, GL_RGBA, GL_UNSIGNED_BYTE, m_blob.data());

			//ostringstream oss;
			//oss << "loading texture Info'" << m_fileName << "'";
			//EasyLog::Inst()->Log(oss.str());
			bLoaded = true;
			return true;
		}
		catch (Magick::Error& Error)
		{
			//ostringstream oss;
			//oss << "Error loading texture '" << m_fileName << "': " << Error.what();
			//EasyLog::Inst()->Log(oss.str());
			bLoaded = false;
			texErrorState = 1;
			return false;

		}
	}

	void Texture::Bind(GLenum TextureUnit)
	{
		//glActiveTexture(TextureUnit);
		glBindTexture(m_textureTarget, m_textureObj);
		GLenum o = TextureUnit;
		int i = 0;
	}