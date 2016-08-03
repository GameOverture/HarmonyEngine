/**************************************************************************
 *	ItemSprite.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef ITEMSPRITE_H
#define ITEMSPRITE_H

#include "ItemWidget.h"
#include "WidgetSprite.h"

class WidgetAtlasManager;

struct SpriteFrame
{
    enum eColumn
    {
        COLUMN_Frame = 0,
        COLUMN_Offset,
        COLUMN_Rotation,
        COLUMN_Scale,
        COLUMN_Duration,
        
        NUMCOLUMNS
    };
    
    HyGuiFrame *            m_pFrame;
    int                     m_iRowIndex;
    
    QTableWidgetItem *      m_pTableItems[NUMCOLUMNS];
    
    QPointF                 m_ptOffset;
    float                   m_fRotation;
    QPointF                 m_ptScale;
    float                   m_fDuration;
    
    SpriteFrame(HyGuiFrame *pFrame, int iRowIndex) :    m_pFrame(pFrame),
                                                        m_iRowIndex(iRowIndex),
                                                        m_ptOffset(0.0f, 0.0f),
                                                        m_fRotation(0.0f),
                                                        m_ptScale(1.0f, 1.0f),
                                                        m_fDuration(0.016f)
    {
        m_pTableItems[COLUMN_Frame] = new QTableWidgetItem(m_pFrame->GetName());
        m_pTableItems[COLUMN_Offset] = new QTableWidgetItem(PointToString(m_ptOffset));
        m_pTableItems[COLUMN_Rotation] = new QTableWidgetItem(QString::number(m_fRotation, 'g', 2));
        m_pTableItems[COLUMN_Scale] = new QTableWidgetItem(PointToString(m_ptScale));
        m_pTableItems[COLUMN_Duration] = new QTableWidgetItem(QString::number(m_fDuration, 'g', 2));

        QVariant v;
        v.setValue(m_pFrame);
        m_pTableItems[COLUMN_Frame]->setData(Qt::UserRole, v);
    }
    
    QString PointToString(QPointF ptPoint)
    {
        return QString::number(ptPoint.x(), 'g', 2) % ", " % QString::number(ptPoint.y(), 'g', 2);
    }
};

class ItemSprite : public ItemWidget
{
    Q_OBJECT
    
    HyPrimitive2d               m_primOriginHorz;
    HyPrimitive2d               m_primOriginVert;
    
public:
    ItemSprite(const QString sPath, WidgetAtlasManager &atlasManRef);
    virtual ~ItemSprite();
    
protected:
    virtual void OnDraw_Load(IHyApplication &hyApp);
    virtual void OnDraw_Unload(IHyApplication &hyApp);

    virtual void OnDraw_Show(IHyApplication &hyApp);
    virtual void OnDraw_Hide(IHyApplication &hyApp);
    virtual void OnDraw_Update(IHyApplication &hyApp);

    virtual void OnLink(HyGuiFrame *pFrame);
    virtual void OnUnlink(HyGuiFrame *pFrame);
    
    virtual void OnSave();
};

#endif // ITEMSPRITE_H
