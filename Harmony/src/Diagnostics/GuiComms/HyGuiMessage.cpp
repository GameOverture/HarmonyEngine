/**************************************************************************
*	HyGuiMessage.cpp
*
*	Harmony Engine
*	Copyright (c) 2015 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Diagnostics/GuiComms/HyGuiMessage.h"

HyGuiMessage::HyGuiMessage()
{
}

HyGuiMessage::HyGuiMessage(eHyPacketType eType, uint32 uiDataSize, const void *pDataToCopy)
{
	unsigned char *pCurWritePos = &m_pData[0];
	
	*reinterpret_cast<uint32 *>(pCurWritePos) = static_cast<uint32>(eType);
	pCurWritePos += sizeof(uint32);

	*reinterpret_cast<uint32 *>(pCurWritePos) = uiDataSize;
	pCurWritePos += sizeof(uint32);

	memcpy(pCurWritePos, pDataToCopy, uiDataSize);
}

HyGuiMessage::~HyGuiMessage()
{
}

eHyPacketType HyGuiMessage::GetType() const
{
	uint32 uiType = *reinterpret_cast<const uint32 *>(&m_pData[0]);
	return static_cast<eHyPacketType>(uiType);
}

const unsigned char *HyGuiMessage::GetData() const
{
	return m_pData;
}

unsigned char* HyGuiMessage::GetData()
{
	return m_pData;
}

uint32 HyGuiMessage::GetTotalSize() const
{
	return HeaderSize + m_uiBodySize;
}

const unsigned char* HyGuiMessage::body() const
{
	return m_pData + HeaderSize;
}

unsigned char* HyGuiMessage::body()
{
	return m_pData + HeaderSize;
}

uint32 HyGuiMessage::body_length() const
{
	return m_uiBodySize;
}

//void HyGuiMessage::body_length(uint32 new_length)
//{
//	m_uiBodySize = new_length;
//	if(m_uiBodySize > MaxBodySize)
//		m_uiBodySize = MaxBodySize;
//}

bool HyGuiMessage::decode_header()
{
	m_uiBodySize = *reinterpret_cast<uint32 *>(&m_pData[4]);

	if(m_uiBodySize > MaxBodySize)
	{
		m_uiBodySize = 0;
		return false;
	}
	return true;
}

//void HyGuiMessage::encode_header()
//{
//	char header[HeaderSize + 1] = "";
//	std::sprintf(header, "%4d", static_cast<int>(m_uiBodySize));
//	std::memcpy(m_pData, header, HeaderSize);
//}

