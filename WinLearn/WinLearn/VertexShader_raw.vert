#version 330

layout(location = 0) in vec3 VertexPosition;    
uniform mat4 gMVP;

void main()    
{       
    gl_Position = gMVP * vec4(VertexPosition, 1.0);
}    