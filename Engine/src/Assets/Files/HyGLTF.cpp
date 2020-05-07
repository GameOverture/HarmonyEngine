/**************************************************************************
 *	HyAtlas.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2015 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Assets/Files/HyGLTF.h"
#include "Renderer/IHyRenderer.h"
#include "Diagnostics/Console/HyConsole.h"
#include "HyEngine.h"

#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/quaternion.hpp"

extern std::string Hy_DataDir();

HyGLTF::HyGLTF(const std::string &sIdentifier) :
	IHyFileData(HYFILE_GLTF),
	m_sIDENTIFIER(sIdentifier)
{
}

HyGLTF::~HyGLTF()
{
}

const std::string &HyGLTF::GetIdentifier() const
{
	return m_sIDENTIFIER;
}

void HyGLTF::OnLoadThread()
{
	if(GetLoadableState() == HYLOADSTATE_Queued)
	{
		//tinygltf::TinyGLTF loader;
		//std::string sError;
		//std::string sWarning;
		//std::string sFilePath = Hy_DataDir() + HYASSETS_PrefabDir + m_sIDENTIFIER + ".gltf";
		//bool bLoadSuccess = loader.LoadASCIIFromFile(&m_AssetData, &sError, &sWarning, sFilePath);
		//if(bLoadSuccess == false)
		//{
		//	HyLogError("HyGLTF::OnLoadThread failed: " << sError.c_str());
		//	return;
		//}
	}
}

void HyGLTF::OnRenderThread(IHyRenderer &rendererRef)
{
	if(GetLoadableState() == HYLOADSTATE_Queued)
	{
		//rendererRef.

		for(uint32 i = 0; i < static_cast<uint32>(m_AssetData.buffers.size()); ++i)
			m_BufferOffsetHandleList.push_back(rendererRef.AppendVertexData3d(&m_AssetData.buffers[i].data[0], static_cast<uint32>(m_AssetData.buffers[i].data.size())));
	}
	else // GetLoadableState() == HYLOADSTATE_Discarded
	{
		//rendererRef.RemoveVertexData3d(
	}
}

void HyGLTF::AppendRenderStates(HyRenderBuffer &renderBufferRef, int32 iSceneIndex /*= -1*/)
{
	glGenVertexArrays(1, &m_hVao);
	glBindVertexArray(m_hVao);
	

	if(iSceneIndex < 0)
	{
		if(m_AssetData.defaultScene < 0)
		{
			HyLogWarning("IHyRenderer::AppendDrawable3d - default scene is not specified");
			return;
		}

		iSceneIndex = m_AssetData.defaultScene;
	}

	for(uint32 i = 0; i < m_AssetData.scenes[iSceneIndex].nodes.size(); ++i)
	{
		int iNodeIndex = m_AssetData.scenes[iSceneIndex].nodes[i];
		
		glm::mat4 transformMtx(1.0f);
		TraverseNode(m_AssetData.nodes[iNodeIndex], transformMtx);
	}
}

void HyGLTF::TraverseNode(const tinygltf::Node &nodeRef, glm::mat4 transformMtx)
{
	ProcessNode(nodeRef, transformMtx);

	for(uint32 i = 0; i < nodeRef.children.size(); ++i)
		TraverseNode(m_AssetData.nodes[nodeRef.children[i]], transformMtx);
}

