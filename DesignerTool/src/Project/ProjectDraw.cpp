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
    interp_vUV.x = UVcoord.x;
    interp_vUV.y = UVcoord.y;

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
    vec2 vScreenCoords = (interp_vUV.xy * (u_vDimensions /** 0.5f*/)) / u_fGridSize;
    out_vColor = mix(u_vGridColor1, u_vGridColor2, step((float(int(floor(vScreenCoords.x) + floor(vScreenCoords.y)) & 1)), 0.9));
    out_vColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}
)src";
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CheckerGrid::CheckerGrid()
{
}

CheckerGrid::~CheckerGrid()
{
}

void CheckerGrid::SetDimensions(int iWidth, int iHeight)
{
    m_vDimensions.x = iWidth;
    m_vDimensions.y = iHeight;
    GetShape().SetAsBox(m_vDimensions.x * 0.5f, m_vDimensions.y * 0.5f);
}

/*virtual*/ void CheckerGrid::OnUpdateUniforms()
{
    glm::mat4 mtx;
    HyPrimitive2d::GetWorldTransform(mtx);

    m_ShaderUniforms.Set("u_mtxTransform", mtx);
    m_ShaderUniforms.Set("u_fGridSize", 25.0f);
    m_ShaderUniforms.Set("u_vDimensions", m_vDimensions);
    m_ShaderUniforms.Set("u_vGridColor1", glm::vec4(106.0f / 255.0f, 105.0f / 255.0f, 113.0f / 255.0f, 1.0f));
    m_ShaderUniforms.Set("u_vGridColor2", glm::vec4(93.0f / 255.0f, 93.0f / 255.0f, 97.0f / 255.0f, 1.0f));
}

/*virtual*/ void CheckerGrid::OnWriteDrawBufferData(char *&pRefDataWritePos)
{
    if(GetNumVerts() != 6)
        HyGuiLog("CheckerGrid::OnWriteDrawBufferData is trying to draw a primitive that's not a quad", LOGTYPE_Error);

    for(int i = 0; i < 6; ++i)
    {
        *reinterpret_cast<glm::vec2 *>(pRefDataWritePos) = m_pVertBuffer[i];
        pRefDataWritePos += sizeof(glm::vec2);

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

        *reinterpret_cast<glm::vec2 *>(pRefDataWritePos) = vUV;
        pRefDataWritePos += sizeof(glm::vec2);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ProjectDraw::ProjectDraw(IHyApplication &hyApp) :   IDraw(nullptr, hyApp)
{
    IHyShader *pShader_CheckerGrid = IHyRenderer::MakeCustomShader();
    pShader_CheckerGrid->SetSourceCode(szCHECKERGRID_VERTEXSHADER, HYSHADER_Vertex);
    pShader_CheckerGrid->SetVertexAttribute("attr_vPosition", HYSHADERVAR_vec2);
    pShader_CheckerGrid->SetVertexAttribute("attr_vUVcoord", HYSHADERVAR_vec2);
    pShader_CheckerGrid->SetSourceCode(szCHECKERGRID_FRAGMENTSHADER, HYSHADER_Fragment);
    pShader_CheckerGrid->Finalize(HYSHADERPROG_Primitive);

    m_CheckerGrid.SetCustomShader(pShader_CheckerGrid);
    m_CheckerGrid.SetDisplayOrder(-1000);
    m_CheckerGrid.SetDimensions(10000, 10000);  // Use a large size that is a multiple of grid size (25)

    ChildAppend(m_CheckerGrid);
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
