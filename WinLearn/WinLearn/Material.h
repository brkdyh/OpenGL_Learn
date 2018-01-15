#pragma once

#ifndef MATERIAL_H  
#define MATERIAL_H

#include "Texture.h"

class Material
{
public:
	std::string mainTexName;
	Texture *mainTex;
	std::string vertexShaderName;
	std::string FragmentShaderName;
	bool loaded;

	Material(const std::string &c_mainTexName, const std::string &c_vertexShader, const std::string &c_fragmentShader);
	void LoadTexture();
	void Bind();
};

#endif