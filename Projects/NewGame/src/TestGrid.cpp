#include "TestGrid.h"


TestGrid::TestGrid()
{
	SetAsQuad(500.0f, 500.0f, false);
}

TestGrid::~TestGrid()
{
}

/*virtual*/ void TestGrid::OnUpdateUniforms(HyShaderUniforms *pShaderUniformsRef)
{
	glm::mat4 mtx;
	GetWorldTransform(mtx);

	pShaderUniformsRef->Set("primitiveColor", color.Get());
	pShaderUniformsRef->Set("transformMtx", mtx);
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

	memcpy(pRefDataWritePos, m_pVertices, m_RenderState.GetNumVertices() * sizeof(glm::vec4));
	pRefDataWritePos += m_RenderState.GetNumVertices() * sizeof(glm::vec4);
}
