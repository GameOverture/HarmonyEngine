/**************************************************************************
 *	WidgetSpriteState.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef WIDGETSPRITESTATE_H
#define WIDGETSPRITESTATE_H

#include "HyGuiFrame.h"

#include <QWidget>
#include <QTableWidgetItem>

namespace Ui {
class WidgetSpriteState;
}

class WidgetSpriteState : public QWidget
{
    Q_OBJECT
    
    enum eColumn
    {
        COLUMN_Frame = 0,
        COLUMN_Offset,
        COLUMN_Rotation,
        COLUMN_Scale,
        COLUMN_Duration,
        
        NUMCOLUMNS
    };
    
    struct Frame
    {
        HyGuiFrame *            m_pFrame;
        QTableWidgetItem *      m_pTableItems[NUMCOLUMNS];
        
        QPointF                 m_ptOffset;
        float                   m_fRotation;
        QPointF                 m_ptScale;
        float                   m_fDuration;
        
        Frame(HyGuiFrame *pFrame) : m_pFrame(pFrame),
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

    QString                     m_sName;
    QList<Frame *>              m_pFrameList;

public:
    explicit WidgetSpriteState(QList<QAction *> stateActionList, QWidget *parent = 0);
    ~WidgetSpriteState();

    QString GetName();
    void SetName(QString sNewName);

    void InsertFrame(HyGuiFrame *pFrame);
    void RemoveFrame(HyGuiFrame *pFrame);

    HyGuiFrame *SelectedFrame();
    
private:
    Ui::WidgetSpriteState *ui;
};
Q_DECLARE_METATYPE(WidgetSpriteState *)

#endif // WIDGETSPRITESTATE_H
