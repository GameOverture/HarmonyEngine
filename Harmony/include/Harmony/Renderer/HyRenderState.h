/**************************************************************************
 *	HyRenderState.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyRenderState_h__
#define __HyRenderState_h__

#include "Afx/HyStdAfx.h"

#include "Utilities/HyMath.h"

// Forward declaration
class IHyInst2d;
class HyShaderUniforms;
class IHyShader;

class HyRenderState
{
	// WARNING: This class should not dynamically allocate any memory or use any object that does so

public:
	enum eAttributes
	{
		SCISSORTEST				= 1 << 0,
		USINGSCREENCOORDS		= 1 << 1,	// If disabled, then using world coordinates

		DRAWINSTANCED			= 1 << 2,	// If enabled, will attempt to batch render multiple instances if they have matching HyRenderStates

		DRAWMODE_TRIANGLESTRIP	= 1 << 3,
		DRAWMODE_TRIANGLEFAN	= 1 << 4,
		DRAWMODE_LINELOOP		= 1 << 5,
		DRAWMODE_LINESTRIP		= 1 << 6,
		DRAWMODEMASK			= DRAWMODE_TRIANGLESTRIP | DRAWMODE_TRIANGLEFAN | DRAWMODE_LINELOOP | DRAWMODE_LINESTRIP,
	};

private:
	uint32				m_uiAttributeFlags;
	uint32				m_uiTextureBindHandle;

	int32				m_iShaderId;
	uint32				m_uiUniformsCrc32;

	uint32				m_uiNumInstances;
	uint32				m_uiNumVerticesPerInstance;
	size_t				m_uiDataOffset;

	HyRectangle<uint32>	m_ScissorRect;

public:
	HyRenderState();
	~HyRenderState(void);

	void SetDataOffset(size_t uiVertexDataOffset);
	size_t GetDataOffset() const;
	
	void AppendInstances(uint32 uiNumInstsToAppend);
	uint32 GetNumInstances() const;
	void SetNumInstances(uint32 uiNumInsts);

	uint32 GetNumVerticesPerInstance();
	void SetNumVerticesPerInstance(uint32 uiNumVerts);

	bool IsScissorRect();
	const HyRectangle<uint32> &GetScissorRect();
	void SetScissorRect(uint32 uiX, uint32 uiY, uint32 uiWidth, uint32 uiHeight);
	void ClearScissorRect();

	void Enable(uint32 uiAttributes);
	void Disable(uint32 uiAttributes);
	bool CompareAttribute(const HyRenderState &rs, uint32 uiMask);
	bool IsEnabled(eAttributes eAttrib);
	uint32 GetAttributeBitFlags() const;

	int32 GetShaderId();
	void SetShaderId(int32 iId);
	void SetUniformCrc32(uint32 uiCrc32);

	uint32 GetTextureHandle() const;
	void SetTextureHandle(uint32 uiHandleId);

	bool operator==(const HyRenderState &right) const;
	bool operator!=(const HyRenderState &right) const;
	bool operator< (const HyRenderState &right) const;
	bool operator> (const HyRenderState &right) const;
	bool operator<=(const HyRenderState &right) const;
	bool operator>=(const HyRenderState &right) const;
};

#endif /* __HyRenderState_h__ */
