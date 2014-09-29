#version 330

smooth in vec4 interpColor;
smooth in vec2 interpUV;

uniform sampler2D Tex;

out vec4 outputColor;

void main()
{
	// Blend interpColor with whatever texel I get from interpUV
	vec4 texelClr = texture(Tex, interpUV);
	outputColor = interpColor * texelClr.x;
}
