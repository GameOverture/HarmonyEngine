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
#version 140

uniform mat4					u_transform_mtx;
uniform mat4					u_view_mtx;
uniform mat4					u_projection_mtx;

//layout(location = 0) in vec2	attr_pos;
//layout(location = 1) in vec2	attr_uv;

attribute vec2					attr_pos;
attribute vec2					attr_uv;

smooth out vec2					interp_uv;

void main()
{
	interp_uv.x = attr_uv.x;
	interp_uv.y = attr_uv.y;

	vec4 pos = u_transform_mtx * vec4(attr_pos, 0, 1);
	pos = u_view_mtx * pos;
	gl_Position = u_projection_mtx * pos;
}
)src";
//-------------------------------------------------------------------------------------------------------------------------------------------------
const char *const szCHECKERGRID_FRAGMENTSHADER = R"src(
#version 140

uniform float					u_grid_size;
uniform vec2					u_dimensions;
uniform vec4					u_grid_color1;
uniform vec4					u_grid_color2;

smooth in vec2					interp_uv;
out vec4						out_color;

void main()
{
	vec2 screen_coords = (interp_uv * u_dimensions) / u_grid_size;
	out_color = mix(u_grid_color1, u_grid_color2, step((float(int(floor(screen_coords.x) + floor(screen_coords.y)) & 1)), 0.9));
}
)src";

//-------------------------------------------------------------------------------------------------------------------------------------------------
const char *const szOVERGRID_FRAGMENTSHADER = R"src(
#version 140

uniform float					u_grid_size;
uniform vec2					u_dimensions;
uniform vec4					u_grid_color;

smooth in vec2					interp_uv;
out vec4						out_color;

void main()
{
	vec2 screen_coords = (interp_uv * u_dimensions);

	int width = int(screen_coords.x);
	int height = int(screen_coords.y);
	int grid_size = int(u_grid_size);

	if(width % grid_size == 0 || height % grid_size == 0)
		out_color = u_grid_color;
	else
		out_color = vec4(0.0f, 0.0f, 0.0f, 0.0f);
}
)src";
//-------------------------------------------------------------------------------------------------------------------------------------------------
const char *const szTILEMAPGRIDSQUARE_FRAGMENTSHADER = R"src(
#version 140

uniform vec2					u_dimensions;
uniform vec2					u_world_origin;	// World-space origin of the grid
uniform vec2					u_tile_size;	// Width and height

smooth in vec2					interp_uv;
out vec4						out_color;

void main()
{
	vec2 screen_coords = (interp_uv * u_dimensions);

	int width = int(screen_coords.x + u_world_origin.x);
	int height = int(screen_coords.y + u_world_origin.y);
	int tile_size_width = int(u_tile_size.x);
	int tile_size_height = int(u_tile_size.y);

	if(width % tile_size_width == 0 || height % tile_size_height == 0)
		out_color = vec4(0.9804, 0.3529, 0.0392, 1.0);
	else
		out_color = vec4(0.0f, 0.0f, 0.0f, 1.0f);
}
)src";
//-------------------------------------------------------------------------------------------------------------------------------------------------
const char *const szTILEMAPGRIDHEX_FRAGMENTSHADER = R"src(
#version 140

uniform vec2					u_dimensions;
uniform vec2					u_world_origin;	// World-space origin of the grid
//uniform vec2					u_tile_size;	// Width and height

smooth in vec2					interp_uv;
out vec4						out_color;

const float x_shift = sin(1.0471975511965);

float hex(vec2 p)
{
	p.x /= x_shift;
	p.y += floor(p.x) * 0.5;
	p = abs(fract(p) - 0.5);
	return smoothstep(0.001, 0.05, abs(1.0 - max(p.x * 1.5 + p.y, p.y * 2.0)));
}

void main()
{
	vec2 uv = interp_uv;
	uv += u_world_origin;
	uv -= 0.5;
	uv.x *= u_dimensions.x / u_dimensions.y;
	uv *= 5.0;
	vec3 color = vec3(0.0, 0.0, 0.0);
	color += vec3(hex(uv));
	out_color = vec4(color, 1.0);
}
)src";
//-------------------------------------------------------------------------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CheckerGrid::CheckerGrid(float fWidth, float fHeight, float fGridSize) :
	m_vDIMENSIONS(fWidth, fHeight),
	m_fGridSize(fGridSize)
{
	SetAsBox(0, m_vDIMENSIONS.x, m_vDIMENSIONS.y, 0.0f);
}

