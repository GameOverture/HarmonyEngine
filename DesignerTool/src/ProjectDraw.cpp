/**************************************************************************
 *	ProjectDraw.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "ProjectDraw.h"

const char * const szCHECKERGRID_VERTEXSHADER = R"src(
#version 400

/*layout(location = 0)*/ in vec2 position;
/*layout(location = 1)*/ in vec2 UVcoord;

smooth out vec2 interpUV;

uniform mat4 transformMtx;
uniform mat4 u_mtxWorldToCamera;
uniform mat4 u_mtxCameraToClip;

void main()
{
    interpUV.x = UVcoord.x;
    interpUV.y = UVcoord.y;

    vec4 temp = transformMtx * vec4(position, 0, 1);
    temp = u_mtxWorldToCamera * temp;
    gl_Position = u_mtxCameraToClip * temp;
}
)src";


const char *const szCHECKERGRID_FRAGMENTSHADER = R"src(
#version 400

in vec2 interpUV;
out vec4 FragColor;

uniform float uGridSize;
uniform vec2 uResolution;
uniform vec4 gridColor1;
uniform vec4 gridColor2;

void main()
{
    vec2 screenCoords = (interpUV.xy * (uResolution /** 0.5f*/)) / uGridSize;
    FragColor = mix(gridColor1, gridColor2, step((float(int(floor(screenCoords.x) + floor(screenCoords.y)) & 1)), 0.9));
}
)src";

CheckerGrid::CheckerGrid()
{
}

CheckerGrid::~CheckerGrid()
{
}

void CheckerGrid::SetSurfaceSize(int iWidth, int iHeight)
{
    m_Resolution.x = iWidth;
    m_Resolution.y = iHeight;
    SetAsQuad(m_Resolution.x, m_Resolution.y, false);
    pos.Set(m_Resolution.x * -0.5f, m_Resolution.y * -0.5f);
}

/*virtual*/ void CheckerGrid::OnUpdateUniforms()
{
    glm::mat4 mtx;
    HyPrimitive2d::GetWorldTransform(mtx);

    m_ShaderUniforms.Set("transformMtx", mtx);
    m_ShaderUniforms.Set("uGridSize", 25.0f);
    m_ShaderUniforms.Set("uResolution", m_Resolution);
    m_ShaderUniforms.Set("gridColor1", glm::vec4(106.0f / 255.0f, 105.0f / 255.0f, 113.0f / 255.0f, 1.0f));
    m_ShaderUniforms.Set("gridColor2", glm::vec4(93.0f / 255.0f, 93.0f / 255.0f, 97.0f / 255.0f, 1.0f));
}

/*virtual*/ void CheckerGrid::OnWriteDrawBufferData(char *&pRefDataWritePos)
{
    HyAssert(m_RenderState.GetNumVerticesPerInstance() == 4, "CheckerGrid::OnWriteDrawBufferData is trying to draw a primitive that's not a quad");

    for(int i = 0; i < 4; ++i)
    {
        *reinterpret_cast<glm::vec2 *>(pRefDataWritePos) = m_pDrawBuffer[i];
        pRefDataWritePos += sizeof(glm::vec2);

        glm::vec2 vUV;
        switch(i)
        {
        case 0:
            vUV.x = 1.0f;
            vUV.y = 0.0f;
            break;

        case 1:
            vUV.x = 0.0f;
            vUV.y = 0.0f;
            break;

        case 2:
            vUV.x = 1.0f;
            vUV.y = 1.0f;
            break;

        case 3:
            vUV.x = 0.0f;
            vUV.y = 1.0f;
            break;
        }

        *reinterpret_cast<glm::vec2 *>(pRefDataWritePos) = vUV;
        pRefDataWritePos += sizeof(glm::vec2);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ProjectDraw::ProjectDraw(IHyApplication &hyApp) : IDraw(nullptr, hyApp)
{
    IHyShader *pShader_CheckerGrid = IHyRenderer::MakeCustomShader();
    pShader_CheckerGrid->SetSourceCode(szCHECKERGRID_VERTEXSHADER, HYSHADER_Vertex);
    pShader_CheckerGrid->SetVertexAttribute("position", HYSHADERVAR_vec2);
    pShader_CheckerGrid->SetVertexAttribute("UVcoord", HYSHADERVAR_vec2);
    pShader_CheckerGrid->SetSourceCode(szCHECKERGRID_FRAGMENTSHADER, HYSHADER_Fragment);
    pShader_CheckerGrid->Finalize(HYSHADERPROG_Primitive);

    m_CheckerGridBG.SetCustomShader(pShader_CheckerGrid);
    m_CheckerGridBG.SetDisplayOrder(-1000);
    m_CheckerGridBG.SetSurfaceSize(10000, 10000);  // Use a large size that is a multiple of grid size (25)

    ChildAppend(m_CheckerGridBG);
}

/*virtual*/ ProjectDraw::~ProjectDraw()
{
}

/*virtual*/ void ProjectDraw::OnShow(IHyApplication &hyApp)
{
}

/*virtual*/ void ProjectDraw::OnHide(IHyApplication &hyApp)
{
}
