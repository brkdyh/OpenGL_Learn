#include <Material.h>

Material::Material(const std::string &c_mainTexName, const std::string &c_vertexShader, const std::string &c_fragmentShader)
{
	mainTexName = c_mainTexName;
	vertexShaderName = c_vertexShader;
	FragmentShaderName = c_fragmentShader;
}

void Material::LoadTexture()
{
	if (!loaded)
	{
		mainTex = new Texture(GL_TEXTURE_2D, mainTexName, "tga");
		mainTex->Load();
	}
}

void Material::Bind()
{
	mainTex->Bind(GL_TEXTURE0);
}
