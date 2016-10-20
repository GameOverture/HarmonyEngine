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
	bool				m_bIsDirty;
	std::string			m_sString;

	uint32				m_uiCurFontState;

	HyAlign				m_eAlignment;
	HyRectangle<float>	m_rBox;

	// Stored in HyRectangle's (m_rBox) tag field
	enum eBoxAttributes
	{
		BOXATTRIB_IsUsed			= 1 << 0,
		BOXATTRIB_CenterVertically	= 1 << 1,
		BOXATTRIB_ScaleDown			= 1 << 2,
		BOXATTRIB_ScaleUp			= 1 << 3
	};

	unsigned char *		m_pVertexBuffer;
	uint32				m_uiBufferSizeBytes;


public:
	HyText2d(const char *szPrefix, const char *szName);
	virtual ~HyText2d(void);

	// Accepts newline characters "\n"
	void TextSet(std::string sText);
	void TextSet(const char *szString, ...);
	std::string TextGet();

	uint32 TextGetLength();

	uint32 TextGetState();
	void TextSetState(uint32 uiStateIndex);

	HyAlign TextGetAlignment();
	void TextSetAlignment(HyAlign eAlignment);

	HyRectangle<float> TextGetBox();
	void TextSetBox(HyRectangle<float> rBox, bool bCenterVertically = false, bool bScaleDownToFit = false, bool bScaleUpToFit = false);
	void TextClearBox();

private:
	virtual void OnDataLoaded();

	virtual void OnInstUpdate();

	virtual void OnUpdateUniforms(HyShaderUniforms *pShaderUniformsRef);
	virtual void OnWriteDrawBufferData(char *&pRefDataWritePos);
};

#endif /* __HyText2d_h__ */
