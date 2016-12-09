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

#include "Utilities/Animation/IHyTransform2d.h"
#include "Utilities/Animation/HyAnimVec2.h"

#include "Assets/Data/IHyData.h"

#include "Utilities/Animation/HyAnimVec4.h"
#include "Renderer/HyRenderState.h"
#include "Renderer/Viewport/HyCamera.h"

typedef void (*HyWriteDrawBufferDataOverride)(char *&);
typedef void(*HyUpdateUniformOverride)(HyShaderUniforms *);

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
	int64							m_iTag;				// This 'tag' isn't used by the engine, and solely used for whatever purpose the client wishes (tracking, unique ID, etc.)

public:
	IHyInst2d(HyType eInstType, const char *szPrefix, const char *szName);
	virtual ~IHyInst2d(void);

	const std::string &GetName()								{ return m_sNAME; }
	const std::string &GetPrefix()								{ return m_sPREFIX; }

	bool IsLoaded()												{ return m_eLoadState == HYLOADSTATE_Loaded; }

	HyCoordinateType GetCoordinateType()						{ return m_eCoordType; }
	void SetCoordinateType(HyCoordinateType eCoordType, HyCamera2d *pCameraToCovertFrom);

	int32 GetDisplayOrder() const								{ return m_iDisplayOrder; }
	void SetDisplayOrder(int32 iOrderValue);

	HyAnimVec4													topColor;
	HyAnimVec4													botColor;

	void SetTint(float fR, float fG, float fB)					{ topColor.Set(fR, fG, fB); botColor.Set(fR, fG, fB); }
	void SetTransparency(float fTransparency)					{ topColor.A(fTransparency); botColor.A(fTransparency); }

	int64 GetTag()												{ return m_iTag; }
	void SetTag(int64 iTag)										{ m_iTag = iTag; }

	int32 GetShaderId();
	void SetCustomShader(IHyShader *pShader);

	void Load();
	void Unload();

private:
	HyLoadState GetLoadState()									{ return m_eLoadState; }
	const HyRenderState &GetRenderState() const					{ return m_RenderState; }

	void SetData(IHyData *pData);
	void SetLoaded();
	IHyData *GetData()											{ return m_pData; }

	virtual void OnUpdate();

	virtual void OnInstUpdate() = 0;
	virtual void OnDataLoaded() = 0;
	virtual void OnUpdateUniforms(HyShaderUniforms *pShaderUniformsRef) = 0;	// Upon updating, this function will set the shaders' uniforms when using the default shader
	virtual void OnWriteDrawBufferData(char *&pRefDataWritePos) = 0;			// This function is responsible for incrementing the passed in reference pointer the size of the data written
};

#endif /* __IHyInst2d_h__ */
