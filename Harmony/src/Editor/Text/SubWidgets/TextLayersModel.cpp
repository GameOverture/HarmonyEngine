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

QJsonArray TextLayersModel::GetLayersArray()
{
	QJsonArray layersArray;
	for(int i = 0; i < m_LayerList.size(); ++i)
	{
		QJsonObject layerObj;
		layerObj.insert("typefaceIndex", static_cast<int>(m_LayerList[i]->m_uiFontIndex));
		layerObj.insert("botR", m_LayerList[i]->m_vBotColor.r);
		layerObj.insert("botG", m_LayerList[i]->m_vBotColor.g);
		layerObj.insert("botB", m_LayerList[i]->m_vBotColor.b);
		layerObj.insert("topR", m_LayerList[i]->m_vTopColor.r);
		layerObj.insert("topG", m_LayerList[i]->m_vTopColor.g);
		layerObj.insert("topB", m_LayerList[i]->m_vTopColor.b);

		layersArray.append(layerObj);
	}

	return layersArray;
}

TextLayerHandle TextLayersModel::AddNewLayer(QString sFontName, rendermode_t eRenderMode, float fOutlineThickness, float fSize)
{
	TextLayerHandle hReturnHandle = m_FontManagerRef.AddNewLayer(sFontName, eRenderMode, fOutlineThickness, fSize);
	Layer *pLayer = new Layer( hReturnHandle);

	int iRowIndex = m_LayerList.count();
	beginInsertRows(QModelIndex(), iRowIndex, iRowIndex);
	m_LayerList.append(pLayer);
	endInsertRows();

	return hReturnHandle;
}

void TextLayersModel::RemoveLayer(TextLayerHandle hHandle)
{
	for(int i = 0; i < m_LayerList.count(); ++i)
	{
		if(m_LayerList[i]->m_hFont == hHandle)
		{
			beginRemoveRows(QModelIndex(), i, i);
			m_RemovedLayerList.append(QPair<int, Layer *>(i, m_LayerList[i]));
			m_LayerList.removeAt(i);
			endRemoveRows();
		}
	}
}

void TextLayersModel::ReAddLayer(TextLayerHandle hHandle)
{
	for(int i = 0; i < m_RemovedLayerList.count(); ++i)
	{
		if(m_RemovedLayerList[i].second->m_hFont == hHandle)
		{
			beginInsertRows(QModelIndex(), m_RemovedLayerList[i].first, m_RemovedLayerList[i].first);
			m_LayerList.insert(m_RemovedLayerList[i].first, m_RemovedLayerList[i].second);
			m_RemovedLayerList.removeAt(i);
			endInsertRows();
		}
	}
}

