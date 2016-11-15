/**************************************************************************
 *	WidgetAudioModelView.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "WidgetAudioModelView.h"
#include "WidgetAudioBank.h"
#include "WidgetAudioManager.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QLineEdit>
#include <QJsonArray>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WidgetAudioBankTableView::WidgetAudioBankTableView(QWidget *pParent /*= 0*/) : QTableView(pParent)
{
}

/*virtual*/ void WidgetAudioBankTableView::resizeEvent(QResizeEvent *pResizeEvent)
{
    int iWidth = pResizeEvent->size().width();
    
    setColumnWidth(WidgetAudioBankModel::COLUMN_Name, iWidth / 2);
    setColumnWidth(WidgetAudioBankModel::COLUMN_Info, iWidth / 2);

    QTableView::resizeEvent(pResizeEvent);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WidgetAudioBankModel::WidgetAudioBankModel(QObject *pParent) :  QAbstractTableModel(pParent),
                                                                m_sName("Unnamed")
{
}

QString WidgetAudioBankModel::GetName()
{
    return m_sName;
}

void WidgetAudioBankModel::SetName(QString sName)
{
    m_sName = sName;
}

void WidgetAudioBankModel::AddWave(HyGuiWave *pNewWave)
{
    beginInsertRows(QModelIndex(), m_WaveList.count(), m_WaveList.count());
    m_WaveList.append(pNewWave);
    endInsertRows();
}

HyGuiWave *WidgetAudioBankModel::GetWaveAt(int iIndex)
{
    return m_WaveList[iIndex];
}

void WidgetAudioBankModel::GetJsonObj(QJsonObject &audioBankObj)
{
    audioBankObj.insert("name", QJsonValue(m_sName));

    QJsonArray waveArray;
    for(int i = 0; i < m_WaveList.size(); ++i)
    {
        QJsonObject waveObj;
        m_WaveList[i]->GetJsonObj(waveObj);
        waveArray.append(waveObj);
    }
    audioBankObj.insert("waves", waveArray);
}

/*virtual*/ int WidgetAudioBankModel::rowCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
    return m_WaveList.size();
}

/*virtual*/ int WidgetAudioBankModel::columnCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
    return NUM_COLUMNS;
}

/*virtual*/ QVariant WidgetAudioBankModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const
{
    HyGuiWave *pWave = m_WaveList[index.row()];
    
    if(role == Qt::DecorationRole && index.column() == COLUMN_Name)
        return pWave->GetIcon();

    if(role == Qt::ToolTipRole)
    {
        return pWave->GetSizeDescription();
    }
    if (role == Qt::TextAlignmentRole && index.column() != COLUMN_Name)
    {
        return Qt::AlignCenter;
    }
    
    if(role == Qt::DisplayRole || role == Qt::EditRole)
    {
        switch(index.column())
        {
        case COLUMN_Name:
            return pWave->GetName();
        case COLUMN_Info:
            return pWave->GetDescription();
        }
    }

    return QVariant();
}

/*virtual*/ QVariant WidgetAudioBankModel::headerData(int iIndex, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal)
        {
            switch(iIndex)
            {
            case COLUMN_Name:
                return QString("Name");
            case COLUMN_Info:
                return "Info";
            }
        }
    }

    return QVariant();
}

/*virtual*/ bool WidgetAudioBankModel::setData(const QModelIndex & index, const QVariant & value, int role /*= Qt::EditRole*/)
{
    return true;
}

