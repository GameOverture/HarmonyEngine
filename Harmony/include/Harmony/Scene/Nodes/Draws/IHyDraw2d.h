/**************************************************************************
 *	IHyDraw2d.h
 *
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __IHyInst2d_h__
#define __IHyInst2d_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Leafs/IHyLeafNode2d.h"
#include "Scene/Nodes/Misc/HyCamera.h"
#include "Scene/Nodes/Components/HyColor.h"
#include "Scene/Nodes/Components/HyScissor.h"
#include "Assets/Nodes/IHyNodeData.h"
#include "Assets/Loadables/IHyShader.h"
#include "Renderer/Components/HyRenderState.h"
#include "Renderer/Components/HyShaderUniforms.h"

#include <set>

class IHyDraw2d : public IHyLeafNode2d
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

	int32							m_iDisplayOrder;	// Higher values are displayed front-most
	HyRenderState					m_RenderState;
	HyShaderUniforms 				m_ShaderUniforms;

public:
	HyColor							color;
	HyScissor						scissor;

public:
	IHyDraw2d(HyType eInstType, const char *szPrefix, const char *szName, HyEntity2d *pParent = nullptr);
	virtual ~IHyDraw2d(void);

	const std::string &GetName();
	const std::string &GetPrefix();

	IHyNodeData *AcquireData();

	HyCoordinateType GetCoordinateType();
	void SetCoordinateType(HyCoordinateType eCoordType);

	int32 GetDisplayOrder() const;
	void SetDisplayOrder(int32 iOrderValue);

	int32 GetShaderId();
	void SetCustomShader(IHyShader *pShader);

	bool IsLoaded() const;

	void Load();
	void Unload();
	
protected:
	bool IsSelfLoaded();
	IHyNodeData *UncheckedGetData();
	void MakeBoundingVolumeDirty();

	virtual void SetNewChildAttributes(IHyNode2d &childInst);

private:
	const HyRenderState &GetRenderState() const;

	void WriteShaderUniformBuffer(char *&pRefDataWritePos);

	virtual void OnUpdate() { };
	virtual void OnDataAcquired() { }									// Invoked once on the first time this node's data is queried
	virtual void OnLoaded() { }											// HyAssets invokes this once all required IHyLoadables are fully loaded for this node
	virtual void OnCalcBoundingVolume() { }								// Should calculate the local bounding volume in 'm_BoundingVolume'
	virtual void OnUpdateUniforms() { }									// Upon updating, this function will set the shaders' uniforms when using the default shader
	virtual void OnWriteDrawBufferData(char *&pRefDataWritePos) { }		// This function is responsible for incrementing the passed in reference pointer the size of the data written

	virtual void InstUpdate() override final;

protected:
	virtual void _SetScissor(HyScissor &scissorRef, bool bOverrideExplicitChildren);			// Only Entity2d/3d will invoke this
	virtual int32 _SetDisplayOrder(int32 iOrderValue, bool bOverrideExplicitChildren);			// Only Entity2d/3d will invoke this
};

#endif /* __IHyInst2d_h__ */
