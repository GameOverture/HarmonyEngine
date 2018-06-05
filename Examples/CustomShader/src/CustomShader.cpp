#include "CustomShader.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const char * const szCHECKERGRID_VERTEXSHADER = R"src(
#version 430

uniform mat4					u_mtxTransform;
uniform mat4					u_mtxWorldToCamera;
uniform mat4					u_mtxCameraToClip;

layout(location = 0) in vec2    attr_vPosition;
layout(location = 1) in vec2    attr_vUVcoord;

smooth out vec2                 interp_vUV;

void main()
{
    interp_vUV.x = attr_vUVcoord.x;
    interp_vUV.y = attr_vUVcoord.y;

    vec4 vTemp = u_mtxTransform * vec4(attr_vPosition, 0, 1);
    vTemp = u_mtxWorldToCamera * vTemp;
    gl_Position = u_mtxCameraToClip * vTemp;
}
)src";
//-------------------------------------------------------------------------------------------------------------------------------------------------
const char *const szCHECKERGRID_FRAGMENTSHADER = R"src(
#version 400

uniform float                   u_fGridSize;
uniform vec2                    u_vDimensions;
uniform vec4                    u_vGridColor1;
uniform vec4                    u_vGridColor2;

smooth in vec2                  interp_vUV;
out vec4                        out_vColor;

void main()
{
    vec2 vScreenCoords = (interp_vUV * u_vDimensions) / u_fGridSize;
    out_vColor = mix(u_vGridColor1, u_vGridColor2, step((float(int(floor(vScreenCoords.x) + floor(vScreenCoords.y)) & 1)), 0.9));
}
)src";
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PRIMITIVE
const char * const szTEST_VERTEXSHADER = R"src(
#version 430

uniform mat4					u_mtxTransform;
uniform mat4					u_mtxWorldToCamera;
uniform mat4					u_mtxCameraToClip;
uniform vec4					u_vColor;

layout(location = 0) in vec2	attr_vPosition;
layout(location = 1) in vec2    attr_vUVcoord;

smooth out vec2					interp_vUV;

void main()
{
	interp_vUV.x = attr_vUVcoord.x;
	interp_vUV.y = attr_vUVcoord.y;

	vec4 vTemp = u_mtxTransform * vec4(attr_vPosition, 0, 1);
	vTemp = u_mtxWorldToCamera * vTemp;
	gl_Position = u_mtxCameraToClip * vTemp;
}
)src";
//-------------------------------------------------------------------------------------------------------------------------------------------------
const char * const szTEST_FRAGMENTSHADER = R"src(
#version 430

uniform vec4	u_vColor;

smooth in vec2	interp_vUV;
out vec4		out_vColor;

void main()
{
	vec2 vTmp = interp_vUV;
	vTmp.x += 1;
	vTmp.y = 0.12;

	out_vColor = vec4(vTmp.x, vTmp.y, u_vColor.x, u_vColor.y);
	//out_vColor = u_vColor;
}
)src";

CustomShader::CustomShader(HarmonyInit &initStruct) :	IHyApplication(initStruct),
														m_CheckerGrid(400.0f, 400.0f, 25.0f, nullptr),
														m_pCheckerGridShader(HY_NEW HyShader(HYSHADERPROG_Primitive))
{
}

CustomShader::~CustomShader()
{
}

/*virtual*/ bool CustomShader::Initialize()
{
	m_pCamera = Window().CreateCamera2d();

	m_pCheckerGridShader->SetSourceCode(szCHECKERGRID_VERTEXSHADER, HYSHADER_Vertex);
	m_pCheckerGridShader->AddVertexAttribute("attr_vPosition", HYSHADERVAR_vec2);
	m_pCheckerGridShader->AddVertexAttribute("attr_vUVcoord", HYSHADERVAR_vec2);
	m_pCheckerGridShader->SetSourceCode(szCHECKERGRID_FRAGMENTSHADER, HYSHADER_Fragment);
	m_pCheckerGridShader->Finalize();

	m_CheckerGrid.SetShader(m_pCheckerGridShader);
	m_CheckerGrid.Load();

	return true;
}

/*virtual*/ bool CustomShader::Update()
{
	return true;
}

/*virtual*/ void CustomShader::Shutdown()
{
}