/*virtual*/ Qt::ItemFlags WidgetAudioBankModel::flags(const QModelIndex & index) const
{
    if(index.column() == COLUMN_Name)
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    else
        return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WidgetAudioManagerModel::WidgetAudioManagerModel(QStackedWidget &atlasGroupsRef, QObject *pParent) :  QStringListModel(pParent),
                                                                                                m_AudioBanksRef(atlasGroupsRef)
{ }

/*virtual*/ QVariant WidgetAudioManagerModel::data(const QModelIndex & index, int role /*= Qt::DisplayRole*/) const
{
    if(role == Qt::DisplayRole)
    {
        if(m_AudioBanksRef.count() == 0)
            return "";
        else
            return static_cast<WidgetAudioBank *>(m_AudioBanksRef.widget(index.row()))->GetName();
    }
    else
        return QStringListModel::data(index, role);
}

/*virtual*/ int	WidgetAudioManagerModel::rowCount(const QModelIndex & parent /*= QModelIndex()*/) const
{
    return m_AudioBanksRef.count();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WidgetAudioCategoryDelegate::WidgetAudioCategoryDelegate(QObject *pParent /*= 0*/) :   QStyledItemDelegate(pParent)
{
}

/*virtual*/ QWidget* WidgetAudioCategoryDelegate::createEditor(QWidget *pParent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.column() == 0)
    {
        QLineEdit *pLineEdit = new QLineEdit(pParent);
        pLineEdit->setValidator(HyGlobal::FileNameValidator());

        if(index.row() == 0)
        {
            HyGuiLog("You cannot edit the \"Default\" category", LOGTYPE_Warning);
            return NULL;//pLineEdit->setReadOnly(true); // Don't allow changing of "Default"
        }

        return pLineEdit;
    }

    QStyledItemDelegate::createEditor(pParent, option, index);
}

/*virtual*/ void WidgetAudioCategoryDelegate::setEditorData(QWidget *pEditor, const QModelIndex &index) const
{
    if(index.column() == 0)
    {
        static_cast<QLineEdit *>(pEditor)->setText(index.model()->data(index).toString());
    }
    else
        QStyledItemDelegate::setEditorData(pEditor, index);
}

/*virtual*/ void WidgetAudioCategoryDelegate::setModelData(QWidget *pEditor, QAbstractItemModel *pModel, const QModelIndex &index) const
{
    if(index.column() == 0)
        pModel->setData(index, static_cast<QLineEdit *>(pEditor)->text());
    else
        QStyledItemDelegate::setModelData(pEditor, pModel, index);
}

/*virtual*/ void WidgetAudioCategoryDelegate::updateEditorGeometry(QWidget *pEditor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
}

/////////////////////////////////////////////////

WidgetAudioCategoryModel::WidgetAudioCategoryModel(QDir audioBankDir, QObject *pParent) :   QStringListModel(pParent),
                                                                                            m_AudioBankDir(audioBankDir)
{
    QJsonArray categoryArray;
    bool bNoFileFound = true;

    QFile audioCategoryFile(m_AudioBankDir.absoluteFilePath(HYGUIPATH_DataAudioCategories));
    if(audioCategoryFile.exists())
    {
        if(!audioCategoryFile.open(QIODevice::ReadOnly))
        {
            HyGuiLog(QString("WidgetAtlasGroup::WidgetAtlasGroup() could not open ") % HYGUIPATH_MetaSettings, LOGTYPE_Error);
        }
        else
        {
            bNoFileFound = false;

            QJsonDocument audioCategoryDoc = QJsonDocument::fromJson(audioCategoryFile.readAll());
            audioCategoryFile.close();

            categoryArray = audioCategoryDoc.array();
        }
    }
    else
    {
        categoryArray.append("Default");
        categoryArray.append("Music");
    }


    QStringList sCategoryList;
    for(int i = 0; i < categoryArray.count(); ++i)
        sCategoryList.append(categoryArray.at(i).toString());

    setStringList(sCategoryList);

    if(bNoFileFound)
        SaveData();
}

/*virtual*/ QVariant WidgetAudioCategoryModel::data(const QModelIndex & index, int role /*= Qt::DisplayRole*/) const
{
    if(role == Qt::DisplayRole && index.column() == 1)
    {
        if(stringList().count() == 0)
            return "";
        else
            return stringList().at(index.row());
    }
    else
        return QStringListModel::data(index, role);
}

/*virtual*/ bool WidgetAudioCategoryModel::insertRows(int row, int count, const QModelIndex &parent /*= QModelIndex()*/)
{
    bool bReturnValue = QStringListModel::insertRows(row, count, parent);
    SaveData();

    return bReturnValue;
}

/*virtual*/ bool WidgetAudioCategoryModel::removeRows(int row, int count, const QModelIndex &parent /*= QModelIndex()*/)
{
    bool bReturnValue = false;
    if(row == 0)
    {
        // Don't allow deleting of "Default"
        if(count > 1)
            bReturnValue = QStringListModel::removeRows(row+1, count-1, parent);
        else
            bReturnValue = false;
    }
    else
        bReturnValue = QStringListModel::removeRows(row, count, parent);

    SaveData();
    return bReturnValue;
}

/*virtual*/ bool WidgetAudioCategoryModel::setData(const QModelIndex &index, const QVariant &value, int role /*= Qt::EditRole*/)
{
    if(index.row() == 0)
        return false;   // Don't allow changing of "Default"

    bool bReturnValue = QStringListModel::setData(index, value, role);
    SaveData();

    return bReturnValue;
}

void WidgetAudioCategoryModel::SaveData()
{
    QJsonDocument audioCategoryDoc;

    QJsonArray categoryArray;
    QStringList sCategoryList = stringList();
    for(int i = 0; i < sCategoryList.size(); ++i)
        categoryArray.append(sCategoryList[i]);

    audioCategoryDoc.setArray(categoryArray);

    QFile audioCategoryFile(m_AudioBankDir.absolutePath() % "/" % HYGUIPATH_DataAudioCategories);
    if(audioCategoryFile.open(QIODevice::WriteOnly | QIODevice::Truncate) == false)
    {
       HyGuiLog("Couldn't open audio category file for writing", LOGTYPE_Error);
    }
    else
    {
        qint64 iBytesWritten = audioCategoryFile.write(audioCategoryDoc.toJson());
        if(0 == iBytesWritten || -1 == iBytesWritten)
        {
            HyGuiLog("Could not write to audio category file: " % audioCategoryFile.errorString(), LOGTYPE_Error);
        }

        audioCategoryFile.close();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

