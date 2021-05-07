/**************************************************************************
*	TextLayersModel.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2019 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "TextLayersModel.h"
#include "Project.h"
#include "ExplorerModel.h"

TextLayersModel::TextLayersModel(TextFontManager &fontManagerRef, const QList<TextLayerHandle> &layerList) :
	QAbstractTableModel(nullptr),
	m_FontManagerRef(fontManagerRef),
	m_LayerList(layerList)
{
}

/*virtual*/ TextLayersModel::~TextLayersModel()
{
}

bool TextLayersModel::IsEmpty() const
{
	return m_LayerList.empty();
}

bool TextLayersModel::HasHandle(TextLayerHandle hHandle) const
{
	for(int i = 0; i < m_LayerList.size(); ++i)
	{
		if(hHandle == m_LayerList[i])
			return true;
	}

	return false;
}

TextFontManager &TextLayersModel::GetFontManager()
{
	return m_FontManagerRef;
}

const TextFontManager &TextLayersModel::GetFontManager() const
{
	return m_FontManagerRef;
}

QJsonArray TextLayersModel::GetLayersArray() const
{
	QJsonArray layersArray;
	for(int i = 0; i < m_LayerList.size(); ++i)
	{
		QJsonObject layerObj;

		QColor topColor, botColor;
		m_FontManagerRef.GetColor(m_LayerList[i], topColor, botColor);

		layerObj.insert("typefaceIndex", static_cast<int>(m_FontManagerRef.GetFontIndex(m_LayerList[i])));
		layerObj.insert("botR", botColor.redF());
		layerObj.insert("botG", botColor.greenF());
		layerObj.insert("botB", botColor.blueF());
		layerObj.insert("topR", topColor.redF());
		layerObj.insert("topG", topColor.greenF());
		layerObj.insert("topB", topColor.blueF());

		layersArray.append(layerObj);
	}

	return layersArray;
}

void TextLayersModel::GetMiscInfo(float &fLeftSideNudgeAmtOut, float &fLineAscenderOut, float &fLineDescenderOut, float &fLineHeightOut) const
{
	fLineHeightOut = 0.0f;
	for(int i = 0; i < m_LayerList.count(); ++i)
	{
		if(fLineHeightOut < m_FontManagerRef.GetLineHeight(m_LayerList[i]))
			fLineHeightOut = m_FontManagerRef.GetLineHeight(m_LayerList[i]);
	}

	fLineAscenderOut = 0.0f;
	for(int i = 0; i < m_LayerList.count(); ++i)
	{
		if(fLineAscenderOut < m_FontManagerRef.GetLineAscender(m_LayerList[i]))
			fLineAscenderOut = m_FontManagerRef.GetLineAscender(m_LayerList[i]);
	}

	fLineDescenderOut = 0.0f;
	for(int i = 0; i < m_LayerList.count(); ++i)
	{
		if(fLineDescenderOut > m_FontManagerRef.GetLineDescender(m_LayerList[i]))
			fLineDescenderOut = m_FontManagerRef.GetLineDescender(m_LayerList[i]);
	}

	fLeftSideNudgeAmtOut = 0.0f;
	for(int i = 0; i < m_LayerList.count(); ++i)
	{
		if(fLeftSideNudgeAmtOut < abs(m_FontManagerRef.GetLeftSideNudgeAmt(m_LayerList[i])))
			fLeftSideNudgeAmtOut = abs(m_FontManagerRef.GetLeftSideNudgeAmt(m_LayerList[i]));
	}
}

TextLayerHandle TextLayersModel::AddNewLayer(QString sFontName, rendermode_t eRenderMode, float fOutlineThickness, float fSize)
{
	TextLayerHandle hNewLayer = m_FontManagerRef.AddNewLayer(sFontName, eRenderMode, fOutlineThickness, fSize);
	if(hNewLayer == HY_UNUSED_HANDLE)
	{
		HyGuiLog("TextFontManager::AddNewLayer failed to return valid handle", LOGTYPE_Error);
		return HY_UNUSED_HANDLE;
	}

	int iRowIndex = m_LayerList.count();
	beginInsertRows(QModelIndex(), iRowIndex, iRowIndex);
	m_LayerList.append(hNewLayer);
	endInsertRows();

	return hNewLayer;
}

