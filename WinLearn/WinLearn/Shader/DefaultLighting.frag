#version 330       

 in vec2 out_texcoord;
 out vec4 FragColor;  
 uniform sampler2D tex;


 void main()
 {
    vec4 c = texture2D(tex,out_texcoord);
	FragColor = vec4(c.rgb, 1.0) ;//+ out_lightColor;
	//FragColor = gLightColor;
 }