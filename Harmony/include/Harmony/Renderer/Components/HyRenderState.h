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
class IHyDraw2d;
class HyShaderUniforms;
class IHyShader;

class HyRenderState
{
	// WARNING: This class should not dynamically allocate any memory or use any object that does so

public:
	enum Attributes
	{
		SCISSORTEST				= 1 << 0,
		DRAWINSTANCED			= 1 << 1,	// If enabled, will attempt to batch render multiple instances if they have matching HyRenderStates

		DRAWMODE_TRIANGLES		= 1 << 2,
		DRAWMODE_TRIANGLESTRIP	= 1 << 3,
		DRAWMODE_TRIANGLEFAN	= 1 << 4,
		DRAWMODE_LINELOOP		= 1 << 5,
		DRAWMODE_LINESTRIP		= 1 << 6,
		DRAWMODEMASK			= DRAWMODE_TRIANGLES | DRAWMODE_TRIANGLESTRIP | DRAWMODE_TRIANGLEFAN | DRAWMODE_LINELOOP | DRAWMODE_LINESTRIP,
	};

private:
	uint32				m_uiAttributeFlags;
	uint32				m_uiTextureBindHandle;
	float				m_fLineThickness;

	int32				m_iShaderId;
	uint32				m_uiUniformsCrc32;

	uint32				m_uiNumInstances;
	uint32				m_uiNumVerticesPerInstance;
	size_t				m_uiDataOffset;

	HyScreenRect<int32>	m_ScissorRect;
	int32				m_iWindowIndex;	// -1 Means using world coordinates

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
	const HyScreenRect<int32> &GetScissorRect();
	void SetScissorRect(const HyScreenRect<int32> &rect);
	void SetScissorRect(int32 uiX, int32 uiY, uint32 uiWidth, uint32 uiHeight);
	void ClearScissorRect();

	bool IsUsingCameraCoordinates();
	void SetCoordinateSystem(int32 iWindowIndex);	// -1 Means use world space, otherwise specify a window index to be the local coordinates
	int32 GetAssignedWindow();						// -1 Means using world space and a camera

	void Enable(uint32 uiAttributes);
	void Disable(uint32 uiAttributes);
	bool CompareAttribute(const HyRenderState &rs, uint32 uiMask);
	bool IsEnabled(Attributes eAttrib);
	uint32 GetAttributeBitFlags() const;

	int32 GetShaderId() const;
	void SetShaderId(int32 iId);
	void SetUniformCrc32(uint32 uiCrc32);

	uint32 GetTextureHandle() const;
	void SetTextureHandle(uint32 uiHandleId);

	float GetLineThickness() const;
	void SetLineThickness(float fParam);

	bool operator==(const HyRenderState &right) const;
	bool operator!=(const HyRenderState &right) const;
	bool operator< (const HyRenderState &right) const;
	bool operator> (const HyRenderState &right) const;
	bool operator<=(const HyRenderState &right) const;
	bool operator>=(const HyRenderState &right) const;
};

#endif /* __HyRenderState_h__ */
