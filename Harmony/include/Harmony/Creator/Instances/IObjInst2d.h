/**************************************************************************
 *	IObjInst2d.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __IObjInst2d_h__
#define __IObjInst2d_h__

#include "Creator/HyEntity2d.h"
#include "Creator/Data/HyFactory.h"

#include "Utilities/Animation/HyAnimVec4.h"

class IObjInst2d : public ITransform<HyAnimVec2>
{
	friend class HyCreator;
	static HyCreator *			sm_pCtor;

public:
	enum eRenderState
	{
		RS_SCISSORTEST				= 1 << 0,
		RS_USINGLOCALCOORDS			= 1 << 1,	// If disabled, then using world coordinates

		RS_SHADER_PRIMITIVEDRAW		= 1 << 2,
		RS_SHADER_TEXT				= 1 << 3,
		RS_SHADER_SPINE				= 1 << 4,
		RS_SHADERMASK				= RS_SHADER_PRIMITIVEDRAW | RS_SHADER_TEXT | RS_SHADER_SPINE,

		RS_DRAWMODE_TRIANGLESTRIP	= 1 << 5,
		RS_DRAWMODE_LINELOOP		= 1 << 6,
		RS_DRAWMODE_LINESTRIP		= 1 << 7,
		RS_DRAWMODEMASK				= RS_DRAWMODE_TRIANGLESTRIP | RS_DRAWMODE_LINELOOP | RS_DRAWMODE_LINESTRIP
	};

protected:
	const HyInstanceType		m_keInstType;
	const std::string			m_ksPath;

	// Data loading
	IData *						m_pDataPtr;
	HyLoadState					m_eLoadState;

	// Scene graph hierarchy 
	IObjInst2d *				m_pParent;
	bool						m_bDirty;
	mat4						m_mtxCached;
	vector<IObjInst2d *>		m_vChildList;

	// Attributes
	bool						m_bEnabled;
	HyAnimVec4					m_vColor;
	float						m_fDisplayOrder;	// Higher values are displayed front-most
	uint32						m_uiRenderStates;	// Uses flags from eRenderState
	int32						m_iTag;				// This 'tag' isn't used by the engine, and solely used for whatever purpose the client wishes (tracking, unique ID, etc.)

public:
	IObjInst2d(HyInstanceType eInstType, const char *szPrefix, const char *szName);
	virtual ~IObjInst2d(void);

	HyInstanceType GetInstType()								{ return m_keInstType; }
	const std::string &GetPath()								{ return m_ksPath; }

	HyLoadState GetLoadState()									{ return m_eLoadState; }

	inline bool	IsEnabled()										{ return m_bEnabled; }
	inline void	SetEnabled(bool bVis)							{ m_bEnabled = bVis; }

	float GetDisplayOrder() const								{ return m_fDisplayOrder; }
	void SetDisplayOrder(float fOrderValue);

	inline uint32 GetRenderState() const						{ return m_uiRenderStates; }
	void SetUsingLocalCoordinates(bool bUseLocalCoords)			{ if(bUseLocalCoords) m_uiRenderStates |= RS_USINGLOCALCOORDS; else m_uiRenderStates &= ~RS_USINGLOCALCOORDS; }

	inline HyAnimVec4 &Color()									{ return m_vColor; }

	inline int32 GetTag()										{ return m_iTag; }
	inline void SetTag(int32 iTag)								{ m_iTag = iTag; }

	void Load();
	void Unload();
	void GetWorldTransform(mat4 &outMtx);
	
	void AddChild(IObjInst2d &childInst);
	void Detach();
	
protected:
	virtual void OnDataLoaded() = 0;
	virtual void Update() = 0;

private:
	void SetData(IData *pData);
	void SetLoaded();
	IData *GetData()											{ return m_pDataPtr; }

	void SetDirty();
	static void OnDirty(void *);
};

#endif /* __IObjInst2d_h__ */
