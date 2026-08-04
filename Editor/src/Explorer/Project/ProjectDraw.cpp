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
const char *const szTILEMAPGRID_FRAGMENTSHADER = R"src(
#version 140

uniform mat4					u_inv_tilemap;		// World to local space transform matrix of the TileMapLayer
uniform vec2					u_position;
uniform vec2					u_dimensions;		// Width and height of entire render quad
uniform vec2					u_grid_size;		// Width and height of the tiles in the grid
uniform bool					u_stagger_odd;		// Whether the grid is staggered on odd rows or even rows
uniform vec3					u_grid_color;
uniform float					u_line_width;		// Grid line width in world units

smooth in vec2					interp_uv;
out vec4						out_color;

float TileSDF(vec2 local_pos);

void main()
{
	// Compute this fragment's world-space position.
	vec2 quad_offset = (interp_uv - vec2(0.5)) * u_dimensions;
	quad_offset.y *= -1.0; // Harmony world up is positive Y, but UV's are the opposite
	vec2 world_pos = u_position + quad_offset;
	
	// Convert the fragment position into the TileMapLayer's local coordinate system.
	vec2 local_pos = (u_inv_tilemap * vec4(world_pos, 0.0, 1.0)).xy;

	// IF NO ANTI-ALIASING USE THIS
	//float dist = TileSDF(local_pos);
	//out_color = vec4(u_grid_color, step(dist, u_line_width));

	// IF ANTI-ALIASING USE THIS (also mixes in black outline)
	float gridAlpha = smoothstep(u_line_width, 0.0, TileSDF(local_pos));
	vec3 finalColor = mix(vec3(0.0), u_grid_color, gridAlpha);
	out_color = vec4(finalColor, gridAlpha);
}
)src";

const char *const szTILEMAPGRIDSQUARESDF_FRAGMENTSHADER = R"src(
float TileSDF(vec2 local_pos)
{
	vec2 cell_frac = fract(local_pos / u_grid_size);
	vec2 dist = min(cell_frac, 1.0 - cell_frac) * u_grid_size;

	return min(dist.x, dist.y);
}
)src";

const char *const szTILEMAPGRIDHALFSQUARESDF_FRAGMENTSHADER = R"src(
float TileSDF(vec2 local_pos)
{
	vec2 p = local_pos;

	float row = floor(p.y / u_grid_size.y);

	bool stagger = u_stagger_odd ?
		(mod(row, 2.0) == 1.0) :
		(mod(row, 2.0) == 0.0);

	if(stagger)
		p.x -= u_grid_size.x * 0.5;

	vec2 f = fract(p / u_grid_size);

	vec2 d = min(f, 1.0 - f) * u_grid_size;
	return min(d.x, d.y);
}
)src";

const char *const szTILEMAPGRIDSISOMETRIC_FRAGMENTSHADER = R"src(
#version 140

uniform mat4					u_inv_tilemap;		// World to local space transform matrix of the TileMapLayer
uniform vec2					u_position;
uniform vec2					u_dimensions;		// Width and height of entire render quad
uniform vec2					u_grid_size;		// Width and height of the tiles in the grid
uniform vec3					u_grid_color;
uniform float					u_line_width;		// Grid line width in world units

smooth in vec2					interp_uv;
out vec4						out_color;

// Helper to calculate distance to nearest grid line for antialiasing
float lineDistance(float coord, float size, float width)
{
	// Find position within the current tile [0, size)
	float pos = mod(coord, size);
	// Calculate distance to the nearest edge (0 or size)
	float dist = min(pos, size - pos);
	// Return smoothed alpha: 1.0 inside the line, 0.0 outside, smooth transition
	return smoothstep(width, 0.0, dist);
}

