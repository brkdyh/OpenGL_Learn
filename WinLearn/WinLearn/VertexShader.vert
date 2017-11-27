in vec3 Position;
in vec3 Color ;
in vec2 TexCoord ;
uniform mat4 gWVP;
out vec3 out_color;
out vec2 out_texcoord;
 
 void main()
 {
    //gl_Position = gWVP * vec4(Position, 1.0);
	gl_Position = vec4(Position, 1.0);
    out_color = Color;
    out_texcoord = TexCoord;
}