void HyGLTF::ProcessNode(const tinygltf::Node &nodeRef, glm::mat4 &transformMtxRef)
{
	if(nodeRef.matrix.empty() == false)
	{
		glm::mat4 mtx;
		memcpy(glm::value_ptr(mtx), &nodeRef.matrix[0], sizeof(glm::mat4));

		transformMtxRef *= mtx;
	}
	else if(!nodeRef.translation.empty() || !nodeRef.rotation.empty() || !nodeRef.scale.empty())
	{
		glm::vec3 vTranslation(0.0f);
		glm::quat quaternion(0.0f, 0.0f, 0.0f, 0.0f);
		glm::vec3 vScale(1.0f);

		if(nodeRef.translation.size() != 0)
			memcpy(glm::value_ptr(vTranslation), &nodeRef.translation[0], sizeof(glm::vec3));
		if(nodeRef.rotation.size() != 0)
			memcpy(glm::value_ptr(quaternion), &nodeRef.rotation[0], sizeof(glm::quat));
		if(nodeRef.scale.size() != 0)
			memcpy(glm::value_ptr(vScale), &nodeRef.scale[0], sizeof(glm::vec3));

		glm::mat4 transformMtx;
		glm::translate(transformMtx, vTranslation);

		glm::mat4 rotationMtx;
		rotationMtx = glm::toMat4(quaternion);

		glm::mat4 scaleMtx;
		glm::scale(scaleMtx, vTranslation);

		transformMtxRef *= (transformMtx * rotationMtx * scaleMtx);
	}

	if(nodeRef.mesh >= 0)
	{
		const tinygltf::Mesh &meshRef = m_AssetData.meshes[nodeRef.mesh];
		for(uint32 i = 0; i < meshRef.primitives.size(); ++i)
		{
			const std::map<std::string, int> &attribMapRef = meshRef.primitives[i].attributes;

			if(attribMapRef.find("POSITION") != attribMapRef.end())
			{
				int iAccessorIndex = attribMapRef.at("POSITION");
				const tinygltf::BufferView &bufferViewRef = m_AssetData.bufferViews[m_AssetData.accessors[iAccessorIndex].bufferView];

				size_t uiBufferOffset = 0;// m_BufferOffsetHandleList[bufferViewRef.buffer];
				uiBufferOffset += bufferViewRef.byteOffset;
				uiBufferOffset += m_AssetData.accessors[iAccessorIndex].byteOffset;
				glVertexAttribPointer(VA_Position, 3, m_AssetData.accessors[iAccessorIndex].componentType, GL_FALSE, static_cast<GLsizei>(bufferViewRef.byteStride), reinterpret_cast<void *>(uiBufferOffset));
			}
			else
				HyError("glTF asset wanted to render a primitive that did not have a POSITION attribute");

			if(attribMapRef.find("NORMAL") != attribMapRef.end())
			{
				int iAccessorIndex = attribMapRef.at("NORMAL");
				const tinygltf::BufferView &bufferViewRef = m_AssetData.bufferViews[m_AssetData.accessors[iAccessorIndex].bufferView];

				size_t uiBufferOffset = 0;// m_BufferOffsetHandleList[bufferViewRef.buffer];
				uiBufferOffset += bufferViewRef.byteOffset;
				uiBufferOffset += m_AssetData.accessors[iAccessorIndex].byteOffset;
				glVertexAttribPointer(VA_Normal, 3, m_AssetData.accessors[iAccessorIndex].componentType, GL_FALSE, static_cast<GLsizei>(bufferViewRef.byteStride), reinterpret_cast<void *>(uiBufferOffset));
			}
			else
				HyError("glTF asset wanted to render a primitive that did not have a NORMAL attribute (they should had been generated when initially importing the asset)");

			if(attribMapRef.find("TANGENT") != attribMapRef.end())
			{
				int iAccessorIndex = attribMapRef.at("TANGENT");
				const tinygltf::BufferView &bufferViewRef = m_AssetData.bufferViews[m_AssetData.accessors[iAccessorIndex].bufferView];

				size_t uiBufferOffset = 0;// m_BufferOffsetHandleList[bufferViewRef.buffer];
				uiBufferOffset += bufferViewRef.byteOffset;
				uiBufferOffset += m_AssetData.accessors[iAccessorIndex].byteOffset;
				glVertexAttribPointer(VA_Tangent, 4, m_AssetData.accessors[iAccessorIndex].componentType, GL_FALSE, static_cast<GLsizei>(bufferViewRef.byteStride), reinterpret_cast<void *>(uiBufferOffset));
			}
			else
				HyError("glTF asset wanted to render a primitive that did not have a TANGENT attribute (they should had been generated when initially importing the asset)");

			if(attribMapRef.find("TEXCOORD_0") != attribMapRef.end())
			{
				int iAccessorIndex = attribMapRef.at("TEXCOORD_0");
				const tinygltf::BufferView &bufferViewRef = m_AssetData.bufferViews[m_AssetData.accessors[iAccessorIndex].bufferView];

				size_t uiBufferOffset = 0;// m_BufferOffsetHandleList[bufferViewRef.buffer];
				uiBufferOffset += bufferViewRef.byteOffset;
				uiBufferOffset += m_AssetData.accessors[iAccessorIndex].byteOffset;
				
				glVertexAttribPointer(VA_UV, 2, m_AssetData.accessors[iAccessorIndex].componentType, m_AssetData.accessors[iAccessorIndex].componentType != GL_FLOAT ? GL_TRUE : GL_FALSE, static_cast<GLsizei>(bufferViewRef.byteStride), reinterpret_cast<void *>(uiBufferOffset));
			}
			else
				HyError("glTF asset wanted to render a primitive that did not have a TEXCOORD_0 attribute (they should had been generated when initially importing the asset)");
		}
	}
}
