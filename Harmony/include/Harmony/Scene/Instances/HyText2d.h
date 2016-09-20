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

#include "Scene/Instances/IHyInst2d.h"

#include "Assets/Data/HyText2dData.h"

class HyText2d : public IHyInst2d
{
protected:
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

private:
	virtual void OnDataLoaded();

	virtual void OnInstUpdate();

	virtual void OnUpdateUniforms(HyShaderUniforms *pShaderUniformsRef);
	virtual void OnWriteDrawBufferData(char *&pRefDataWritePos);
	
	void CalcVertexBuffer();
};

#endif /* __HyText2d_h__ */
