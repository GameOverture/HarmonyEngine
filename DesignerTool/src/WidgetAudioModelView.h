/**************************************************************************
 *	WidgetAudioModelView.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef WIDGETAUDIOMODELVIEW_H
#define WIDGETAUDIOMODELVIEW_H

#include <QStringListModel>
#include <QStringList>

class WidgetAudioCategoryModel : public QStringListModel
{
    Q_OBJECT

    QStringList                     m_sCategoryList;
    QList<SpriteFrame *>            m_FramesList;
    QMap<quint32, SpriteFrame *>    m_RemovedFrameMap;  // Used to reinsert frames (via undo/redo) while keeping their attributes


public:
    enum eColumn
    {
        COLUMN_Frame = 0,
        COLUMN_OffsetX,
        COLUMN_OffsetY,
        COLUMN_Duration,

        NUMCOLUMNS
    };

    WidgetAudioCategoryModel(QObject *pParent);

    int Add(HyGuiFrame *pFrame);
    void Remove(HyGuiFrame *pFrame);
    void MoveRowUp(int iIndex);
    void MoveRowDown(int iIndex);
    void RefreshFrame(HyGuiFrame *pFrame);
    void OffsetFrame(int iIndex, QPoint vOffset);       // iIndex of -1 will apply to all
    void DurationFrame(int iIndex, float fDuration);    // iIndex of -1 will apply to all

    SpriteFrame *GetFrameAt(int iIndex);

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int iIndex, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    virtual bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
    virtual Qt::ItemFlags flags(const QModelIndex & index) const;

signals:
    void editCompleted(const QString &);
};

#endif // WIDGETAUDIOMODELVIEW_H
