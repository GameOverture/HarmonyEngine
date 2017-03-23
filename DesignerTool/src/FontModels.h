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
#include "IModel.h"

class FontStateData : public IStateData
{
    CheckBoxMapper *    m_pChkMapper_Loop;
    CheckBoxMapper *    m_pChkMapper_Reverse;
    CheckBoxMapper *    m_pChkMapper_Bounce;
    SpriteFramesModel * m_pFramesModel;

public:
    SpriteStateData(IModel &modelRef, QJsonObject stateObj);
    virtual ~SpriteStateData();
    
    CheckBoxMapper *GetLoopMapper();
    CheckBoxMapper *GetReverseMapper();
    CheckBoxMapper *GetBounceMapper();
    SpriteFramesModel *GetFramesModel();

    void GetStateInfo(QJsonObject &stateObjOut);
    
    virtual void AddFrame(AtlasFrame *pFrame);
    virtual void RelinquishFrame(AtlasFrame *pFrame);
    virtual void RefreshFrame(AtlasFrame *pFrame);
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

public:
    FontModel(ProjectItem *pItem, QJsonObject fontObj);
    virtual ~FontModel();
    
    virtual int AppendState(QJsonObject stateObj);
    virtual void InsertState(int iStateIndex, QJsonObject stateObj);
    virtual QJsonObject PopStateAt(uint32 uiIndex);
    
    virtual QJsonValue GetSaveInfo();
};

#endif // FONTMODELS_H
