#version 330

layout(location = 0) in vec2 size;
layout(location = 1) in vec2 offset;
layout(location = 2) in vec4 tint;
layout(location = 3) in vec2 UVcoord0;
layout(location = 4) in vec2 UVcoord1;
layout(location = 5) in vec2 UVcoord2;
layout(location = 6) in vec2 UVcoord3;
layout(location = 7) in mat4 mtxLocalToWorld;

/*
layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 uv;
*/

smooth out vec4 interpColor;
smooth out vec2 interpUV;

uniform mat4 mtxCameraToClipMatrix;
uniform mat4 mtxWorldToCameraMatrix;
//uniform mat4 mtxLocalToWorld;


const vec2 position[] = vec2[4](
  vec2(0.0f, 0.0f),
  vec2(0.0f, 1.0f),
  vec2(1.0f, 0.0f),
  vec2(1.0f, 1.0f)
);


void main()
{
/*
	vec4 tmp;
	tmp.x = pos[gl_VertexID].x;
	tmp.y = pos[gl_VertexID].y;
	tmp.z = 0.0f;
	tmp.w = 1.0f;

	vec4 temp = mtxLocalToWorld * tmp;
	temp = mtxWorldToCameraMatrix * temp;
	gl_Position = mtxCameraToClipMatrix * temp;
*/

	switch(gl_VertexID)
	{
	case 0:
		interpUV = UVcoord0;
		break;
	case 1:
		interpUV = UVcoord1;
		break;
	case 2:
		interpUV = UVcoord2;
		break;
	case 3:
		interpUV = UVcoord3;
		break;
	}

	interpColor	= tint;

	//gl_Position.x = position[gl_VertexID].x;
	//gl_Position.y = position[gl_VertexID].y;
	//gl_Position.z = 0.0f;
	//gl_Position.w = 1.0f;


	vec4 pos = vec4( (position[gl_VertexID].x * size.x) + offset.x,
			  (position[gl_VertexID].y * size.y) + offset.y,
			  0.0, 1.0);

	//pos.x = position[gl_VertexID].x;
	//pos.y = position[gl_VertexID].y;
	//pos.z = 0.0f;
	//pos.w = 1.0f;

	pos = mtxLocalToWorld * pos;
	pos = mtxWorldToCameraMatrix * pos;
	gl_Position = mtxCameraToClipMatrix * pos;
}

