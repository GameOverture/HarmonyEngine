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

TextLayersModel::TextLayersModel(TextFontManager &fontManagerRef, const QList<TextLayerHandle> &layerList, QObject *pParent) :
	QAbstractTableModel(pParent),
	m_FontManagerRef(fontManagerRef),
	m_LayerList(layerList)
{
}

/*virtual*/ TextLayersModel::~TextLayersModel()
{
}

TextFontManager &TextLayersModel::GetFontManager()
{
	return m_FontManagerRef;
}

const TextFontManager &TextLayersModel::GetFontManager() const
{
	return m_FontManagerRef;
}

QJsonArray TextLayersModel::GetLayersArray()
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
}

void TextLayersModel::MoveRowUp(int iIndex)
{
	if(beginMoveRows(QModelIndex(), iIndex, iIndex, QModelIndex(), iIndex - 1) == false)
		return;

	m_LayerList.swap(iIndex, iIndex - 1);
	endMoveRows();
}

void TextLayersModel::MoveRowDown(int iIndex)
{
	if(beginMoveRows(QModelIndex(), iIndex, iIndex, QModelIndex(), iIndex + 2) == false)
		return;

	m_LayerList.swap(iIndex, iIndex + 1);
	endMoveRows();
}

QString TextLayersModel::GetFont() const
{
	if(m_LayerList.empty())
		return QString();

	return m_FontManagerRef.GetFontName(m_LayerList[0]);
}

void TextLayersModel::SetFont(QString sFontName)
{
}

float TextLayersModel::GetFontSize() const
{
	if(m_LayerList.empty())
		return 0.0f;

	return m_FontManagerRef.GetSize(m_LayerList[0]);
}

void TextLayersModel::SetFontSize(float fSize)
{
}

//void TextLayersModel::SetFontSize(int iSize)
//{
//	for(int i = 0; i < m_LayerList.count(); ++i)
//		m_LayerList[i]->iSize = iSize;
//}
//
////void TextLayersModel::SetFontStageReference(int iRowIndex, FontTypeface *pStageRef)
////{
////	m_LayerList[iRowIndex]->pReference = pStageRef;
////}
//
//float TextLayersModel::GetLineHeight()
//{
//	float fHeight = 0.0f;
//
//	for(int i = 0; i < m_LayerList.count(); ++i)
//	{
//		if(fHeight < m_LayerList[i]->pReference->pTextureFont->height)
//			fHeight = m_LayerList[i]->pReference->pTextureFont->height;
//	}
//
//	return fHeight;
//}
//
//float TextLayersModel::GetLineAscender()
//{
//	float fAscender = 0.0f;
//
//	for(int i = 0; i < m_LayerList.count(); ++i)
//	{
//		if(fAscender < abs(m_LayerList[i]->pReference->pTextureFont->ascender))
//			fAscender = abs(m_LayerList[i]->pReference->pTextureFont->ascender);
//	}
//
//	return fAscender;
//}
//
//float TextLayersModel::GetLineDescender()
//{
//	float fDescender = 0.0f;
//
//	for(int i = 0; i < m_LayerList.count(); ++i)
//	{
//		if(fDescender < abs(m_LayerList[i]->pReference->pTextureFont->descender))
//			fDescender = abs(m_LayerList[i]->pReference->pTextureFont->descender);
//	}
//
//	return fDescender;
//}
//
//float TextLayersModel::GetLeftSideNudgeAmt(QString sAvailableTypefaceGlyphs)
//{
//	float fLeftSideNudgeAmt = 0.0f;
//
//	for(int i = 0; i < m_LayerList.count(); ++i)
//	{
//		for(int j = 0; j < sAvailableTypefaceGlyphs.count(); ++j)
//		{
//			// NOTE: Assumes LITTLE ENDIAN
//			QString sSingleChar = sAvailableTypefaceGlyphs[j];
//			texture_glyph_t *pGlyph = texture_font_get_glyph(m_LayerList[i]->pReference->pTextureFont, sSingleChar.toUtf8().data());
//
//			// Only keep track of negative offset_x's
//			if(pGlyph->offset_x < 0 && fLeftSideNudgeAmt < abs(pGlyph->offset_x))
//				fLeftSideNudgeAmt = abs(pGlyph->offset_x);
//		}
//	}
//
//	return fLeftSideNudgeAmt;
//}

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
	return m_LayerList[indexRef.row()];
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
