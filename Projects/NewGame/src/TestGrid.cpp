#include "TestGrid.h"


TestGrid::TestGrid()
{
}

TestGrid::~TestGrid()
{
}

void TestGrid::SetResolution(int iWidth, int iHeight)
{
	m_Resolution.x = iWidth;
	m_Resolution.y = iHeight;
	SetAsQuad(m_Resolution.x, m_Resolution.y, false);
}

/*virtual*/ void TestGrid::OnUpdateUniforms()
{
	glm::mat4 mtx;
	GetWorldTransform(mtx);

	m_ShaderUniforms.Set("u_mtxTransform", mtx);
	m_ShaderUniforms.Set("uGridSize", 25.0f);
	m_ShaderUniforms.Set("uResolution", m_Resolution);
	m_ShaderUniforms.Set("gridColor1", glm::vec4(106.0f / 255.0f, 105.0f / 255.0f, 113.0f / 255.0f, 1.0f));
	m_ShaderUniforms.Set("gridColor2", glm::vec4(93.0f / 255.0f, 93.0f / 255.0f, 97.0f / 255.0f, 1.0f));
}

/*virtual*/ void TestGrid::OnWriteDrawBufferData(char *&pRefDataWritePos)
{
	//memcpy(pRefDataWritePos, &color.Get(), sizeof(glm::vec4));
	//pRefDataWritePos += sizeof(glm::vec4);

	//glm::mat4 mtx;
	//GetWorldTransform(mtx);
	//memcpy(pRefDataWritePos, &mtx, sizeof(glm::mat4));
	//pRefDataWritePos += sizeof(glm::mat4);

	//memcpy(pRefDataWritePos, &m_uiNumVerts, sizeof(uint32));
	//pRefDataWritePos += sizeof(uint32);

	memcpy(pRefDataWritePos, m_pDrawBuffer, m_RenderState.GetNumVerticesPerInstance() * sizeof(glm::vec2));
	pRefDataWritePos += m_RenderState.GetNumVerticesPerInstance() * sizeof(glm::vec2);
}
