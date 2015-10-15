/**************************************************************************
 *	HyOpenGLShaderSrc.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2015 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyOpenGLShaderSrc_h__
#define __HyOpenGLShaderSrc_h__

#include "Afx/HyStdAfx.h"

const char *szHYQUADBATCH_VERTEXSHADER = "#version 330						\n\
																			\n\
																			\n\
layout(location = 0) in vec2 size;											\n\
layout(location = 1) in vec2 offset;										\n\
layout(location = 2) in vec4 tint;											\n\
layout(location = 3) in int textureIndex;									\n\
layout(location = 4) in vec2 UVcoord0;										\n\
layout(location = 5) in vec2 UVcoord1;										\n\
layout(location = 6) in vec2 UVcoord2;										\n\
layout(location = 7) in vec2 UVcoord3;										\n\
layout(location = 8) in mat4 mtxLocalToWorld;								\n\
																			\n\
																			\n\
smooth out vec4 interpColor;												\n\
smooth out vec3 interpUV;													\n\
																			\n\
uniform mat4 mtxCameraToClipMatrix;											\n\
uniform mat4 mtxWorldToCameraMatrix;										\n\
//uniform mat4 mtxLocalToWorld;												\n\
																			\n\
																			\n\
const vec2 position[] = vec2[4](											\n\
	vec2(1.0f, 1.0f),														\n\
	vec2(0.0f, 1.0f),														\n\
	vec2(1.0f, 0.0f),														\n\
	vec2(0.0f, 0.0f)														\n\
	);																		\n\
																			\n\
/* const vec2 position[] = vec2[4](				\n\
	vec2(0.0f, 0.0f), \n\
	vec2(0.0f, 1.0f), \n\
	vec2(1.0f, 0.0f), \n\
	vec2(1.0f, 1.0f)														\n\
	);*/																	\n\
																			\n\
																			\n\
void main()																	\n\
{																			\n\
	/*																		\n\
	vec4 tmp;																\n\
	tmp.x = pos[gl_VertexID].x;												\n\
	tmp.y = pos[gl_VertexID].y;												\n\
	tmp.z = 0.0f;															\n\
	tmp.w = 1.0f;															\n\
																			\n\
	vec4 temp = mtxLocalToWorld * tmp;										\n\
	temp = mtxWorldToCameraMatrix * temp;									\n\
	gl_Position = mtxCameraToClipMatrix * temp;								\n\
	*/																		\n\
																			\n\
	switch(gl_VertexID)														\n\
	{																		\n\
	case 0:																	\n\
		interpUV.x = UVcoord0.x;											\n\
		interpUV.y = UVcoord0.y;											\n\
		break;																\n\
	case 1:																	\n\
		interpUV.x = UVcoord1.x;											\n\
		interpUV.y = UVcoord1.y;											\n\
		break;																\n\
	case 2:																	\n\
		interpUV.x = UVcoord2.x;											\n\
		interpUV.y = UVcoord2.y;											\n\
		break;																\n\
	case 3:																	\n\
		interpUV.x = UVcoord3.x;											\n\
		interpUV.y = UVcoord3.y;											\n\
		break;																\n\
	}																		\n\
	interpUV.z = textureIndex;												\n\
																			\n\
	interpColor = tint;														\n\
																			\n\
	//gl_Position.x = position[gl_VertexID].x;								\n\
	//gl_Position.y = position[gl_VertexID].y;								\n\
	//gl_Position.z = 0.0f;													\n\
	//gl_Position.w = 1.0f;													\n\
																			\n\
																			\n\
	vec4 pos = vec4((position[gl_VertexID].x * size.x) + offset.x,			\n\
	(position[gl_VertexID].y * size.y) + offset.y, 							\n\
		0.0, 1.0);															\n\
																			\n\
	//pos.x = position[gl_VertexID].x;										\n\
	//pos.y = position[gl_VertexID].y;										\n\
	//pos.z = 0.0f;															\n\
	//pos.w = 1.0f;															\n\
																			\n\
	pos = mtxLocalToWorld * pos;											\n\
	pos = mtxWorldToCameraMatrix * pos;										\n\
	gl_Position = mtxCameraToClipMatrix * pos;								\n\
}";

const char *szHYQUADBATCH_FRAGMENTSHADER = "								\n\
#version 330																\n\
																			\n\
smooth in vec4 interpColor;													\n\
smooth in vec3 interpUV;													\n\
																			\n\
uniform sampler2DArray Tex;													\n\
																			\n\
out vec4 outputColor;														\n\
																			\n\
void main()																	\n\
{																			\n\
	// Blend interpColor with whatever texel I get from interpUV			\n\
	vec4 texelClr = texture(Tex, interpUV);									\n\
	outputColor = interpColor * texelClr;									\n\
}";

const char *szHYPRIMATIVE_VERTEXSHADER = "									\n\
#version 400																\n\
																			\n\
layout(location = 0) in vec4 position;										\n\
																			\n\
out vec4 Color;																\n\
																			\n\
uniform mat4 transformMtx;													\n\
uniform mat4 cameraToClipMatrix;											\n\
uniform mat4 worldToCameraMatrix;											\n\
uniform vec4 primitiveColor;												\n\
																			\n\
void main()																	\n\
{																			\n\
	Color = primitiveColor;													\n\
																			\n\
	vec4 temp = transformMtx * position;									\n\
	temp = worldToCameraMatrix * temp;										\n\
	gl_Position = cameraToClipMatrix * temp;								\n\
}";

const char *szHYPRIMATIVE_FRAGMENTSHADER = "								\n\
#version 400																\n\
																			\n\
in vec4 Color;																\n\
out vec4 FragColor;															\n\
																			\n\
void main()																	\n\
{																			\n\
	FragColor = Color;														\n\
}";

#endif __HyOpenGLShaderSrc_h__
