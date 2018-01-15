#version 330

layout(location = 0) in vec3 VertexPosition; 
layout(location = 1) in vec2 TexCoord;

uniform mat4 gMV;
uniform mat4 gP;

out vec2 out_texcoord;

void main()    
{       
    gl_Position = gP*(gMV*vec4(VertexPosition, 1.0));
	out_texcoord = TexCoord;
}    