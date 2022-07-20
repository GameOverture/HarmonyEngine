/**************************************************************************
 *	HyOpenGLShaderSrc.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2015 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyOpenGLShaderSrc_h__
#define HyOpenGLShaderSrc_h__

#include "Afx/HyStdAfx.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// QUADBATCH
const char * const szHYQUADBATCH_VERTEXSHADER = R"src(
#version 140
//#extension GL_ARB_explicit_attrib_location : enable

uniform mat4					u_mtxWorldToCamera;
uniform mat4					u_mtxCameraToClip;

//layout(location = 0) in vec2	attr_vSize;
//layout(location = 1) in vec2	attr_vOffset;
//layout(location = 2) in vec4	attr_vTopTint;
//layout(location = 3) in vec4	attr_vBotTint;
//layout(location = 4) in vec2	attr_vUVcoord0;
//layout(location = 5) in vec2	attr_vUVcoord1;
//layout(location = 6) in vec2	attr_vUVcoord2;
//layout(location = 7) in vec2	attr_vUVcoord3;
//layout(location = 8) in mat4	attr_mtxLocalToWorld;

attribute vec2	attr_vSize;
attribute vec2	attr_vOffset;
attribute vec4	attr_vTopTint;
attribute vec4	attr_vBotTint;
attribute vec2	attr_vUVcoord0;
attribute vec2	attr_vUVcoord1;
attribute vec2	attr_vUVcoord2;
attribute vec2	attr_vUVcoord3;
attribute mat4	attr_mtxLocalToWorld;

smooth out vec2					interp_vUV;
smooth out vec4					interp_vColor;

vec2 g_vPOSITION[] = vec2[4](vec2(1.0f, 1.0f),
							 vec2(0.0f, 1.0f),
							 vec2(1.0f, 0.0f),
							 vec2(0.0f, 0.0f));

vec2 g_vUVCOORDS[] = vec2[4](attr_vUVcoord0,
							 attr_vUVcoord1,
							 attr_vUVcoord2,
							 attr_vUVcoord3);

vec4 g_vCOLORS[] = vec4[4](attr_vTopTint,
						   attr_vTopTint,
						   attr_vBotTint,
						   attr_vBotTint);

void main()
{
	interp_vUV = g_vUVCOORDS[gl_VertexID];
	interp_vColor = g_vCOLORS[gl_VertexID];

	vec4 vPos = vec4((g_vPOSITION[gl_VertexID].x * attr_vSize.x) + attr_vOffset.x,
					 (g_vPOSITION[gl_VertexID].y * attr_vSize.y) + attr_vOffset.y,
					 0.0, 1.0);

	vPos = attr_mtxLocalToWorld * vPos;
	vPos = u_mtxWorldToCamera * vPos;
	gl_Position = u_mtxCameraToClip * vPos;
}
)src";
//-------------------------------------------------------------------------------------------------------------------------------------------------
const char * const szHYQUADBATCH_FRAGMENTSHADER = R"src(
#version 140
//#extension GL_ARB_explicit_attrib_location : enable

uniform sampler2D	u_Tex;

smooth in vec2		interp_vUV;
smooth in vec4		interp_vColor;

out vec4			out_vColor;

void main()
{
	// Blend interp_vColor with whatever texel I get from interp_vUV
	vec4 texelClr = texture(u_Tex, interp_vUV);

	out_vColor = interp_vColor * texelClr;

	// Discard fully transparent pixels so any potential stencil test isn't affected
	if(out_vColor.a == 0.0)
		discard;
}
)src";

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PRIMITIVE
const char * const szHYPRIMATIVE_VERTEXSHADER = R"src(
#version 140
//#extension GL_ARB_explicit_attrib_location : enable

uniform mat4					u_mtxTransform;
uniform mat4					u_mtxWorldToCamera;
uniform mat4					u_mtxCameraToClip;
uniform vec4					u_vColor;

//layout(location = 0) in vec2	attr_vPosition;

attribute vec2					attr_vPosition;

void main()
{
	vec4 vTemp = u_mtxTransform * vec4(attr_vPosition, 0, 1);
	vTemp = u_mtxWorldToCamera * vTemp;
	gl_Position = u_mtxCameraToClip * vTemp;
}
)src";
//-------------------------------------------------------------------------------------------------------------------------------------------------
const char * const szHYPRIMATIVE_FRAGMENTSHADER = R"src(
#version 140
//#extension GL_ARB_explicit_attrib_location : enable

uniform vec4	u_vColor;
out vec4		out_vColor;

void main()
{
	out_vColor = u_vColor;
}
)src";

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CIRCLE
const char *const szHYCIRCLE_VERTEXSHADER = R"src(
#version 140
//#extension GL_ARB_explicit_attrib_location : enable

uniform mat4					u_mtxTransform;
uniform mat4					u_mtxWorldToCamera;
uniform mat4					u_mtxCameraToClip;
uniform vec4					u_vColor;

//layout(location = 0) in vec2	attr_vPosition;

attribute vec2					attr_vPosition;

smooth out vec2					interp_vUV;


void main()
{
	vec4 vTemp = u_mtxTransform * vec4(attr_vPosition, 0, 1);
	vTemp = u_mtxWorldToCamera * vTemp;
	gl_Position = u_mtxCameraToClip * vTemp;
}
)src";
//-------------------------------------------------------------------------------------------------------------------------------------------------
const char *const szHYCIRCLE_FRAGMENTSHADER = R"src(
#version 140
//#extension GL_ARB_explicit_attrib_location : enable

uniform vec4	u_vColor;
out vec4		out_vColor;

void main()
{
	out_vColor = u_vColor;

	// Normalized pixel coordinates (from 0 to 1)
	vec2 uv = fragCoord/iResolution.xy * 2.0 - 1.0;
	float fAspect = iResolution.x / iResolution.y;
	// uv.x *= fAspect;

	float fDist = 1.0 - length(uv);

	if(fDist > 0.0)
		fDist = 1.0;
	else
		discard

	out_vColor.rgb = vec3(fDist);
}
)src";

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LINES2D
const char * const szHYLINES2D_VERTEXSHADER = R"src(
#version 140
//#extension GL_ARB_explicit_attrib_location : enable

uniform float					u_fHalfWidth;
uniform float					u_fFeatherAmt;
uniform vec4					u_vColor;
uniform mat4					u_mtxTransform;
uniform mat4					u_mtxWorldToCamera;
uniform mat4					u_mtxCameraToClip;

//layout(location = 0) in vec2	attr_vPosition;
//layout(location = 1) in vec2	attr_vNormal;

attribute vec2	attr_vPosition;
attribute vec2	attr_vNormal;

out vec2						interp_vNormal;

//////////////////////////////////////////////////////////////////////////
void main()
{
	interp_vNormal = attr_vNormal;

	vec4 vPos = u_mtxTransform * vec4(attr_vPosition, 0, 1);
	vPos = u_mtxWorldToCamera * vPos;
	gl_Position = u_mtxCameraToClip * (vPos + vec4(attr_vNormal * (u_fHalfWidth + (u_fFeatherAmt * 0.5f)), 0, 0));
}
)src";
//-------------------------------------------------------------------------------------------------------------------------------------------------
const char * const szHYLINES2D_FRAGMENTSHADER = R"src(
#version 140
//#extension GL_ARB_explicit_attrib_location : enable

uniform float		u_fHalfWidth;
uniform float		u_fFeatherAmt;
uniform vec4		u_vColor;

in vec2				interp_vNormal;
out vec4			out_vColor;

//////////////////////////////////////////////////////////////////////////
void main()
{
	out_vColor = u_vColor;
	out_vColor.w = smoothstep(u_fHalfWidth, u_fHalfWidth - u_fFeatherAmt, length(interp_vNormal) * u_fHalfWidth);
}
)src";

// Scratch/Sample functions
//////////////////////////////////////////////////////////////////////////
//vec4 when_greaterThan(vec4 x, vec4 y) // Can also overload to take vec2, vec3, and float
//{
//	return max(sign(x - y), 0.0);
//}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GLTF
const char * const szHYGLTF_VERTEXSHADER = R"src(
#version 140
//#extension GL_ARB_explicit_attrib_location : enable

uniform mat4					u_mtxTransform;
uniform mat4					u_mtxWorldToCamera;
uniform mat4					u_mtxCameraToClip;

//layout(location = 0) in vec3	attr_vPosition;
//layout(location = 1) in vec3	attr_vNormal;
//layout(location = 2) in vec2	attr_vUVcoord0;

attribute vec3	attr_vPosition;
attribute vec3	attr_vNormal;
attribute vec2	attr_vUVcoord0;

smooth out vec2					interp_vUV;

//////////////////////////////////////////////////////////////////////////
void main()
{
	vec4 vTemp = u_mtxTransform * vec4(attr_vPosition, 0, 1);
	vTemp = u_mtxWorldToCamera * vTemp;
	gl_Position = u_mtxCameraToClip * vTemp;

	interp_vUV = attr_vUVcoord0;
}
)src";
//-------------------------------------------------------------------------------------------------------------------------------------------------
const char * const szHYGLTF_FRAGMENTSHADER = R"src(
#version 140
//#extension GL_ARB_explicit_attrib_location : enable

uniform sampler2D	u_Tex;

smooth in vec2		interp_vUV;
smooth in vec4		interp_vColor;

out vec4			out_vColor;

void main()
{
	// Blend interp_vColor with whatever texel I get from interp_vUV
	vec4 texelClr = texture(u_Tex, interp_vUV);

	out_vColor = interp_vColor * texelClr;

	// Discard fully transparent pixels so any potential stencil test isn't affected
	if(out_vColor.a == 0.0)
		discard;
}
)src";

#endif /* HyOpenGLShaderSrc_h__ */
