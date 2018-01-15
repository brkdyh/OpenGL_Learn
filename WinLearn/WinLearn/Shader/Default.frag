#version 330       

 in vec2 out_texcoord;
 out vec4 FragColor;  
 uniform sampler2D tex;

 void main()
 {
    vec4 c = texture2D(tex,out_texcoord);
	FragColor = vec4(c.rgb, 1.0);
	//FragColor = vec4(1.0,0.0,0.0,1.0);
	//FragColor = vec4(out_texcoord,0.0,1.0);
 }