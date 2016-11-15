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

#include "HyGuiWave.h"

#include <QStringListModel>
#include <QStringList>
#include <QStackedWidget>
#include <QStyledItemDelegate>
#include <QDir>

//class WidgetAudioModel : public QStringListModel
//{
//    Q_OBJECT

//    QStringList                     m_sCategoryList;
//    QList<SpriteFrame *>            m_FramesList;
//    QMap<quint32, SpriteFrame *>    m_RemovedFrameMap;  // Used to reinsert frames (via undo/redo) while keeping their attributes


//public:
//    enum eColumn
//    {
//        COLUMN_Frame = 0,
//        COLUMN_OffsetX,
//        COLUMN_OffsetY,
//        COLUMN_Duration,

//        NUMCOLUMNS
//    };

//    WidgetAudioCategoryModel(QObject *pParent);

//    int Add(HyGuiFrame *pFrame);
//    void Remove(HyGuiFrame *pFrame);
//    void MoveRowUp(int iIndex);
//    void MoveRowDown(int iIndex);
//    void RefreshFrame(HyGuiFrame *pFrame);
//    void OffsetFrame(int iIndex, QPoint vOffset);       // iIndex of -1 will apply to all
//    void DurationFrame(int iIndex, float fDuration);    // iIndex of -1 will apply to all

//    SpriteFrame *GetFrameAt(int iIndex);

//    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
//    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
//    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
//    virtual QVariant headerData(int iIndex, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
//    virtual bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
//    virtual Qt::ItemFlags flags(const QModelIndex & index) const;

//signals:
//    void editCompleted(const QString &);
//};

class WidgetAudioBankModel : public QAbstractTableModel
{
    Q_OBJECT
    
    QString                     m_sName;

    QList<HyGuiWave *>          m_WaveList;
    
    enum eColumns
    {
        COLUMN_Name = 0,
        COLUMN_Size,
        COLUMN_Info,
        
        NUM_COLUMNS
    };
    
public:
    WidgetAudioBankModel(QObject *pParent);

    QString GetName();
    void SetName(QString sName);
    
    void AddWave(HyGuiWave *pNewWave);
    HyGuiWave *GetWaveAt(int iIndex);

    void GetJsonObj(QJsonObject &audioBankObj);

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int iIndex, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    virtual bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
    virtual Qt::ItemFlags flags(const QModelIndex & index) const;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class WidgetAudioManagerModel : public QStringListModel
{
    QStackedWidget &        m_AudioBanksRef;

public:
    WidgetAudioManagerModel(QStackedWidget &audioBanksRef, QObject *pParent);

    virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    virtual int	rowCount(const QModelIndex & parent = QModelIndex()) const;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class WidgetAudioCategoryDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    WidgetAudioCategoryDelegate(QObject *pParent = 0);

    virtual QWidget* createEditor(QWidget *pParent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual void setEditorData(QWidget *pEditor, const QModelIndex &index) const;
    virtual void setModelData(QWidget *pEditor, QAbstractItemModel *pModel, const QModelIndex &index) const;
    virtual void updateEditorGeometry(QWidget *pEditor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};


class WidgetAudioCategoryModel : public QStringListModel
{
    QDir             m_AudioBankDir;

public:
    WidgetAudioCategoryModel(QDir audioBankDir, QObject *pParent);

    virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    virtual bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());

    virtual bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    void SaveData();
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // WIDGETAUDIOMODELVIEW_H
