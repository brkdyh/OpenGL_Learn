 #version 330
 out vec4 FragColor;
 in vec3 out_color;
 in vec2 out_texcoord;
 uniform sampler2D gSampler;
 void main()
 {
    vec4 c = texture2D(gSampler,out_texcoord);
	//FragColor = vec4(out_color+c.rgb, 1.0);
	FragColor = vec4(c.rgb, 1.0);
 }