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

//#include "Creator/HyEntity2d.h"
#include "FileIO/Data/IHyData.h"

#include "Utilities/Animation/HyAnimVec4.h"
#include "Renderer/HyRenderState.h"

class IHyInst2d : public ITransform<HyAnimVec2>
{
	friend class HyCreator;

	friend class IHyFileIO;
	static IHyFileIO *			sm_pCtor;

protected:
	const HyInstanceType		m_keInstType;
	const std::string			m_ksPath;
	
	HyRenderState				m_RenderState;

	// Data loading
	IHyData *					m_pDataPtr;
	HyLoadState					m_eLoadState;

	// Scene graph hierarchy 
	IHyInst2d *					m_pParent;
	bool						m_bDirty;
	mat4						m_mtxCached;
	vector<IHyInst2d *>			m_vChildList;

	// Attributes
	bool						m_bEnabled;
	HyAnimVec4					m_vColor;
	float						m_fDisplayOrder;	// Higher values are displayed front-most
	int32						m_iTag;				// This 'tag' isn't used by the engine, and solely used for whatever purpose the client wishes (tracking, unique ID, etc.)

public:
	IHyInst2d(HyInstanceType eInstType, const char *szPrefix, const char *szName);
	IHyInst2d(HyInstanceType eInstType, uint32 uiTextureIndex);
	virtual ~IHyInst2d(void);

	void CtorInit();

	HyInstanceType GetInstType()								{ return m_keInstType; }
	const std::string &GetPath()								{ return m_ksPath; }

	HyLoadState GetLoadState()									{ return m_eLoadState; }

	inline bool	IsEnabled()										{ return m_bEnabled; }
	inline void	SetEnabled(bool bVis)							{ m_bEnabled = bVis; }

	float GetDisplayOrder() const								{ return m_fDisplayOrder; }
	void SetDisplayOrder(float fOrderValue);

	const HyRenderState &GetRenderState() const					{ return m_RenderState; }
	void SetUsingLocalCoordinates(bool bUseLocalCoords)			{ if(bUseLocalCoords) m_RenderState.Enable(HyRenderState::USINGLOCALCOORDS); else m_RenderState.Disable(HyRenderState::USINGLOCALCOORDS); }

	inline HyAnimVec4 &Color()									{ return m_vColor; }

	inline int32 GetTag()										{ return m_iTag; }
	inline void SetTag(int32 iTag)								{ m_iTag = iTag; }

	void Load();
	void Unload();
	void GetWorldTransform(mat4 &outMtx);
	
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
	IHyData *GetData()											{ return m_pDataPtr; }

	void SetDirty();
	static void OnDirty(void *);
};

#endif /* __IHyInst2d_h__ */