//int TextLayersModel::GetLayerId(int iRowIndex) const
//{
//	return m_LayerList[iRowIndex]->iUNIQUE_ID;
//}
//
//TextLayersModel::FontTypeface *TextLayersModel::GetStageRef(int iRowIndex)
//{
//	return m_LayerList[iRowIndex]->pReference;
//}
//
//rendermode_t TextLayersModel::GetLayerRenderMode(int iRowIndex) const
//{
//	return m_LayerList[iRowIndex]->eMode;
//}
//
//void TextLayersModel::SetLayerRenderMode(int iId, rendermode_t eMode)
//{
//	for(int i = 0; i < m_LayerList.count(); ++i)
//	{
//		if(m_LayerList[i]->iUNIQUE_ID == iId)
//		{
//			m_LayerList[i]->eMode = eMode;
//			dataChanged(createIndex(i, COLUMN_Type), createIndex(i, COLUMN_Type));
//		}
//	}
//}
//
//float TextLayersModel::GetLayerOutlineThickness(int iRowIndex) const
//{
//	return m_LayerList[iRowIndex]->fOutlineThickness;
//}
//
//void TextLayersModel::SetLayerOutlineThickness(int iId, float fThickness)
//{
//	for(int i = 0; i < m_LayerList.count(); ++i)
//	{
//		if(m_LayerList[i]->iUNIQUE_ID == iId)
//		{
//			m_LayerList[i]->fOutlineThickness = fThickness;
//			dataChanged(createIndex(i, COLUMN_Thickness), createIndex(i, COLUMN_Thickness));
//		}
//	}
//}
//
//QColor TextLayersModel::GetLayerTopColor(int iRowIndex) const
//{
//	return QColor(m_LayerList[iRowIndex]->vTopColor.x * 255.0f, m_LayerList[iRowIndex]->vTopColor.y * 255.0f, m_LayerList[iRowIndex]->vTopColor.z * 255.0f);
//}
//
//QColor TextLayersModel::GetLayerBotColor(int iRowIndex) const
//{
//	return QColor(m_LayerList[iRowIndex]->vBotColor.x * 255.0f, m_LayerList[iRowIndex]->vBotColor.y * 255.0f, m_LayerList[iRowIndex]->vBotColor.z * 255.0f);
//}
//
//void TextLayersModel::SetLayerColors(int iId, QColor topColor, QColor botColor)
//{
//	for(int i = 0; i < m_LayerList.count(); ++i)
//	{
//		if(m_LayerList[i]->iUNIQUE_ID == iId)
//		{
//			m_LayerList[i]->vTopColor = glm::vec4(topColor.redF(), topColor.greenF(), topColor.blueF(), 1.0f);
//			m_LayerList[i]->vBotColor = glm::vec4(botColor.redF(), botColor.greenF(), botColor.blueF(), 1.0f);
//			dataChanged(createIndex(i, COLUMN_Color), createIndex(i, COLUMN_Color));
//		}
//	}
//}
//
//void TextLayersModel::MoveRowUp(int iIndex)
//{
//	if(beginMoveRows(QModelIndex(), iIndex, iIndex, QModelIndex(), iIndex - 1) == false)
//		return;
//
//	m_LayerList.swap(iIndex, iIndex - 1);
//	endMoveRows();
//}
//
//void TextLayersModel::MoveRowDown(int iIndex)
//{
//	if(beginMoveRows(QModelIndex(), iIndex, iIndex, QModelIndex(), iIndex + 2) == false)    // + 2 is here because Qt is retarded
//		return;
//
//	m_LayerList.swap(iIndex, iIndex + 1);
//	endMoveRows();
//}
//
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
	Layer *pLayer = m_LayerList[indexRef.row()];

	if(iRole == Qt::TextAlignmentRole && indexRef.column() != COLUMN_Type)
		return Qt::AlignCenter;

	if(indexRef.column() == COLUMN_Color)
	{
		if(iRole == Qt::BackgroundRole)
		{
			QLinearGradient gradient(0, 0, 0, 25.0f);
			gradient.setColorAt(0.0, QColor(pLayer->m_vTopColor.x * 255.0f, pLayer->m_vTopColor.y * 255.0f, pLayer->m_vTopColor.z * 255.0f));
			gradient.setColorAt(1.0, QColor(pLayer->m_vBotColor.x * 255.0f, pLayer->m_vBotColor.y * 255.0f, pLayer->m_vBotColor.z * 255.0f));

			QBrush bgColorBrush(gradient);
			return QVariant(bgColorBrush);
		}
		else if(iRole == Qt::ForegroundRole)
		{
			// Counting the perceptive luminance - human eye favors green color
			double a = 1 - ( 0.299 * pLayer->m_vTopColor.x + 0.587 * pLayer->m_vTopColor.y + 0.114 * pLayer->m_vTopColor.z)/255;

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

	if(iRole == Qt::DisplayRole || iRole == Qt::EditRole)
	{
		switch(indexRef.column())
		{
		case COLUMN_Type:
			switch(m_FontManagerRef.GetRenderMode(pLayer->m_uiFontIndex))
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
			switch(m_FontManagerRef.GetRenderMode(pLayer->m_uiFontIndex))
			{
			case RENDER_NORMAL:
			case RENDER_SIGNED_DISTANCE_FIELD:
				return "N/A";
			case RENDER_OUTLINE_EDGE:
			case RENDER_OUTLINE_POSITIVE:
			case RENDER_OUTLINE_NEGATIVE:
				return QString::number(m_FontManagerRef.GetOutlineThickness(pLayer->m_uiFontIndex), 'g', 2);
			}

		case COLUMN_Color:
			return "";
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
			case COLUMN_Type:
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
	Layer *pLayer = m_LayerList[indexRef.row()];

	if(iRole == Qt::EditRole)
	{
		switch(indexRef.column())
		{
		case COLUMN_Type:
			pFrame->m_vOffset.setX(value.toInt());
			break;
		case COLUMN_Thickness:
			pFrame->m_vOffset.setY(value.toInt());
			break;
		case COLUMN_Color:
			pFrame->m_fDuration = value.toFloat();
			break;
		}

		QVector<int> vRolesChanged;
		vRolesChanged.append(iRole);
		dataChanged(indexRef, indexRef, vRolesChanged);
	}

	return true;
}

/*virtual*/ Qt::ItemFlags TextLayersModel::flags(const QModelIndex & index) const
{
	return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}
