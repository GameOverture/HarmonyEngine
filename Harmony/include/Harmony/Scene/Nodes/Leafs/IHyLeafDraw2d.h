/**************************************************************************
*	IHyLeafDraw2d.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IHyLeafDraw2d_h__
#define IHyLeafDraw2d_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/IHyNodeDraw2d.h"
#include "Assets/Nodes/IHyNodeData.h"
#include "Assets/Loadables/IHyShader.h"
#include "Renderer/Components/HyRenderState.h"
#include "Renderer/Components/HyShaderUniforms.h"

#include <set>

class IHyLeafDraw2d : public IHyNodeDraw2d
{	
	friend class HyScene;
	friend class HyAssets;

	static HyAssets *				sm_pHyAssets;

	HyLoadState						m_eLoadState;
	std::set<uint32>				m_RequiredAtlasIndices;
	std::set<int32>					m_RequiredCustomShaders;

	IHyNodeData *					m_pData;

protected:
	const std::string				m_sNAME;
	const std::string				m_sPREFIX;

	HyRenderState					m_RenderState;
	HyShaderUniforms 				m_ShaderUniforms;

public:
	IHyLeafDraw2d(HyType eInstType, const char *szPrefix, const char *szName, HyEntity2d *pParent);
	virtual ~IHyLeafDraw2d();

	const std::string &GetName();
	const std::string &GetPrefix();

	bool IsLoaded() const;

	IHyNodeData *AcquireData();

	HyCoordinateType GetCoordinateType();
	void SetCoordinateType(HyCoordinateType eCoordType);

	int32 GetDisplayOrder() const;
	void SetDisplayOrder(int32 iOrderValue);

	int32 GetShaderId();
	void SetCustomShader(IHyShader *pShader);

	void Load();
	void Unload();

protected:
	IHyNodeData *UncheckedGetData();
	const HyRenderState &GetRenderState() const;

	void WriteShaderUniformBuffer(char *&pRefDataWritePos);

	virtual void OnUpdate() { };
	virtual void OnDataAcquired() { }									// Invoked once on the first time this node's data is queried
	virtual void OnLoaded() { }											// HyAssets invokes this once all required IHyLoadables are fully loaded for this node
	virtual void OnCalcBoundingVolume() { }								// Should calculate the local bounding volume in 'm_BoundingVolume'
	virtual void OnUpdateUniforms() { }									// Upon updating, this function will set the shaders' uniforms when using the default shader
	virtual void OnWriteDrawBufferData(char *&pRefDataWritePos) { }		// This function is responsible for incrementing the passed in reference pointer the size of the data written
};

#endif /* IHyLeafDraw2d_h__ */
