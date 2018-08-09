/**************************************************************************
 *	ProjectDraw.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "ProjectDraw.h"
#include "GlobalUndoCmds.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CheckerGrid::CheckerGrid(float fWidth, float fHeight, float fGridSize, HyEntity2d *pParent) :   HyPrimitive2d(pParent),
																								m_vDIMENSIONS(fWidth, fHeight),
																								m_fGridSize(fGridSize)
{
	GetShape().SetAsBox(m_vDIMENSIONS.x * 0.5f, m_vDIMENSIONS.y * 0.5f);
}

CheckerGrid::~CheckerGrid()
{
}

/*virtual*/ void CheckerGrid::OnUpdateUniforms() /*override*/
{
	glm::mat4 mtx = HyPrimitive2d::GetWorldTransform();

	m_ShaderUniforms.Set("u_mtxTransform", mtx);
	m_ShaderUniforms.Set("u_fGridSize", m_fGridSize);
	m_ShaderUniforms.Set("u_vDimensions", m_vDIMENSIONS);
	m_ShaderUniforms.Set("u_vGridColor1", glm::vec4(106.0f / 255.0f, 105.0f / 255.0f, 113.0f / 255.0f, 1.0f));
	m_ShaderUniforms.Set("u_vGridColor2", glm::vec4(93.0f / 255.0f, 93.0f / 255.0f, 97.0f / 255.0f, 1.0f));
}

/*virtual*/ void CheckerGrid::OnWriteVertexData(HyVertexBuffer &vertexBufferRef) /*override*/
{
	HyAssert(GetNumVerts() == 6, "CheckerGrid::OnWriteDrawBufferData is trying to draw a primitive that's not a quad");

	for(int i = 0; i < 6; ++i)
	{
		vertexBufferRef.AppendData2d(&m_pVertBuffer[i], sizeof(glm::vec2));
		//*reinterpret_cast<glm::vec2 *>(pRefDataWritePos) = m_pVertBuffer[i];
		//pRefDataWritePos += sizeof(glm::vec2);

		glm::vec2 vUV;
		switch(i)
		{
		case 0:
		case 3:
			vUV.x = 0.0f;
			vUV.y = 1.0f;
			break;

		case 1:
			vUV.x = 1.0f;
			vUV.y = 1.0f;
			break;

		case 2:
		case 4:
			vUV.x = 1.0f;
			vUV.y = 0.0f;
			break;

		case 5:
			vUV.x = 0.0f;
			vUV.y = 0.0f;
			break;
		}

		vertexBufferRef.AppendData2d(&vUV, sizeof(glm::vec2));
		//*reinterpret_cast<glm::vec2 *>(pRefDataWritePos) = vUV;
		//pRefDataWritePos += sizeof(glm::vec2);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ProjectDraw::ProjectDraw(IHyApplication &hyApp) :   IDraw(nullptr, hyApp),
													m_CheckerGrid(20000.0f, 20000.0f, 25.0f, this)
{
	m_pCheckerGridShader = HY_NEW HyShader(HYSHADERPROG_Primitive);
	m_pCheckerGridShader->SetSourceCode(szCHECKERGRID_VERTEXSHADER, HYSHADER_Vertex);
	m_pCheckerGridShader->AddVertexAttribute("attr_vPosition", HyShaderVariable::vec2);
	m_pCheckerGridShader->AddVertexAttribute("attr_vUVcoord", HyShaderVariable::vec2);
	m_pCheckerGridShader->SetSourceCode(szCHECKERGRID_FRAGMENTSHADER, HYSHADER_Fragment);
	m_pCheckerGridShader->Finalize();

	m_CheckerGrid.SetShader(m_pCheckerGridShader);
	m_CheckerGrid.SetDisplayOrder(-1000);
}

/*virtual*/ ProjectDraw::~ProjectDraw()
{
}

/*virtual*/ void ProjectDraw::OnShow(IHyApplication &hyApp) /*override*/
{
}

/*virtual*/ void ProjectDraw::OnHide(IHyApplication &hyApp) /*override*/
{
}

/*virtual*/ void ProjectDraw::OnResizeRenderer() /*override*/
{
}
