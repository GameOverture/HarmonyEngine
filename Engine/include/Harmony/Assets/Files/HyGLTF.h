/**************************************************************************
 *	HyGLTF.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2018 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyGLTF_h__
#define HyGLTF_h__

#include "Afx/HyStdAfx.h"
#include "Assets/Files/IHyFileData.h"
#include "Renderer/Components/HyRenderBuffer.h"

#include "rapidjson/document.h"

#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#ifdef HY_PLATFORM_GUI
	#define TINYGLTF_NO_FS
#endif
#define TINYGLTF_NOEXCEPTION
#define TINYGLTF_USE_CPP14
#define TINYGLTF_USE_RAPIDJSON
#include "tinygltf/tiny_gltf.h"

class HyGLTF : public IHyFileData
{
	const std::string					m_sIDENTIFIER;
	tinygltf::Model						m_AssetData;

	std::vector<HyVertexBufferHandle>	m_BufferOffsetHandleList;

	GLuint								m_hVao;

	enum VertexAttributes
	{
		VA_Position = 0,
		VA_Normal,
		VA_Tangent,
		VA_UV
	};

public:
	HyGLTF(const std::string &sIdentifier);
	~HyGLTF();

	const std::string &GetIdentifier() const;

	virtual void OnLoadThread() override;
	virtual void OnRenderThread(IHyRenderer &rendererRef) override;

	void AppendRenderStates(HyRenderBuffer &renderBufferRef, int32 iSceneIndex = -1);

private:
	void TraverseNode(const tinygltf::Node &nodeRef, glm::mat4 transformMtx);
	void ProcessNode(const tinygltf::Node &nodeRef, glm::mat4 &transformMtxRef);
};

#endif /* HyAtlas_h__ */
