#version 330

in vec4 VertexPosition;    
in vec4 VertexColor;  
    
out vec4 Color;    
    
void main()    
{    
    Color = VertexColor;    
    gl_Position = VertexPosition;
}    