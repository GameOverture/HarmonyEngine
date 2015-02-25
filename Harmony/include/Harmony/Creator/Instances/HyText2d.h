/**************************************************************************
 *	HyText2d.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyText2d_h__
#define __HyText2d_h__

#include "Creator/Instances/IObjInst2d.h"

#include "Creator/Data/HyText2dData.h"

class HyText2d : public IObjInst2d
{
	static HyChar		sm_pTempTextBuffer[HY_TEMP_TEXTBUFFER_SIZE];

	HyString			m_sString;
	unsigned char *		m_pVertexBuffer;
	size_t				m_uiBufferSizeBytes;

	uint32				m_uiCurFontIndex;

public:
	HyText2d(const char *szPrefix, const char *szName);
	virtual ~HyText2d(void);

	uint32 GetCurFontIndex()			{ return m_uiCurFontIndex; }

	size_t GetBufferSizeBytes()			{ return m_uiBufferSizeBytes; }
	unsigned char *GetBufferDataPtr()	{ return m_pVertexBuffer; }

	void SetString(const HyChar *szString, ...);
	//HyString &GetString()				{ return m_sString; }
	size_t GetStrLen()					{ return m_sString.length(); }

	uint32 GetTextureId()				{ return (m_pDataPtr == NULL) ? 0 : reinterpret_cast<HyText2dData *>(m_pDataPtr)->GetTexturePtr()->GetId(); }


private:
	virtual void Update();
	virtual void OnDataLoaded();
	virtual void WriteDrawBufferData(char *&pRefDataWritePos);
	
	void CalcVertexBuffer();
};

#endif /* __HyText2d_h__ */
