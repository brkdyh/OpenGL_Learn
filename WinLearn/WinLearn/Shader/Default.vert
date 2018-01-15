#version 330

layout(location = 0) in vec3 VertexPosition; 
layout(location = 1) in vec2 TexCoord ;   
 
out vec2 out_texcoord;

uniform Matrix
{
	mat4 gMV;
	mat4 gP;
}matrix;

void main()    
{
    gl_Position = matrix.gP * (matrix.gMV*vec4(VertexPosition, 1.0));
	out_texcoord = TexCoord;
}    