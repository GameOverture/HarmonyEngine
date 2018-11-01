/**************************************************************************
 *	AudioModelView.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "AudioModelView.h"
#include "AudioWidgetBank.h"
#include "AudioWidgetManager.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QLineEdit>
#include <QJsonArray>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

AudioBankTableView::AudioBankTableView(QWidget *pParent /*= 0*/) : QTableView(pParent)
{
}

/*virtual*/ void AudioBankTableView::resizeEvent(QResizeEvent *pResizeEvent)
{
	int iWidth = pResizeEvent->size().width();
	
	setColumnWidth(AudioBankTableModel::COLUMN_Name, iWidth / 2);
	setColumnWidth(AudioBankTableModel::COLUMN_Info, iWidth / 2);

	QTableView::resizeEvent(pResizeEvent);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

AudioBankTableModel::AudioBankTableModel(QObject *pParent) :  QAbstractTableModel(pParent),
																m_sName("Unnamed")
{
}

QString AudioBankTableModel::GetName()
{
	return m_sName;
}

void AudioBankTableModel::SetName(QString sName)
{
	m_sName = sName;
}

void AudioBankTableModel::AddWave(AudioWave *pNewWave)
{
	beginInsertRows(QModelIndex(), m_WaveList.count(), m_WaveList.count());
	m_WaveList.append(pNewWave);
	endInsertRows();
}

AudioWave *AudioBankTableModel::GetWaveAt(int iIndex)
{
	return m_WaveList[iIndex];
}

void AudioBankTableModel::GetJsonObj(QJsonObject &audioBankObj)
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

/*virtual*/ int AudioBankTableModel::rowCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
	return m_WaveList.size();
}

/*virtual*/ int AudioBankTableModel::columnCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
	return NUM_COLUMNS;
}

/*virtual*/ QVariant AudioBankTableModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const
{
	AudioWave *pWave = m_WaveList[index.row()];
	
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

/*virtual*/ QVariant AudioBankTableModel::headerData(int iIndex, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
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

/*virtual*/ bool AudioBankTableModel::setData(const QModelIndex & index, const QVariant & value, int role /*= Qt::EditRole*/)
{
	return true;
}

/*virtual*/ Qt::ItemFlags AudioBankTableModel::flags(const QModelIndex & index) const
{
	if(index.column() == COLUMN_Name)
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
	else
		return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

AudioManagerStringListModel::AudioManagerStringListModel(QStackedWidget &atlasGroupsRef, QObject *pParent) :  QStringListModel(pParent),
																								m_AudioBanksRef(atlasGroupsRef)
{ }

/*virtual*/ QVariant AudioManagerStringListModel::data(const QModelIndex & index, int role /*= Qt::DisplayRole*/) const
{
	if(role == Qt::DisplayRole)
	{
		if(m_AudioBanksRef.count() == 0)
			return "";
		else
			return static_cast<AudioWidgetBank *>(m_AudioBanksRef.widget(index.row()))->GetName();
	}
	else
		return QStringListModel::data(index, role);
}

/*virtual*/ int	AudioManagerStringListModel::rowCount(const QModelIndex & parent /*= QModelIndex()*/) const
{
	return m_AudioBanksRef.count();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

AudioCategoryDelegate::AudioCategoryDelegate(QObject *pParent /*= 0*/) :   QStyledItemDelegate(pParent)
{
}

/*virtual*/ QWidget* AudioCategoryDelegate::createEditor(QWidget *pParent, const QStyleOptionViewItem &option, const QModelIndex &index) const
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

	return QStyledItemDelegate::createEditor(pParent, option, index);
}

/*virtual*/ void AudioCategoryDelegate::setEditorData(QWidget *pEditor, const QModelIndex &index) const
{
	if(index.column() == 0)
	{
		static_cast<QLineEdit *>(pEditor)->setText(index.model()->data(index).toString());
	}
	else
		QStyledItemDelegate::setEditorData(pEditor, index);
}

/*virtual*/ void AudioCategoryDelegate::setModelData(QWidget *pEditor, QAbstractItemModel *pModel, const QModelIndex &index) const
{
	if(index.column() == 0)
		pModel->setData(index, static_cast<QLineEdit *>(pEditor)->text());
	else
		QStyledItemDelegate::setModelData(pEditor, pModel, index);
}

/*virtual*/ void AudioCategoryDelegate::updateEditorGeometry(QWidget *pEditor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
}

/////////////////////////////////////////////////

AudioCategoryStringListModel::AudioCategoryStringListModel(QDir audioBankDir, QObject *pParent) :   QStringListModel(pParent),
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

/*virtual*/ QVariant AudioCategoryStringListModel::data(const QModelIndex & index, int role /*= Qt::DisplayRole*/) const
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

/*virtual*/ bool AudioCategoryStringListModel::insertRows(int row, int count, const QModelIndex &parent /*= QModelIndex()*/)
{
	bool bReturnValue = QStringListModel::insertRows(row, count, parent);
	SaveData();

	return bReturnValue;
}

/*virtual*/ bool AudioCategoryStringListModel::removeRows(int row, int count, const QModelIndex &parent /*= QModelIndex()*/)
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

/*virtual*/ bool AudioCategoryStringListModel::setData(const QModelIndex &index, const QVariant &value, int role /*= Qt::EditRole*/)
{
	if(index.row() == 0)
		return false;   // Don't allow changing of "Default"

	bool bReturnValue = QStringListModel::setData(index, value, role);
	SaveData();

	return bReturnValue;
}

void AudioCategoryStringListModel::SaveData()
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

