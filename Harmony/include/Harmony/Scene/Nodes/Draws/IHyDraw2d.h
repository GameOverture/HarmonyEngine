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

#include "Scene/Nodes/Transforms/IHyTransform2d.h"

#include "Assets/Data/IHyData.h"

#include "Renderer/Components/HyRenderState.h"
#include "Scene/Nodes/Misc/HyCamera.h"

class IHyDraw2d : public IHyTransform2d
{
	friend class HyScene;
	friend class HyAssetManager;

	static HyAssetManager *			sm_pAssetManager;

protected:
	const std::string				m_sNAME;
	const std::string				m_sPREFIX;

	// Data loading
	IHyData *						m_pData;
	HyLoadState						m_eLoadState;

	enum eAttributes
	{
		ATTRIBFLAG_Scissor					= 1 << 0,
		ATTRIBFLAG_MouseInput				= 1 << 1,
		ATTRIBFLAG_HasBoundingVolume		= 1 << 2,
		ATTRIBFLAG_BoundingVolumeDirty		= 1 << 3,
		ATTRIBFLAG_Physics					= 1 << 4
	};
	uint32							m_uiAttributes;

	enum eMouseInputState
	{
		MOUSEINPUT_None = 0,
		MOUSEINPUT_Hover,
		MOUSEINPUT_Down
	};
	eMouseInputState				m_eMouseInputState;
	void *							m_pMouseInputUserParam;

	HyCoordinateType				m_eCoordType;
	int32							m_iDisplayOrder;	// Higher values are displayed front-most
	int32							m_iDisplayOrderMax;	// The highest display order in this hierarchy (children attached)
	HyRenderState					m_RenderState;
	HyRectangle<int32>				m_LocalScissorRect;

	float							m_fAlpha;
	float							m_fPrevAlphaValue;

	HyShaderUniforms 				m_ShaderUniforms;


public:
	HyTweenVec3						topColor;
	HyTweenVec3						botColor;
	HyTweenFloat					alpha;

	IHyDraw2d(HyType eInstType, const char *szPrefix, const char *szName);
	virtual ~IHyDraw2d(void);

	const std::string &GetName();
	const std::string &GetPrefix();

	HyCoordinateType GetCoordinateType();
	void SetCoordinateType(HyCoordinateType eCoordType, HyCamera2d *pCameraToCovertFrom);

	int32 GetDisplayOrder() const;
	int32 GetDisplayOrderMax() const;
	void SetDisplayOrder(int32 iOrderValue);

	void SetTint(float fR, float fG, float fB);
	void SetTint(uint32 uiColor);

	void EnableMouseInput(bool bEnable, void *pUserParam = NULL);
	void EnableCollider(bool bEnable);
	void EnablePhysics(bool bEnable);

	const HyRectangle<int32> &GetScissor();
	void SetScissor(int32 uiX, int32 uiY, uint32 uiWidth, uint32 uiHeight);
	void ClearScissor();

	int32 GetShaderId();
	void SetCustomShader(IHyShader *pShader);

	bool IsLoaded() const;

	void Load();
	virtual void Unload();

protected:
	void MakeBoundingVolumeDirty();

private:
	HyLoadState GetLoadState()									{ return m_eLoadState; }
	const HyRenderState &GetRenderState() const					{ return m_RenderState; }

	void SetData(IHyData *pData);
	void SetLoaded();
	IHyData *GetData()											{ return m_pData; }

	void WriteShaderUniformBuffer(char *&pRefDataWritePos);

	virtual void OnUpdate() { };

	virtual void OnCalcBoundingVolume() { }								// Should calculate the local bounding volume in 'm_BoundingVolume'

	virtual void OnDataLoaded() { }
	virtual void OnUpdateUniforms() { }									// Upon updating, this function will set the shaders' uniforms when using the default shader
	virtual void OnWriteDrawBufferData(char *&pRefDataWritePos) { }		// This function is responsible for incrementing the passed in reference pointer the size of the data written

	virtual void OnMouseEnter(void *pUserParam) { }
	virtual void OnMouseLeave(void *pUserParam) { }
	virtual void OnMouseDown(void *pUserParam) { }
	virtual void OnMouseUp(void *pUserParam) { }
	virtual void OnMouseClicked(void *pUserParam) { }

	virtual void InstUpdate() override final;
};

#endif /* __IHyInst2d_h__ */
