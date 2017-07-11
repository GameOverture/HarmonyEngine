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

// NOTE: This class should contain a copy of all the functions/members of IHyLeaf2d. Multiple inheritance is not an option
class IHyLeafDraw2d : public IHyNodeDraw2d
{	
	friend class HyScene;
	friend class HyAssets;

	static HyAssets *				sm_pHyAssets;

	HyLoadState						m_eLoadState;
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

	void SetEnabled(bool bEnabled);
	void SetPauseUpdate(bool bUpdateWhenPaused);

	void SetScissor(int32 uiLocalX, int32 uiLocalY, uint32 uiWidth, uint32 uiHeight);
	void ClearScissor(bool bUseParentScissor);

	void SetDisplayOrder(int32 iOrderValue);

	const std::string &GetName();
	const std::string &GetPrefix();

	IHyNodeData *AcquireData();

	HyCoordinateType GetCoordinateType();
	void UseCameraCoordinates();
	void UseWindowCoordinates(uint32 uiWindowIndex);

	int32 GetShaderId();
	void SetCustomShader(IHyShader *pShader);

	virtual bool IsLoaded() const override;
	virtual void Load() override;
	virtual void Unload() override;

protected:
	virtual void NodeUpdate() override final;

	virtual void _SetScissor(const HyScreenRect<int32> &worldScissorRectRef, bool bIsOverriding) override;
	virtual int32 _SetDisplayOrder(int32 iOrderValue, bool bIsOverriding) override;

	IHyNodeData *UncheckedGetData();
	const HyRenderState &GetRenderState() const;

	void WriteShaderUniformBuffer(char *&pRefDataWritePos);

	virtual void DrawUpdate() { };
	virtual void OnDataAcquired() { }									// Invoked once on the first time this node's data is queried
	virtual void OnLoaded() { }											// HyAssets invokes this once all required IHyLoadables are fully loaded for this node
	virtual void OnCalcBoundingVolume() { }								// Should calculate the local bounding volume in 'm_BoundingVolume'
	virtual void OnUpdateUniforms() { }									// Upon updating, this function will set the shaders' uniforms when using the default shader
	virtual void OnWriteDrawBufferData(char *&pRefDataWritePos) { }		// This function is responsible for incrementing the passed in reference pointer the size of the data written
};

#endif /* IHyLeafDraw2d_h__ */
