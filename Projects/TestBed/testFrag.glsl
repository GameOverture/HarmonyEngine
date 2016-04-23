#version 420																
																			
smooth in vec4 interpColor;													
smooth in vec2 interpUV;													
flat in float texIndex;														
																			
uniform sampler2DArray Tex;													
																			
out vec4 outputColor;														

void main()
{
	// Blend interpColor with whatever texel I get from interpUV
	vec4 texelClr = texture(Tex, vec3(interpUV.x, interpUV.y, texIndex));

	outputColor = vec4(1.0f, 0.0f, 0.0f, 1.0f); //interpColor * texelClr;
}";
