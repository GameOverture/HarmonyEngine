/**************************************************************************
 *	HyMeshBuffer.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2025 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyMeshBuffer_h__
#define HyMeshBuffer_h__

#include "Afx/HyStdAfx.h"

class IHyRenderer;

class HyMeshBuffer
{
	IHyRenderer &							m_RendererRef;

	unsigned int vao;
	unsigned int vbo;
	int num_vertices;
	unsigned int ibo;
	int num_indices;

public:
	HyMeshBuffer(IHyRenderer &rendererRef);
	~HyMeshBuffer();
};

#endif /* HyMeshBuffer_h__ */
