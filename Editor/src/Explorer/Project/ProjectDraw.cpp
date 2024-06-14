/**************************************************************************
 *	ProjectDraw.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
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

//-------------------------------------------------------------------------------------------------------------------------------------------------
const char *const szOVERGRID_FRAGMENTSHADER = R"src(
#version 400

uniform float                   u_fGridSize;
uniform vec2                    u_vDimensions;
uniform vec4                    u_vGridColor;

smooth in vec2                  interp_vUV;
out vec4                        out_vColor;

void main()
{
	vec2 vScreenCoords = (interp_vUV * u_vDimensions);

	int iWidth = int(vScreenCoords.x);
	int iHeight = int(vScreenCoords.y);
	int iGridSize = int(u_fGridSize);

	if(iWidth % iGridSize == 0 || iHeight % iGridSize == 0)
		out_vColor = u_vGridColor;
	else
		out_vColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);
}
)src";
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CheckerGrid::CheckerGrid(float fWidth, float fHeight, float fGridSize) :
	m_vDIMENSIONS(fWidth, fHeight),
	m_fGridSize(fGridSize)
{
	SetAsBox(m_vDIMENSIONS.x, m_vDIMENSIONS.y);
	pos.Set(m_vDIMENSIONS.x * -0.5f, m_vDIMENSIONS.y * -0.5f);
}

/*virtual*/ CheckerGrid::~CheckerGrid()
{
}

/*virtual*/ void CheckerGrid::OnUpdateUniforms(float fExtrapolatePercent) /*override*/
{
	glm::mat4 mtx = HyPrimitive2d::GetSceneTransform(fExtrapolatePercent);

	m_ShaderUniforms.Set("u_mtxTransform", mtx);
	m_ShaderUniforms.Set("u_fGridSize", m_fGridSize);
	m_ShaderUniforms.Set("u_vDimensions", m_vDIMENSIONS);
	m_ShaderUniforms.Set("u_vGridColor1", glm::vec4(106.0f / 255.0f, 105.0f / 255.0f, 113.0f / 255.0f, 1.0f));
	m_ShaderUniforms.Set("u_vGridColor2", glm::vec4(93.0f / 255.0f, 93.0f / 255.0f, 97.0f / 255.0f, 1.0f));
}

/*virtual*/ bool CheckerGrid::WriteVertexData(uint32 uiNumInstances, HyVertexBuffer &vertexBufferRef, float fExtrapolatePercent) /*override*/
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

	return true;
}


OverGrid::OverGrid(float fWidth, float fHeight, float fGridSize) :
	CheckerGrid(fWidth, fHeight, fGridSize)
{
}

/*virtual*/ OverGrid::~OverGrid()
{
}

/*virtual*/ void OverGrid::OnUpdateUniforms(float fExtrapolatePercent) /*override*/
{
	glm::mat4 mtx = HyPrimitive2d::GetSceneTransform(fExtrapolatePercent);

	m_ShaderUniforms.Set("u_mtxTransform", mtx);
	m_ShaderUniforms.Set("u_fGridSize", m_fGridSize);
	m_ShaderUniforms.Set("u_vDimensions", m_vDIMENSIONS);
	m_ShaderUniforms.Set("u_vGridColor", glm::vec4(0.0f, 0.0f, 0.0f, 0.25f));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const float fDIMENSION_SIZE = 20000.0f;

ProjectDraw::ProjectDraw() :
	IDraw(nullptr, FileDataPair()),
	m_CheckerGrid(fDIMENSION_SIZE, fDIMENSION_SIZE, DEFAULT_GRID_SIZE),
	m_OverGrid(fDIMENSION_SIZE, fDIMENSION_SIZE, DEFAULT_GRID_SIZE)
{
	ChildAppend(m_CheckerGrid);

	m_pCheckerGridShader = HY_NEW HyShader(HYSHADERPROG_Primitive);
	m_pCheckerGridShader->SetSourceCode(szCHECKERGRID_VERTEXSHADER, HYSHADER_Vertex);
	m_pCheckerGridShader->AddVertexAttribute("attr_vPosition", HyShaderVariable::vec2);
	m_pCheckerGridShader->AddVertexAttribute("attr_vUVcoord", HyShaderVariable::vec2);
	m_pCheckerGridShader->SetSourceCode(szCHECKERGRID_FRAGMENTSHADER, HYSHADER_Fragment);
	m_pCheckerGridShader->Finalize();

	m_CheckerGrid.SetShader(m_pCheckerGridShader);
	m_CheckerGrid.SetDisplayOrder(-1000);

	//std::vector<glm::vec2> lineList(2, glm::vec2());
	//lineList[0].x = -fDIMENSION_SIZE * 0.5f;
	//lineList[0].y = 0.0f;
	//lineList[1].x = fDIMENSION_SIZE * 0.5f;
	//lineList[1].y = 0.0f;

	glm::vec2 vWindowSize = HyEngine::Window().GetWindowSize();

	m_OriginHorzBg.SetLineThickness(3.0f);
	m_OriginHorzBg.SetTint(HyColor::Black);
	m_OriginHorzBg.SetVisible(false);
	m_OriginHorzBg.SetAsLineSegment(glm::vec2(0.0f, 0.0f), glm::vec2(vWindowSize.x, 0.0f));
	m_OriginHorzBg.UseWindowCoordinates();
	m_OriginHorz.SetLineThickness(1.0f);
	m_OriginHorz.SetTint(HyColor::White);
	m_OriginHorz.SetVisible(false);
	m_OriginHorz.SetAsLineSegment(glm::vec2(0.0f, 0.0f), glm::vec2(vWindowSize.x, 0.0f));
	m_OriginHorz.UseWindowCoordinates();

	//lineList[0].x = 0.0f;
	//lineList[0].y = -fDIMENSION_SIZE * 0.5f;
	//lineList[1].x = 0.0f;
	//lineList[1].y = fDIMENSION_SIZE * 0.5f;
	m_OriginVertBg.SetLineThickness(3.0f);
	m_OriginVertBg.SetTint(HyColor::Black);
	m_OriginVertBg.SetVisible(false);
	m_OriginVertBg.SetAsLineSegment(glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, vWindowSize.y));
	m_OriginVertBg.UseWindowCoordinates();
	m_OriginVert.SetLineThickness(1.0f);
	m_OriginVert.SetTint(HyColor::White);
	m_OriginVert.SetVisible(false);
	m_OriginVert.SetAsLineSegment(glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, vWindowSize.y));
	m_OriginVert.UseWindowCoordinates();

	ChildAppend(m_OriginHorzBg);
	ChildAppend(m_OriginVertBg);
	ChildAppend(m_OriginHorz);
	ChildAppend(m_OriginVert);

	ChildAppend(m_OverGrid);

	m_pOverGridShader = HY_NEW HyShader(HYSHADERPROG_Primitive);
	m_pOverGridShader->SetSourceCode(szCHECKERGRID_VERTEXSHADER, HYSHADER_Vertex);
	m_pOverGridShader->AddVertexAttribute("attr_vPosition", HyShaderVariable::vec2);
	m_pOverGridShader->AddVertexAttribute("attr_vUVcoord", HyShaderVariable::vec2);
	m_pOverGridShader->SetSourceCode(szOVERGRID_FRAGMENTSHADER, HYSHADER_Fragment);
	m_pOverGridShader->Finalize();

	m_OverGrid.SetShader(m_pOverGridShader);
	m_OverGrid.SetDisplayOrder(99999);
}