void main()
{
	// Compute this fragment's world-space position.
	vec2 quad_offset = (interp_uv - vec2(0.5)) * u_dimensions;
	quad_offset.y *= -1.0; // Harmony world up is positive Y, but UV's are the opposite
	vec2 world_pos = u_position + quad_offset;
	
	// Convert the fragment position into the TileMapLayer's local coordinate system.
	vec2 local_pos = (u_inv_tilemap * vec4(world_pos, 0.0, 1.0)).xy;


	float slope = u_grid_size.y / u_grid_size.x;

	float axisA = (slope * local_pos.x) + local_pos.y;
	float axisB = (-slope * local_pos.x) + local_pos.y;
	
	float spacing = slope * u_grid_size.x;

	float distA = lineDistance(axisA, spacing, u_line_width);
	float distB = lineDistance(axisB, spacing, u_line_width);

	// IF NO ANTI-ALIASING USE THIS
	//float dist = max(distA, distB);
	//out_color = vec4(u_grid_color, step(dist, u_line_width));

	// IF ANTI-ALIASING USE THIS (also mixes in black outline)
	float gridAlpha = max(distA, distB);
	vec3 finalColor = mix(vec3(0.0), u_grid_color, gridAlpha);
	out_color = vec4(finalColor, gridAlpha);
}
)src";

const char *const szTILEMAPGRIDHEXFLATTOP_FRAGMENTSHADER = R"src(
#version 140

uniform mat4					u_inv_tilemap;	// World to local space transform matrix of the infinite TileMapLayer grid
uniform vec2					u_position;		// Centered position of the quad viewport in world space
uniform vec2					u_dimensions;	// Width and height of entire render quad
uniform vec2					u_grid_size;	// Width and height of the tiles in the grid
uniform vec3					u_grid_color;	// Color of the grid lines
uniform float					u_line_width;	// Grid line width in world units

smooth in vec2					interp_uv;
out vec4						out_color;

const float x_shift = 0.8660254038; // AKA sin(3.1415926535 / 3.0)

void main()
{
	// Compute this fragment's world-space position.
	vec2 quad_offset = (interp_uv - vec2(0.5)) * u_dimensions;
	quad_offset.y *= -1.0; // Harmony world up is positive Y, but UV's are the opposite
	vec2 world_pos = u_position + quad_offset;

	// Convert the fragment position into the TileMapLayer's local coordinate system.
	vec2 local_pos = (u_inv_tilemap * vec4(world_pos, 0.0, 1.0)).xy;

	// Offset grid so the sample rectangle (corner) of a hex tile lies on the origin.
	local_pos.x -= u_grid_size.x * x_shift;
	local_pos.y -= u_grid_size.y * 0.5;
	
	// Hexagonal grid math
	vec2 cell = vec2(u_grid_size.x * 0.75, u_grid_size.y);
	local_pos /= cell;
	local_pos.y += floor(local_pos.x) * 0.5;
	local_pos = abs(fract(local_pos) - 0.5);

	float line = u_line_width / min(u_grid_size.x, u_grid_size.y);
	float edge = abs(1.0 - max(local_pos.x * 1.5 + local_pos.y, local_pos.y * 2.0));

	// IF NO ANTI-ALIASING USE THIS
	//float d = 1.0 - step(line, edge);
	//vec3 color = vec3(d);
	//color *= u_grid_color;
	//out_color = vec4(color, d);

	// IF ANTI-ALIASING USE THIS (also mixes in black outline)
	float gridAlpha = smoothstep(line, 0.0, edge);
	vec3 finalColor = mix(vec3(0.0), u_grid_color, gridAlpha);
	out_color = vec4(finalColor, gridAlpha);
}
)src";

const char *const szTILEMAPGRIDHEXPOINTTOP_FRAGMENTSHADER = R"src(
#version 140

uniform mat4					u_inv_tilemap;	// World to local space transform matrix of the infinite TileMapLayer grid
uniform vec2					u_position;		// Centered position of the quad viewport in world space
uniform vec2					u_dimensions;	// Width and height of entire render quad
uniform vec2					u_grid_size;	// Width and height of the tiles in the grid
uniform vec3					u_grid_color;	// Color of the grid lines
uniform float					u_line_width;	// Grid line width in world units

smooth in vec2					interp_uv;
out vec4						out_color;

const float x_shift = 0.8660254038; // AKA sin(3.1415926535 / 3.0)

