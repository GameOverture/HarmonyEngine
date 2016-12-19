/**************************************************************************
 *	IHyInst2d.h
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

#include "Scene/Transforms/IHyTransform2d.h"

#include "Assets/Data/IHyData.h"

#include "Renderer/HyRenderState.h"
#include "Renderer/Viewport/HyCamera.h"

//typedef void (*HyWriteDrawBufferDataOverride)(char *&);
//typedef void (*HyUpdateUniformOverride)(HyShaderUniforms *);

class IHyInst2d : public IHyTransform2d
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
	bool							m_bInvalidLoad;

	// Attributes
	HyCoordinateType				m_eCoordType;
	int32							m_iDisplayOrder;	// Higher values are displayed front-most
	HyRenderState					m_RenderState;

	float							m_fAlpha;

	HyShaderUniforms 				m_ShaderUniforms;

public:
	HyTweenVec3						topColor;
	HyTweenVec3						botColor;
	HyTweenFloat					alpha;

	IHyInst2d(HyType eInstType, const char *szPrefix, const char *szName);
	virtual ~IHyInst2d(void);

	const std::string &GetName();
	const std::string &GetPrefix();

	virtual bool IsLoaded() const;

	HyCoordinateType GetCoordinateType();
	void SetCoordinateType(HyCoordinateType eCoordType, HyCamera2d *pCameraToCovertFrom);

	int32 GetDisplayOrder() const;
	virtual void SetDisplayOrder(int32 iOrderValue);

	void SetTint(float fR, float fG, float fB);

	int32 GetShaderId();
	void SetCustomShader(IHyShader *pShader);

	virtual void Load();
	virtual void Unload();

private:
	HyLoadState GetLoadState()									{ return m_eLoadState; }
	const HyRenderState &GetRenderState() const					{ return m_RenderState; }

	void SetData(IHyData *pData);
	void SetLoaded();
	IHyData *GetData()											{ return m_pData; }

	void WriteShaderUniformBuffer(char *&pRefDataWritePos);

	virtual void OnUpdate();

	virtual void OnInstUpdate() = 0;
	virtual void OnDataLoaded() = 0;
	virtual void OnUpdateUniforms() = 0;								// Upon updating, this function will set the shaders' uniforms when using the default shader
	virtual void OnWriteDrawBufferData(char *&pRefDataWritePos) = 0;	// This function is responsible for incrementing the passed in reference pointer the size of the data written
};

#endif /* __IHyInst2d_h__ */