/*virtual*/ ProjectDraw::~ProjectDraw()
{
}

/*virtual*/ void ProjectDraw::OnUpdate() /*override*/
{
	IDraw::OnUpdate();

	// TODO: Do this in OnCameraUpdated() instead - Requires OnCameraUpdated() to be called for ProjectDraw (only other items' IDraw::OnCameraUpdated are called)
	glm::vec2 ptOriginPos;
	m_pCamera->ProjectToCamera(glm::vec2(0.0f, 0.0f), ptOriginPos);

	m_OriginHorzBg.pos.Set(0.0f, ptOriginPos.y);
	m_OriginVertBg.pos.Set(ptOriginPos.x, 0.0f);
	m_OriginHorz.pos.Set(0.0f, ptOriginPos.y);
	m_OriginVert.pos.Set(ptOriginPos.x, 0.0f);
}

void ProjectDraw::EnableGridBackground(bool bEnable)
{
	m_CheckerGrid.SetVisible(bEnable);
}

void ProjectDraw::EnableGridOrigin(bool bEnable)
{
	m_OriginHorzBg.SetVisible(bEnable);
	m_OriginVertBg.SetVisible(bEnable);
	m_OriginHorz.SetVisible(bEnable);
	m_OriginVert.SetVisible(bEnable);
}

void ProjectDraw::EnableGridOverlay(bool bEnable)
{
	m_OverGrid.SetVisible(bEnable);
}

/*virtual*/ void ProjectDraw::OnShow() /*override*/
{
}

/*virtual*/ void ProjectDraw::OnHide() /*override*/
{
}

/*virtual*/ void ProjectDraw::OnResizeRenderer() /*override*/
{
	glm::vec2 vWindowSize = HyEngine::Window().GetWindowSize();

	m_OriginHorzBg.SetAsLineSegment(glm::vec2(0.0f, 0.0f), glm::vec2(vWindowSize.x, 0.0f));
	m_OriginHorz.SetAsLineSegment(glm::vec2(0.0f, 0.0f), glm::vec2(vWindowSize.x, 0.0f));
	m_OriginVertBg.SetAsLineSegment(glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, vWindowSize.y));
	m_OriginVert.SetAsLineSegment(glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, vWindowSize.y));
}

///*virtual*/ void ProjectDraw::OnCameraUpdated() /*override*/
//{
	//glm::vec2 ptOriginPos;
	//m_pCamera->ProjectToCamera(glm::vec2(0.0f, 0.0f), ptOriginPos);

	//m_OriginHorzBg.pos.Set(0.0f, ptOriginPos.y);
	//m_OriginVertBg.pos.Set(ptOriginPos.x, 0.0f);
	//m_OriginHorz.pos.Set(0.0f, ptOriginPos.y);
	//m_OriginVert.pos.Set(ptOriginPos.x, 0.0f);
//}
