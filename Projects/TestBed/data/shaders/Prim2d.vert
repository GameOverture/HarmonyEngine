#version 400

layout(location = 0) in vec4 position;

out vec4 Color;

uniform mat4 transformMtx;
uniform mat4 cameraToClipMatrix;
uniform mat4 worldToCameraMatrix;
uniform vec4 primitiveColor;

void main()
{
	Color = primitiveColor;

	vec4 temp = transformMtx * position;
	temp = worldToCameraMatrix * temp;
	gl_Position = cameraToClipMatrix * temp;
}