void TextLayersModel::RemoveLayer(TextLayerHandle hHandle)
{
	for(int i = 0; i < m_LayerList.count(); ++i)
	{
		if(m_LayerList[i] == hHandle)
		{
			beginRemoveRows(QModelIndex(), i, i);
			m_RemovedLayerList.append(QPair<int, TextLayerHandle>(i, m_LayerList[i]));
			m_LayerList.removeAt(i);
			endRemoveRows();
		}
	}

	m_FontManagerRef.RegenAtlas();
}

void TextLayersModel::ReAddLayer(TextLayerHandle hHandle)
{
	for(int i = 0; i < m_RemovedLayerList.count(); ++i)
	{
		if(m_RemovedLayerList[i].second == hHandle)
		{
			beginInsertRows(QModelIndex(), m_RemovedLayerList[i].first, m_RemovedLayerList[i].first);
			m_LayerList.insert(m_RemovedLayerList[i].first, m_RemovedLayerList[i].second);
			m_RemovedLayerList.removeAt(i);
			endInsertRows();
		}
	}

	m_FontManagerRef.RegenAtlas();
}

void TextLayersModel::MoveRowUp(int iIndex)
{
	if(beginMoveRows(QModelIndex(), iIndex, iIndex, QModelIndex(), iIndex - 1) == false)
		return;

	m_LayerList.swapItemsAt(iIndex, iIndex - 1);
	endMoveRows();
}

void TextLayersModel::MoveRowDown(int iIndex)
{
	if(beginMoveRows(QModelIndex(), iIndex, iIndex, QModelIndex(), iIndex + 2) == false)
		return;

	m_LayerList.swapItemsAt(iIndex, iIndex + 1);
	endMoveRows();
}

QString TextLayersModel::GetFont() const
{
	if(m_LayerList.empty())
		return QString();

	return m_FontManagerRef.GetFontName(m_LayerList[0]);
}

QString TextLayersModel::GetFontPath() const
{
	if(m_LayerList.empty())
		return QString();

	return m_FontManagerRef.GetFontPath(m_LayerList[0]);
}

void TextLayersModel::SetFont(QString sFontName)
{
	for(int i = 0; i < m_LayerList.size(); ++i)
		m_FontManagerRef.SetFont(m_LayerList[i], sFontName);
}

float TextLayersModel::GetFontSize() const
{
	if(m_LayerList.empty())
		return 0.0f;

	return m_FontManagerRef.GetSize(m_LayerList[0]);
}

void TextLayersModel::SetFontSize(float fSize)
{
	for(int i = 0; i < m_LayerList.size(); ++i)
		m_FontManagerRef.SetFontSize(m_LayerList[i], fSize);
}

QModelIndex TextLayersModel::GetIndex(TextLayerHandle hLayer, Column eCol) const
{
	for(int i = 0; i < m_LayerList.size(); ++i)
	{
		if(m_LayerList[i] == hLayer)
			return createIndex(i, eCol);
	}

	return QModelIndex();
}

TextLayerHandle TextLayersModel::GetHandle(const QModelIndex &indexRef) const
{
	if(indexRef.isValid())
		return m_LayerList[indexRef.row()];

	return HY_UNUSED_HANDLE;
}

/*virtual*/ int TextLayersModel::rowCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
	return m_LayerList.count();
}

/*virtual*/ int TextLayersModel::columnCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
	return NUMCOLUMNS;
}

