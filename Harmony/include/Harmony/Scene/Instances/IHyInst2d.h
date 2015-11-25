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

#include "Utilities/Animation/ITransform.h"
#include "Utilities/Animation/HyAnimVec2.h"

//#include "Scene/HyEntity2d.h"
#include "Assets/Data/IHyData.h"

#include "Utilities/Animation/HyAnimVec4.h"
#include "Renderer/HyRenderState.h"

class IHyInst2d : public ITransform<HyAnimVec2>
{
	friend class HyScene;

	friend class HyAssetManager;
	static HyAssetManager *			sm_pFileIO;

protected:
	const HyInstanceType		m_eTYPE;
	const std::string			m_sNAME;
	const std::string			m_sPREFIX;

	// Data loading
	IHyData *					m_pData;
	HyLoadState					m_eLoadState;

	// Scene graph hierarchy 
	IHyInst2d *					m_pParent;
	bool						m_bDirty;
	mat4						m_mtxCached;
	vector<IHyInst2d *>			m_vChildList;

	// Attributes
	bool						m_bEnabled;
	HyAnimVec4					m_vColor;
	uint32						m_uiDisplayOrder;	// Lower values are displayed front-most
	HyRenderState				m_RenderState;
	int32						m_iTag;				// This 'tag' isn't used by the engine, and solely used for whatever purpose the client wishes (tracking, unique ID, etc.)

public:
	IHyInst2d(HyInstanceType eInstType, const char *szPrefix, const char *szName);
	virtual ~IHyInst2d(void);

	HyInstanceType GetInstType()								{ return m_eTYPE; }
	const std::string &GetName()								{ return m_sNAME; }
	const std::string &GetPrefix()								{ return m_sPREFIX; }

	HyLoadState GetLoadState()									{ return m_eLoadState; }

	inline bool	IsEnabled()										{ return m_bEnabled; }
	inline void	SetEnabled(bool bVis)							{ m_bEnabled = bVis; }

	uint32 GetDisplayOrder() const								{ return m_uiDisplayOrder; }
	void SetDisplayOrder(uint32 uiOrderValue);

	const HyRenderState &GetRenderState() const					{ return m_RenderState; }
	void SetUsingLocalCoordinates(bool bUseLocalCoords)			{ bUseLocalCoords ? m_RenderState.Enable(HyRenderState::USINGLOCALCOORDS) : m_RenderState.Disable(HyRenderState::USINGLOCALCOORDS); }

	inline HyAnimVec4 &Color()									{ return m_vColor; }

	inline int32 GetTag()										{ return m_iTag; }
	inline void SetTag(int32 iTag)								{ m_iTag = iTag; }

	void Load();
	void Unload();
	void GetWorldTransform(mat4 &outMtx);
	void GetRenderState(HyRenderState &renderStateOut) const;
	
	void AddChild(IHyInst2d &childInst);
	void Detach();
	
protected:
	virtual void OnDataLoaded() = 0;
	virtual void Update() = 0;

	// This function is responsible for incrementing the passed in reference pointer the size of the data written
	virtual void WriteDrawBufferData(char *&pRefDataWritePos) = 0;

private:
	void SetData(IHyData *pData);
	void SetLoaded();
	IHyData *GetData()											{ return m_pData; }

	void SetDirty();
	static void OnDirty(void *);
};

#endif /* __IHyInst2d_h__ */