void main()
{
	// Compute this fragment's world-space position.
	vec2 quad_offset = (interp_uv - vec2(0.5)) * u_dimensions;
	quad_offset.y *= -1.0; // Harmony world up is positive Y, but UV's are the opposite
	vec2 world_pos = u_position + quad_offset;

	// Convert the fragment position into the TileMapLayer's local coordinate system.
	vec2 local_pos = (u_inv_tilemap * vec4(world_pos, 0.0, 1.0)).xy;

	// Offset grid so the sample rectangle (corner) of a hex tile lies on the origin.
	local_pos.x -= u_grid_size.x * 0.5;
	local_pos.y -= u_grid_size.y * x_shift;
	
	// Hexagonal grid math
	vec2 cell = vec2(u_grid_size.x, u_grid_size.y * 0.75);
	local_pos /= cell;
	local_pos.x += floor(local_pos.y) * 0.5;
	local_pos = abs(fract(local_pos) - 0.5);

	float line = u_line_width / min(u_grid_size.x, u_grid_size.y);
	float edge = abs(1.0 - max(local_pos.y * 1.5 + local_pos.x, local_pos.x * 2.0));

	// IF NO ANTI-ALIASING USE THIS
	//float d = 1.0 - step(line, edge);
	//vec3 color = vec3(d);
	//color *= u_grid_color;
	//out_color = vec4(color, d);

	// IF ANTI-ALIASING USE THIS (also mixes in black outline)
	float gridAlpha = smoothstep(line, 0.0, edge);
	vec3 finalColor = mix(vec3(0.0), u_grid_color, gridAlpha);
	out_color = vec4(finalColor, gridAlpha);
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
	m_pTileMapHalfSquareShader(nullptr),
	m_pTileMapIsometricShader(nullptr),
	m_pTileMapHexFlatTopShader(nullptr),
	m_pTileMapHexPointTopShader(nullptr)
{
	ChildAppend(m_CheckerGrid);

	m_pCheckerGridShader = HY_NEW HyShader(HYSHADERPROG_Primitive);
	m_pCheckerGridShader->SetSourceCodePtrs({szCHECKERGRID_VERTEXSHADER}, HYSHADER_Vertex);
	m_pCheckerGridShader->AddVertexAttribute("attr_pos", HyShaderVariable::vec2);
	m_pCheckerGridShader->AddVertexAttribute("attr_uv", HyShaderVariable::vec2);
	m_pCheckerGridShader->SetSourceCodePtrs({szCHECKERGRID_FRAGMENTSHADER}, HYSHADER_Fragment);
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
	m_pOverGridShader->SetSourceCodePtrs({szCHECKERGRID_VERTEXSHADER}, HYSHADER_Vertex);
	m_pOverGridShader->AddVertexAttribute("attr_pos", HyShaderVariable::vec2);
	m_pOverGridShader->AddVertexAttribute("attr_uv", HyShaderVariable::vec2);
	m_pOverGridShader->SetSourceCodePtrs({szOVERGRID_FRAGMENTSHADER}, HYSHADER_Fragment);
	m_pOverGridShader->Finalize();

	m_OverGrid.SetShader(m_pOverGridShader);
	m_OverGrid.SetDisplayOrder(99999);
	
	// Misc shaders
	m_pTileMapSquareShader = HY_NEW HyShader(HYSHADERPROG_Primitive);
	m_pTileMapSquareShader->SetSourceCodePtrs({szCHECKERGRID_VERTEXSHADER}, HYSHADER_Vertex);
	m_pTileMapSquareShader->AddVertexAttribute("attr_pos", HyShaderVariable::vec2);
	m_pTileMapSquareShader->AddVertexAttribute("attr_uv", HyShaderVariable::vec2);
	m_pTileMapSquareShader->SetSourceCodePtrs({szTILEMAPGRID_FRAGMENTSHADER, szTILEMAPGRIDSQUARESDF_FRAGMENTSHADER}, HYSHADER_Fragment);
	m_pTileMapSquareShader->Finalize();

	m_pTileMapHalfSquareShader = HY_NEW HyShader(HYSHADERPROG_Primitive);
	m_pTileMapHalfSquareShader->SetSourceCodePtrs({ szCHECKERGRID_VERTEXSHADER }, HYSHADER_Vertex);
	m_pTileMapHalfSquareShader->AddVertexAttribute("attr_pos", HyShaderVariable::vec2);
	m_pTileMapHalfSquareShader->AddVertexAttribute("attr_uv", HyShaderVariable::vec2);
	m_pTileMapHalfSquareShader->SetSourceCodePtrs({szTILEMAPGRID_FRAGMENTSHADER, szTILEMAPGRIDHALFSQUARESDF_FRAGMENTSHADER}, HYSHADER_Fragment);
	m_pTileMapHalfSquareShader->Finalize();

	m_pTileMapIsometricShader = HY_NEW HyShader(HYSHADERPROG_Primitive);
	m_pTileMapIsometricShader->SetSourceCodePtrs({szCHECKERGRID_VERTEXSHADER}, HYSHADER_Vertex);
	m_pTileMapIsometricShader->AddVertexAttribute("attr_pos", HyShaderVariable::vec2);
	m_pTileMapIsometricShader->AddVertexAttribute("attr_uv", HyShaderVariable::vec2);
	m_pTileMapIsometricShader->SetSourceCodePtrs({szTILEMAPGRIDSISOMETRIC_FRAGMENTSHADER}, HYSHADER_Fragment);
	m_pTileMapIsometricShader->Finalize();

	m_pTileMapHexFlatTopShader = HY_NEW HyShader(HYSHADERPROG_Primitive);
	m_pTileMapHexFlatTopShader->SetSourceCodePtrs({szCHECKERGRID_VERTEXSHADER}, HYSHADER_Vertex);
	m_pTileMapHexFlatTopShader->AddVertexAttribute("attr_pos", HyShaderVariable::vec2);
	m_pTileMapHexFlatTopShader->AddVertexAttribute("attr_uv", HyShaderVariable::vec2);
	m_pTileMapHexFlatTopShader->SetSourceCodePtrs({szTILEMAPGRIDHEXFLATTOP_FRAGMENTSHADER}, HYSHADER_Fragment);
	m_pTileMapHexFlatTopShader->Finalize();

	m_pTileMapHexPointTopShader = HY_NEW HyShader(HYSHADERPROG_Primitive);
	m_pTileMapHexPointTopShader->SetSourceCodePtrs({szCHECKERGRID_VERTEXSHADER}, HYSHADER_Vertex);
	m_pTileMapHexPointTopShader->AddVertexAttribute("attr_pos", HyShaderVariable::vec2);
	m_pTileMapHexPointTopShader->AddVertexAttribute("attr_uv", HyShaderVariable::vec2);
	m_pTileMapHexPointTopShader->SetSourceCodePtrs({szTILEMAPGRIDHEXPOINTTOP_FRAGMENTSHADER}, HYSHADER_Fragment);
	m_pTileMapHexPointTopShader->Finalize();
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

HyShader *ProjectDraw::GetTileMapGridShader(HyTileMapLayout eLayout) const
{
	switch(eLayout)
	{
	case HYTILEMAPLAYOUT_Unknown:
	case HYTILEMAPLAYOUT_Square:
		return m_pTileMapSquareShader;

	case HYTILEMAPLAYOUT_HalfOffsetSquare:
		return m_pTileMapHalfSquareShader;

	case HYTILEMAPLAYOUT_Isometric:
	case HYTILEMAPLAYOUT_IsometricStaggerX:
	case HYTILEMAPLAYOUT_IsometricStaggerY:
		return m_pTileMapIsometricShader;

	case HYTILEMAPLAYOUT_HexagonFlatTop:
		return m_pTileMapHexFlatTopShader;

	case HYTILEMAPLAYOUT_HexagonPointTop:
		return m_pTileMapHexPointTopShader;

	default:
		HyGuiLog("ProjectDraw::GetTileMapGridShader() - Unknown tilemap layout type: " + QString::number(eLayout), LOGTYPE_Error);
		break;
	}

	return nullptr;
}
