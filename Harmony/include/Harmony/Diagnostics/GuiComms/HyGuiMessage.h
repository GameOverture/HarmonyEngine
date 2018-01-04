/**************************************************************************
*	HyGuiMessage.h
*
*	Harmony Engine
*	Copyright (c) 2015 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyGuiMessage_h__
#define HyGuiMessage_h__

#include "Afx/HyStdAfx.h"

enum eHyPacketType
{
	HYPACKET_LogNormal = 0,
	HYPACKET_LogWarning,
	HYPACKET_LogError,
	HYPACKET_LogInfo,
	HYPACKET_LogTitle,

	HYPACKET_Int,
	HYPACKET_Float,

	HYPACKET_ReloadStart,
	HYPACKET_ReloadItem,
	HYPACKET_ReloadEnd
};

class HyGuiMessage
{
public:
	enum { HeaderSize = 8 };
	enum { MaxBodySize = 512 };

private:
	unsigned char	m_pData[HeaderSize + MaxBodySize];
	uint32			m_uiBodySize;

public:
	HyGuiMessage();
	HyGuiMessage(eHyPacketType eType, uint32 uiDataSize, const void *pDataToCopy);
	virtual ~HyGuiMessage();

	eHyPacketType			GetType() const;

	const unsigned char *	GetData() const;
	unsigned char *			GetData();

	uint32					GetTotalSize() const;

	const unsigned char *	body() const;
	unsigned char *			body();

	uint32					body_length() const;
	//void					body_length(uint32 new_length);

	bool					decode_header();
	//void					encode_header();
};

#endif /* HyGuiMessage_h__ */
