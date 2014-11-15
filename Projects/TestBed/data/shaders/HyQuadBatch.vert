#version 330

layout(location = 0) in vec4 unitQuadPos;

layout(location = 1) in vec2 size
layout(location = 2) in vec2 offset;
layout(location = 3) in vec4 tint;
layout(location = 4) in vec2 UVcoord0;
layout(location = 5) in vec2 UVcoord1;
layout(location = 6) in vec2 UVcoord2;
layout(location = 7) in vec2 UVcoord3;
layout(location = 8) in mat4 mtxLocalToWorld;

smooth out vec4 interpColor;
smooth out vec2 interpUV;

uniform mat4 mtxCameraToClipMatrix;
uniform mat4 mtxWorldToCameraMatrix;

void main()
{
	switch(gl_VertexID)
	{
	case 0:	interpUV = UVCoord0;	break;
	case 1:	interpUV = UVCoord1;	break;
	case 2:	interpUV = UVCoord2;	break;
	case 3:	interpUV = UVCoord3;	break;
	}

	interpColor	= tint;

	position.x *= size.x;
	position.y *= size.y;
	position.x += offset.x;
	position.y += offset.y;
	
	position = (localToWorld * position);
	position = worldToCameraMatrix * position;

	gl_Position = cameraToClipMatrix * position;
}