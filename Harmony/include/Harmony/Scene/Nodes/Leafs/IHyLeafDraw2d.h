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

class HyStencil;

#include <set>

// NOTE: This class should contain a copy of all the functions/members of IHyLeaf2d. Multiple inheritance is not an option
class IHyLeafDraw2d : public IHyNodeDraw2d
{
	friend class HyScene;
	friend class HyAssets;
	friend class HyShape2d;

protected:
	static HyAssets *				sm_pHyAssets;

	HyLoadState						m_eLoadState;
	std::set<int32>					m_RequiredCustomShaders;

	IHyNodeData *					m_pData;
	const std::string				m_sNAME;
	const std::string				m_sPREFIX;

	HyRenderState					m_RenderState;
	HyShaderUniforms 				m_ShaderUniforms;

	HyShape2d						m_BoundingVolume;
	b2AABB							m_aabbCached;

public:
	IHyLeafDraw2d(HyType eInstType, const char *szPrefix, const char *szName, HyEntity2d *pParent);
	virtual ~IHyLeafDraw2d();

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// NOTE: Below mutators manipulate data from derived classes "IHyNodeDraw2d" and "IHyNode". Handled in regard to being a "leaf"
	void SetEnabled(bool bEnabled);
	void SetPauseUpdate(bool bUpdateWhenPaused);

	void SetScissor(int32 uiLocalX, int32 uiLocalY, uint32 uiWidth, uint32 uiHeight);
	void ClearScissor(bool bUseParentScissor);

	void SetStencil(HyStencil *pStencil);
	void ClearStencil(bool bUseParentStencil);

	void SetDisplayOrder(int32 iOrderValue);
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	const std::string &GetName();
	const std::string &GetPrefix();

	IHyNodeData *AcquireData();

	const HyShape2d &GetBoundingVolume();
	const b2AABB &GetWorldAABB();
	HyShape2d *GetUserBoundingVolume(uint32 uiIndex);

	int32 GetCoordinateSystem();	// Returns -1 if using camera coordinates
	void UseCameraCoordinates();
	void UseWindowCoordinates(int32 iWindowIndex = 0);

	int32 GetShaderId();
	void SetCustomShader(IHyShader *pShader);

	virtual bool IsLoaded() const override;
	virtual void Load() override;
	virtual void Unload() override;

protected:
	virtual void CalcBoundingVolume() = 0;
	virtual void AcquireBoundingVolumeIndex(uint32 &uiStateOut, uint32 &uiSubStateOut) = 0;

	virtual void OnShapeSet(HyShape2d *pShape) { }

	virtual void NodeUpdate() override final;

	virtual void _SetScissor(const HyScreenRect<int32> &worldScissorRectRef, bool bIsOverriding) override;
	virtual void _SetStencil(HyStencil *pStencil, bool bIsOverriding) override;
	virtual int32 _SetDisplayOrder(int32 iOrderValue, bool bIsOverriding) override;
	virtual void _SetCoordinateSystem(int32 iWindowIndex, bool bIsOverriding) override;

	IHyNodeData *UncheckedGetData();
	const HyRenderState &GetRenderState() const;

	void WriteShaderUniformBuffer(char *&pRefDataWritePos);

	virtual bool IsLoadDataValid() { return true; }
	virtual void DrawUpdate() { }
	virtual void OnDataAcquired() { }									// Invoked once on the first time this node's data is queried
	virtual void OnLoaded() { }											// HyAssets invokes this once all required IHyLoadables are fully loaded for this node
	virtual void OnUpdateUniforms() { }									// Upon updating, this function will set the shaders' uniforms when using the default shader
	virtual void OnWriteDrawBufferData(char *&pRefDataWritePos) { }		// This function is responsible for incrementing the passed in reference pointer the size of the data written

#ifdef HY_PLATFORM_GUI
public:
	template<typename HYDATATYPE>
	void GuiOverrideData(jsonxx::Value &dataValueRef)
	{
		delete m_pData;
		m_pData = HY_NEW HYDATATYPE("+HyGuiOverride", dataValueRef, *sm_pHyAssets);
		OnDataAcquired();
	}
#endif
};

#endif /* IHyLeafDraw2d_h__ */