/*virtual*/ QVariant TextLayersModel::data(const QModelIndex &indexRef, int iRole /*= Qt::DisplayRole*/) const
{
	TextLayerHandle hLayer = m_LayerList[indexRef.row()];

	if(iRole == Qt::TextAlignmentRole && indexRef.column() != COLUMN_Mode)
		return Qt::AlignCenter;

	if(indexRef.column() == COLUMN_Color)
	{
		QColor topColor, botColor;
		m_FontManagerRef.GetColor(hLayer, topColor, botColor);

		if(iRole == Qt::BackgroundRole)
		{
			QLinearGradient gradient(0, 0, 0, 25.0f);
			gradient.setColorAt(0.0, topColor);
			gradient.setColorAt(1.0, botColor);

			QBrush bgColorBrush(gradient);
			return QVariant(bgColorBrush);
		}
		else if(iRole == Qt::ForegroundRole)
		{
			// Counting the perceptive luminance - human eye favors green color
			double a = 1 - ( 0.299 * topColor.redF() + 0.587 * topColor.greenF() + 0.114 * topColor.blueF())/255;

			if (a < 0.5)
			{
				QBrush bgColorBrush(Qt::black);
				return QVariant(bgColorBrush);
			}
			else
			{
				QBrush bgColorBrush(Qt::white);
				return QVariant(bgColorBrush);
			}
		}
	}

	if(iRole == Qt::DisplayRole)
	{
		switch(indexRef.column())
		{
		case COLUMN_Mode:
			switch(m_FontManagerRef.GetRenderMode(hLayer))
			{
			case RENDER_NORMAL:
				return "Fill";
			case RENDER_OUTLINE_EDGE:
				return "Outline Edge";
			case RENDER_OUTLINE_POSITIVE:
				return "Outline Edge+Fill";
			case RENDER_OUTLINE_NEGATIVE:
				return "Inner";
			case RENDER_SIGNED_DISTANCE_FIELD:
				return "Signed Dist Field";
			}
			return "Unknown";

		case COLUMN_Thickness:
			switch(m_FontManagerRef.GetRenderMode(hLayer))
			{
			case RENDER_NORMAL:
			case RENDER_SIGNED_DISTANCE_FIELD:
				return "N/A";
			case RENDER_OUTLINE_EDGE:
			case RENDER_OUTLINE_POSITIVE:
			case RENDER_OUTLINE_NEGATIVE:
				return QString::number(m_FontManagerRef.GetOutlineThickness(hLayer), 'g', 2);
			}

		case COLUMN_Color:
			return "";
		}
	}
	else if(iRole == Qt::EditRole)
	{
		switch(indexRef.column())
		{
		case COLUMN_Mode:
			return m_FontManagerRef.GetRenderMode(hLayer);
			
		case COLUMN_Thickness:
			return m_FontManagerRef.GetOutlineThickness(hLayer);

		case COLUMN_Color:
			return QVariant();
		}
	}

	return QVariant();
}

/*virtual*/ QVariant TextLayersModel::headerData(int iIndex, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
	if (role == Qt::DisplayRole)
	{
		if (orientation == Qt::Horizontal)
		{
			switch(iIndex)
			{
			case COLUMN_Mode:
				return QString("Type");
			case COLUMN_Thickness:
				return QString("Thickness");
			case COLUMN_Color:
				return QString("Color");
			}
		}
		else
			return QString::number(iIndex);
	}

	return QVariant();
}

/*virtual*/ bool TextLayersModel::setData(const QModelIndex &indexRef, const QVariant &valueRef, int iRole /*= Qt::EditRole*/)
{
	TextLayerHandle hLayer = m_LayerList[indexRef.row()];

	if(iRole == Qt::EditRole)
	{
		switch(indexRef.column())
		{
		case COLUMN_Mode:
			m_FontManagerRef.SetRenderMode(hLayer, static_cast<rendermode_t>(valueRef.toInt()));
			break;
		case COLUMN_Thickness:
			m_FontManagerRef.SetOutlineThickness(hLayer, valueRef.toFloat());
			break;
		case COLUMN_Color:
			
			break;
		}

		QVector<int> vRolesChanged;
		vRolesChanged.append(iRole);
		dataChanged(indexRef, indexRef, vRolesChanged);
	}

	return true;
}

/*virtual*/ Qt::ItemFlags TextLayersModel::flags(const QModelIndex &indexRef) const
{
	Qt::ItemFlags eFlags = Qt::ItemIsEditable | Qt::ItemIsEnabled;
	if(indexRef.column() != COLUMN_Color)
		eFlags |= Qt::ItemIsSelectable;

	return eFlags;
}
