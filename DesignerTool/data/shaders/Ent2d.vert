#version 330

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 uv;

smooth out vec4 interpColor;
smooth out vec2 interpUV;

uniform mat4 cameraToClipMatrix;
uniform mat4 worldToCameraMatrix;
uniform mat4 localToWorld;

void main()
{
	interpUV	= uv;
	interpColor	= color;
	
	vec4 temp = localToWorld * position;
	temp = worldToCameraMatrix * temp;
	gl_Position = cameraToClipMatrix * temp;
}