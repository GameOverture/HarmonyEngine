/**************************************************************************
 *	HyMeshBuffer.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2025 Jason Knobler
 *
 *	MIT License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/main/Engine/LICENSE
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
