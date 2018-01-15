#pragma once
#ifndef TEXTURE_H
#define TEXTURE_H

#include <glew.h>
#include <string>
#include "TgaProcesser.h"

class Texture
{

public:
	Texture(GLenum TextureTarget, const std::string &FileName, const std::string &fileType);
	bool Load();
	void Bind(GLenum TextureUnit);
	void Unbind();

public:
	std::string m_fileName;
	GLenum m_textureTarget;
	GLuint m_textureObj;
	TGAFILE *tgaFile;
	bool bLoaded;
	int texErrorState;
};

#endif