/*virtual*/ CheckerGrid::~CheckerGrid()
{
}

/*virtual*/ void CheckerGrid::OnUpdateUniforms(float fExtrapolatePercent) /*override*/
{
	glm::mat4 mtx = HyPrimitive2d::GetSceneTransform(fExtrapolatePercent);

	m_ShaderUniforms.Set("u_transform_mtx", mtx);
	m_ShaderUniforms.Set("u_grid_size", m_fGridSize);
	m_ShaderUniforms.Set("u_dimensions", m_vDIMENSIONS);
	m_ShaderUniforms.Set("u_grid_color1", glm::vec4(HyGlobal::GetEditorColor(EDITORCOLOR_GridColor1).GetAsVec4()));
	m_ShaderUniforms.Set("u_grid_color2", glm::vec4(HyGlobal::GetEditorColor(EDITORCOLOR_GridColor2).GetAsVec4()));
}

/*virtual*/ bool CheckerGrid::WriteVertexData(uint32 uiNumInstances, HyVertexBuffer &vertexBufferRef, float fExtrapolatePercent) /*override*/
{
	HyAssert(GetNumVerts(0) == 6, "CheckerGrid::OnWriteDrawBufferData is trying to draw a primitive that's not a quad");

	for(int i = 0; i < 6; ++i)
	{
		vertexBufferRef.AppendVertexData(&m_LayerList[0].m_pVertBuffer[i], sizeof(glm::vec2));

		glm::vec2 vUV;
		switch(i)
		{
		case 0:
		case 5:
			vUV.x = 0.0f;
			vUV.y = 0.0f;
			break;

		case 1:
			vUV.x = 0.0f;
			vUV.y = 1.0f;
			break;

		case 2:
		case 3:
			vUV.x = 1.0f;
			vUV.y = 1.0f;
			break;

		case 4:
			vUV.x = 1.0f;
			vUV.y = 0.0f;
			break;
		}

		vertexBufferRef.AppendVertexData(&vUV, sizeof(glm::vec2));
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

	m_ShaderUniforms.Set("u_transform_mtx", mtx);
	m_ShaderUniforms.Set("u_grid_size", m_fGridSize);
	m_ShaderUniforms.Set("u_dimensions", m_vDIMENSIONS);
	m_ShaderUniforms.Set("u_grid_color", glm::vec4(0.0f, 0.0f, 0.0f, 0.25f));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const float fDIMENSION_SIZE = 20000.0f;

ProjectDraw::ProjectDraw() :
	HyEntity2d(),
	m_CheckerGrid(fDIMENSION_SIZE, fDIMENSION_SIZE, DEFAULT_GRID_SIZE),
	m_Origin(this),
	m_OverGrid(fDIMENSION_SIZE, fDIMENSION_SIZE, DEFAULT_GRID_SIZE),
	m_pTileMapSquareShader(nullptr),
	m_pTileMapHexShader(nullptr)
{
	ChildAppend(m_CheckerGrid);

	m_pCheckerGridShader = HY_NEW HyShader(HYSHADERPROG_Primitive);
	m_pCheckerGridShader->SetSourceCode(szCHECKERGRID_VERTEXSHADER, HYSHADER_Vertex);
	m_pCheckerGridShader->AddVertexAttribute("attr_pos", HyShaderVariable::vec2);
	m_pCheckerGridShader->AddVertexAttribute("attr_uv", HyShaderVariable::vec2);
	m_pCheckerGridShader->SetSourceCode(szCHECKERGRID_FRAGMENTSHADER, HYSHADER_Fragment);
	m_pCheckerGridShader->Finalize();

	m_CheckerGrid.SetShader(m_pCheckerGridShader);
	m_CheckerGrid.SetDisplayOrder(-1000);

	m_Origin.SetLayerColor(0, HyColor::Black);
	m_Origin.SetLayerColor(1, HyColor::Black);
	m_Origin.SetLayerColor(2, HyColor::White);
	m_Origin.SetLayerColor(3, HyColor::White);

	OnResizeRenderer();

	m_Origin.SetVisible(false);
	m_Origin.UseWindowCoordinates();

	ChildAppend(m_OverGrid);

	m_pOverGridShader = HY_NEW HyShader(HYSHADERPROG_Primitive);
	m_pOverGridShader->SetSourceCode(szCHECKERGRID_VERTEXSHADER, HYSHADER_Vertex);
	m_pOverGridShader->AddVertexAttribute("attr_pos", HyShaderVariable::vec2);
	m_pOverGridShader->AddVertexAttribute("attr_uv", HyShaderVariable::vec2);
	m_pOverGridShader->SetSourceCode(szOVERGRID_FRAGMENTSHADER, HYSHADER_Fragment);
	m_pOverGridShader->Finalize();

	m_OverGrid.SetShader(m_pOverGridShader);
	m_OverGrid.SetDisplayOrder(99999);
	
	// Misc shaders
	m_pTileMapSquareShader = HY_NEW HyShader(HYSHADERPROG_Primitive);
	m_pTileMapSquareShader->SetSourceCode(szCHECKERGRID_VERTEXSHADER, HYSHADER_Vertex);
	m_pTileMapSquareShader->AddVertexAttribute("attr_pos", HyShaderVariable::vec2);
	m_pTileMapSquareShader->AddVertexAttribute("attr_uv", HyShaderVariable::vec2);
	m_pTileMapSquareShader->SetSourceCode(szTILEMAPGRIDSQUARE_FRAGMENTSHADER, HYSHADER_Fragment);
	m_pTileMapSquareShader->Finalize();

	m_pTileMapHexShader = HY_NEW HyShader(HYSHADERPROG_Primitive);
	m_pTileMapHexShader->SetSourceCode(szCHECKERGRID_VERTEXSHADER, HYSHADER_Vertex);
	m_pTileMapHexShader->AddVertexAttribute("attr_pos", HyShaderVariable::vec2);
	m_pTileMapHexShader->AddVertexAttribute("attr_uv", HyShaderVariable::vec2);
	m_pTileMapHexShader->SetSourceCode(szTILEMAPGRIDHEX_FRAGMENTSHADER, HYSHADER_Fragment);
	m_pTileMapHexShader->Finalize();
}

/*virtual*/ ProjectDraw::~ProjectDraw()
{
}

void ProjectDraw::EnableGridBackground(bool bEnable)
{
	m_CheckerGrid.SetVisible(bEnable);
}

void ProjectDraw::EnableGridOrigin(bool bEnable)
{
	m_Origin.SetVisible(bEnable);
}

void ProjectDraw::EnableGridOverlay(bool bEnable)
{
	m_OverGrid.SetVisible(bEnable);
}

void ProjectDraw::OnResizeRenderer()
{
	glm::vec2 vWindowSize = HyEngine::Window().GetWindowSize();

	m_Origin.SetAsLineSegment(0, glm::vec2(0.0f, 0.0f), glm::vec2(vWindowSize.x, 0.0f), 3.0f);
	m_Origin.SetAsLineSegment(1, glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, vWindowSize.y), 3.0f);
	m_Origin.SetAsLineSegment(2, glm::vec2(0.0f, 0.0f), glm::vec2(vWindowSize.x, 0.0f), 1.0f);
	m_Origin.SetAsLineSegment(3, glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, vWindowSize.y), 1.0f);
}

void ProjectDraw::OnCameraUpdated()
{
	glm::vec2 ptOriginPos;
	HyEngine::Window(0).GetCamera2d(0)->ProjectToCamera(glm::vec2(0.0f, 0.0f), ptOriginPos);

	m_Origin.SetLayerOffset(0, glm::vec2(0.0f, ptOriginPos.y));
	m_Origin.SetLayerOffset(1, glm::vec2(ptOriginPos.x, 0.0f));
	m_Origin.SetLayerOffset(2, glm::vec2(0.0f, ptOriginPos.y));
	m_Origin.SetLayerOffset(3, glm::vec2(ptOriginPos.x, 0.0f));
}

HyShader *ProjectDraw::GetTileMapSquareShader()
{
	return m_pTileMapSquareShader;
}

HyShader *ProjectDraw::GetTileMapHexShader()
{
	return m_pTileMapHexShader;
}
