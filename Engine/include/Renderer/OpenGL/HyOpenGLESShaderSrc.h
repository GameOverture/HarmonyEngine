/**************************************************************************
 *	HyOpenGLESShaderSrc.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2020 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyOpenGLESShaderSrc_h__
#define HyOpenGLESShaderSrc_h__

#include "Afx/HyStdAfx.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// QUADBATCH
const char * const szHYQUADBATCH_VERTEXSHADER = R"src(#version 300 es

precision mediump float;

uniform mat4					u_view_mtx;
uniform mat4					u_projection_mtx;

layout(location = 0) in vec2	attr_size;
layout(location = 1) in vec2	attr_offset;
layout(location = 2) in vec4	attr_top_tint;
layout(location = 3) in vec4	attr_bot_tint;
layout(location = 4) in vec2	attr_uv0;
layout(location = 5) in vec2	attr_uv1;
layout(location = 6) in vec2	attr_uv2;
layout(location = 7) in vec2	attr_uv3;
layout(location = 8) in mat4	attr_transform_mtx;

smooth out vec2					interp_uv;
smooth out vec4					interp_color;

const vec2 g_vPOSITION[] = vec2[4](vec2(1.0f, 1.0f),
								   vec2(0.0f, 1.0f),
								   vec2(1.0f, 0.0f),
								   vec2(0.0f, 0.0f));

void main()
{
	vec2 g_vUVCOORDS[] = vec2[4](attr_uv0,
								 attr_uv1,
								 attr_uv2,
								 attr_uv3);

	vec4 g_vCOLORS[] = vec4[4](attr_top_tint,
							   attr_top_tint,
							   attr_bot_tint,
							   attr_bot_tint);

	interp_uv = g_vUVCOORDS[gl_VertexID];
	interp_color = g_vCOLORS[gl_VertexID];

	vec4 pos = vec4((g_vPOSITION[gl_VertexID].x * attr_size.x) + attr_offset.x,
					 (g_vPOSITION[gl_VertexID].y * attr_size.y) + attr_offset.y,
					 0.0, 1.0);

	pos = attr_transform_mtx * pos;
	pos = u_view_mtx * pos;
	gl_Position = u_projection_mtx * pos;
}
)src";
//-------------------------------------------------------------------------------------------------------------------------------------------------
const char * const szHYQUADBATCH_FRAGMENTSHADER = R"src(#version 300 es

precision mediump float;

uniform sampler2D				u_diffuse;

smooth in vec2					interp_uv;
smooth in vec4					interp_color;

out vec4						out_color;

void main()
{
	// Blend interp_color with whatever texel I get from interp_uv
	vec4 texel_color = texture(u_diffuse, interp_uv);

	out_color = interp_color * texel_color;

	// Discard fully transparent pixels so any potential stencil test isn't affected
	if(out_color.a == 0.0)
		discard;
}
)src";

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PRIMITIVE
const char * const szHYPRIMATIVE_VERTEXSHADER = R"src(#version 300 es

precision mediump float;

uniform mat4					u_transform_mtx;
uniform mat4					u_view_mtx;
uniform mat4					u_projection_mtx;

layout(location = 0) in vec2	attr_pos;
layout(location = 1) in vec4	attr_color;

smooth out vec4					interp_color;

void main()
{
	interp_color = attr_color;

	vec4 vTemp = u_transform_mtx * vec4(attr_pos, 0, 1);
	vTemp = u_view_mtx * vTemp;
	gl_Position = u_projection_mtx * vTemp;
}
)src";
//-------------------------------------------------------------------------------------------------------------------------------------------------
const char * const szHYPRIMATIVE_FRAGMENTSHADER = R"src(#version 300 es

precision mediump float;

smooth in vec4					interp_color;

out vec4						out_color;

void main()
{
	out_color = interp_color;
}
)src";

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LINES2D
const char * const szHYLINES2D_VERTEXSHADER = R"src(#version 300 es

precision mediump float;

uniform float					u_half_width;
uniform float					u_feather_amt;
uniform vec4					u_color;
uniform mat4					u_transform_mtx;
uniform mat4					u_view_mtx;
uniform mat4					u_projection_mtx;

layout(location = 0) in vec2	attr_pos;
layout(location = 1) in vec2	attr_normal;

out vec2						interp_normal;

void main()
{
	interp_normal = attr_normal;

	vec4 pos = u_transform_mtx * vec4(attr_pos, 0, 1);
	pos = u_view_mtx * pos;
	gl_Position = u_projection_mtx * (pos + vec4(attr_normal * (u_half_width + (u_feather_amt * 0.5f)), 0, 0));
}
)src";
//-------------------------------------------------------------------------------------------------------------------------------------------------
const char * const szHYLINES2D_FRAGMENTSHADER = R"src(#version 300 es

precision mediump float;

uniform float					u_half_width;
uniform float					u_feather_amt;
uniform vec4					u_color;

in vec2							interp_normal;
out vec4						out_color;

void main()
{
	out_color = u_color;
	out_color.w = smoothstep(u_half_width, u_half_width - u_feather_amt, length(interp_normal) * u_half_width);
}
)src";

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GLTF
const char * const szHYGLTF_VERTEXSHADER = R"src(#version 300 es

precision mediump float;

uniform mat4					u_transform_mtx;
uniform mat4					u_view_mtx;
uniform mat4					u_projection_mtx;

layout(location = 0) in vec3	attr_pos;
layout(location = 1) in vec3	attr_normal;
layout(location = 2) in vec2	attr_uv0;

smooth out vec2					interp_uv;

//////////////////////////////////////////////////////////////////////////
void main()
{
	vec4 vTemp = u_transform_mtx * vec4(attr_pos, 0, 1);
	vTemp = u_view_mtx * vTemp;
	gl_Position = u_projection_mtx * vTemp;

	interp_uv = attr_uv0;
}
)src";
//-------------------------------------------------------------------------------------------------------------------------------------------------
const char * const szHYGLTF_FRAGMENTSHADER = R"src(#version 300 es

precision mediump float;

uniform sampler2D				u_diffuse;

smooth in vec2					interp_uv;
smooth in vec4					interp_color;

out vec4						out_color;

void main()
{
	// Blend interp_color with whatever texel I get from interp_uv
	vec4 texel_color = texture(u_diffuse, interp_uv);

	out_color = interp_color * texel_color;

	// Discard fully transparent pixels so any potential stencil test isn't affected
	if(out_color.a == 0.0)
		discard;
}
)src";

#endif /* HyOpenGLESShaderSrc_h__ */
