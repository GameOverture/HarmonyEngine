#version 330

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 uv;

smooth out vec4 interpColor;
smooth out vec2 interpUV;

uniform vec4 textColor;
uniform mat4 transformMtx;
uniform mat4 cameraToClipMatrix;
uniform mat4 worldToCameraMatrix;

void main()
{
	interpColor	= textColor;
	interpUV	= uv;
	
	vec4 temp = transformMtx * position;
	temp = worldToCameraMatrix * temp;
	gl_Position = cameraToClipMatrix * temp;
}