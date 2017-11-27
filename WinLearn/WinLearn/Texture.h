#pragma once
#ifndef TEXTURE_H
#define TEXTURE_H

#include <glew.h>
#include <string>

class Texture
{

public:
	Texture(GLenum TextureTarget, const std::string &FileName);
	bool Load();
	void Bind(GLenum TextureUnit);
};

#endif