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

const char * const szHYQUADBATCH_VERTEXSHADER = "							\n\
#version 420																\n\
																			\n\
layout(location = 0) in vec2 size;											\n\
layout(location = 1) in vec2 offset;										\n\
layout(location = 2) in vec4 topTint;										\n\
layout(location = 2) in vec4 botTint;										\n\
layout(location = 3) in float textureIndex;									\n\
layout(location = 4) in vec2 UVcoord0;										\n\
layout(location = 5) in vec2 UVcoord1;										\n\
layout(location = 6) in vec2 UVcoord2;										\n\
layout(location = 7) in vec2 UVcoord3;										\n\
layout(location = 8) in mat4 mtxLocalToWorld;								\n\
																			\n\
smooth out vec4 interpColor;												\n\
smooth out vec2 interpUV;													\n\
flat out float texIndex;													\n\
																			\n\
uniform mat4 mtxCameraToClip;												\n\
uniform mat4 mtxWorldToCamera;												\n\
																			\n\
const vec2 position[] = vec2[4](											\n\
	vec2(1.0f, 1.0f),														\n\
	vec2(0.0f, 1.0f),														\n\
	vec2(1.0f, 0.0f),														\n\
	vec2(0.0f, 0.0f)														\n\
	);																		\n\
																			\n\
void main()																	\n\
{																			\n\
	switch(gl_VertexID)														\n\
	{																		\n\
	case 0:																	\n\
		interpUV.x = UVcoord0.x;											\n\
		interpUV.y = UVcoord0.y;											\n\
																			\n\
		interpColor = botTint;												\n\
		break;																\n\
	case 1:																	\n\
		interpUV.x = UVcoord1.x;											\n\
		interpUV.y = UVcoord1.y;											\n\
																			\n\
		interpColor = botTint;												\n\
		break;																\n\
	case 2:																	\n\
		interpUV.x = UVcoord2.x;											\n\
		interpUV.y = UVcoord2.y;											\n\
																			\n\
		interpColor = topTint;												\n\
		break;																\n\
	case 3:																	\n\
		interpUV.x = UVcoord3.x;											\n\
		interpUV.y = UVcoord3.y;											\n\
																			\n\
		interpColor = topTint;												\n\
		break;																\n\
	}																		\n\
	texIndex = textureIndex;												\n\
																			\n\
																			\n\
	vec4 pos = vec4((position[gl_VertexID].x * size.x) + offset.x,			\n\
					(position[gl_VertexID].y * size.y) + offset.y, 			\n\
					0.0, 1.0);												\n\
																			\n\
	pos = mtxLocalToWorld * pos;											\n\
	pos = mtxWorldToCamera * pos;											\n\
	gl_Position = mtxCameraToClip * pos;									\n\
}";

const char * const szHYQUADBATCH_FRAGMENTSHADER = "							\n\
#version 420																\n\
																			\n\
smooth in vec4 interpColor;													\n\
smooth in vec2 interpUV;													\n\
flat in float texIndex;														\n\
																			\n\
uniform sampler2DArray Tex;													\n\
																			\n\
out vec4 outputColor;														\n\
																			\n\
void main()																	\n\
{																			\n\
	// Blend interpColor with whatever texel I get from interpUV			\n\
	vec4 texelClr = texture(Tex, vec3(interpUV.x, interpUV.y, texIndex));	\n\
																			\n\
	outputColor = interpColor * texelClr;									\n\
}";

const char * const szHYPRIMATIVE_VERTEXSHADER = "							\n\
#version 400																\n\
																			\n\
layout(location = 0) in vec4 position;										\n\
																			\n\
out vec4 Color;																\n\
																			\n\
uniform mat4 transformMtx;													\n\
uniform mat4 mtxCameraToClip;												\n\
uniform mat4 mtxWorldToCamera;												\n\
uniform vec4 primitiveTopColor;												\n\
uniform vec4 primitiveBotColor;												\n\
																			\n\
void main()																	\n\
{																			\n\
	Color = primitiveTopColor;												\n\
																			\n\
	vec4 temp = transformMtx * position;									\n\
	temp = mtxWorldToCamera * temp;											\n\
	gl_Position = mtxCameraToClip * temp;									\n\
}";

const char * const szHYPRIMATIVE_FRAGMENTSHADER = "							\n\
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
