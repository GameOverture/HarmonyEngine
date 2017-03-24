/**************************************************************************
 *	FontModels.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef FONTMODELS_H
#define FONTMODELS_H

#include "ProjectItem.h"
#include "FontModelView.h"
#include "IModel.h"

#include <QJsonArray>

class FontStateData : public IStateData
{
	FontTableModel *			m_pFontTableModel;
	
	LineEditMapper *            m_pTxtMapper_Font;
	DoubleSpinBoxMapper *		m_pSbMapper_Size;
	
public:
    FontStateData(IModel &modelRef, QJsonObject stateObj);
    virtual ~FontStateData();

    void GetStateInfo(QJsonObject &stateObjOut);
    
	virtual void AddFrame(AtlasFrame *pFrame) { }
	virtual void RelinquishFrame(AtlasFrame *pFrame) { }
	virtual void RefreshFrame(AtlasFrame *pFrame) { }
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class FontModel : public IModel
{
    Q_OBJECT
    
    CheckBoxMapper *            m_pChkMapper_09;
    CheckBoxMapper *            m_pChkMapper_AZ;
    CheckBoxMapper *            m_pChkMapper_az;
    CheckBoxMapper *            m_pChkMapper_Symbols;
    LineEditMapper *            m_pTxtMapper_AdditionalSymbols;
	
	AtlasFrame *                m_pTrueAtlasFrame;
	
	QJsonArray					m_TypefaceArray;

public:
    FontModel(ProjectItem *pItem, QJsonObject fontObj);
    virtual ~FontModel();
	
	QJsonObject GetTypefaceObj(int iTypefaceIndex);
    
    virtual QJsonObject PopStateAt(uint32 uiIndex);
    virtual QJsonValue GetSaveInfo();
};

#endif // FONTMODELS_H
