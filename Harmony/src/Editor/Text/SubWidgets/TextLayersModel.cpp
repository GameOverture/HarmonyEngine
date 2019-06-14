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


int TextLayersModel::sm_iUniqueIdCounter = 0;

TextLayersModel::TextLayersModel(QObject *parent) :
	QAbstractTableModel(parent)
{
	m_sRenderModeStringList.append("Fill");
	m_sRenderModeStringList.append("Outline Edge");
	m_sRenderModeStringList.append("Outline Edge+Fill");
	m_sRenderModeStringList.append("Inner");
	m_sRenderModeStringList.append("Signed Dist Field");
}

/*virtual*/ TextLayersModel::~TextLayersModel()
{
}


QString TextLayersModel::GetRenderModeString(rendermode_t eMode) const
{
	return m_sRenderModeStringList[eMode];
}

int TextLayersModel::AddNewLayer(rendermode_t eRenderMode, int iSize, float fOutlineThickness)
{
	sm_iUniqueIdCounter++;

	int iRowIndex = m_LayerList.count();
	FontLayer *pLayer = new FontLayer(sm_iUniqueIdCounter, eRenderMode, iSize, fOutlineThickness);

	beginInsertRows(QModelIndex(), iRowIndex, iRowIndex);
	m_LayerList.append(pLayer);
	endInsertRows();

	return pLayer->iUNIQUE_ID;
}

void TextLayersModel::RemoveLayer(int iId)
{
	for(int i = 0; i < m_LayerList.count(); ++i)
	{
		if(m_LayerList[i]->iUNIQUE_ID == iId)
		{
			beginRemoveRows(QModelIndex(), i, i);
			m_RemovedLayerList.append(QPair<int, FontLayer *>(i, m_LayerList[i]));
			m_LayerList.removeAt(i);
			endRemoveRows();
		}
	}
}

void TextLayersModel::ReAddLayer(int iId)
{
	for(int i = 0; i < m_RemovedLayerList.count(); ++i)
	{
		if(m_RemovedLayerList[i].second->iUNIQUE_ID == iId)
		{
			beginInsertRows(QModelIndex(), m_RemovedLayerList[i].first, m_RemovedLayerList[i].first);
			m_LayerList.insert(m_RemovedLayerList[i].first, m_RemovedLayerList[i].second);
			m_RemovedLayerList.removeAt(i);
			endInsertRows();
		}
	}
}

int TextLayersModel::GetLayerId(int iRowIndex) const
{
	return m_LayerList[iRowIndex]->iUNIQUE_ID;
}

TextLayersModel::FontTypeface *TextLayersModel::GetStageRef(int iRowIndex)
{
	return m_LayerList[iRowIndex]->pReference;
}

rendermode_t TextLayersModel::GetLayerRenderMode(int iRowIndex) const
{
	return m_LayerList[iRowIndex]->eMode;
}

void TextLayersModel::SetLayerRenderMode(int iId, rendermode_t eMode)
{
	for(int i = 0; i < m_LayerList.count(); ++i)
	{
		if(m_LayerList[i]->iUNIQUE_ID == iId)
		{
			m_LayerList[i]->eMode = eMode;
			dataChanged(createIndex(i, COLUMN_Type), createIndex(i, COLUMN_Type));
		}
	}
}

float TextLayersModel::GetLayerOutlineThickness(int iRowIndex) const
{
	return m_LayerList[iRowIndex]->fOutlineThickness;
}

void TextLayersModel::SetLayerOutlineThickness(int iId, float fThickness)
{
	for(int i = 0; i < m_LayerList.count(); ++i)
	{
		if(m_LayerList[i]->iUNIQUE_ID == iId)
		{
			m_LayerList[i]->fOutlineThickness = fThickness;
			dataChanged(createIndex(i, COLUMN_Thickness), createIndex(i, COLUMN_Thickness));
		}
	}
}

QColor TextLayersModel::GetLayerTopColor(int iRowIndex) const
{
	return QColor(m_LayerList[iRowIndex]->vTopColor.x * 255.0f, m_LayerList[iRowIndex]->vTopColor.y * 255.0f, m_LayerList[iRowIndex]->vTopColor.z * 255.0f);
}

QColor TextLayersModel::GetLayerBotColor(int iRowIndex) const
{
	return QColor(m_LayerList[iRowIndex]->vBotColor.x * 255.0f, m_LayerList[iRowIndex]->vBotColor.y * 255.0f, m_LayerList[iRowIndex]->vBotColor.z * 255.0f);
}

void TextLayersModel::SetLayerColors(int iId, QColor topColor, QColor botColor)
{
	for(int i = 0; i < m_LayerList.count(); ++i)
	{
		if(m_LayerList[i]->iUNIQUE_ID == iId)
		{
			m_LayerList[i]->vTopColor = glm::vec4(topColor.redF(), topColor.greenF(), topColor.blueF(), 1.0f);
			m_LayerList[i]->vBotColor = glm::vec4(botColor.redF(), botColor.greenF(), botColor.blueF(), 1.0f);
			dataChanged(createIndex(i, COLUMN_DefaultColor), createIndex(i, COLUMN_DefaultColor));
		}
	}
}

float TextLayersModel::GetLineHeight()
{
	float fHeight = 0.0f;

	for(int i = 0; i < m_LayerList.count(); ++i)
	{
		if(fHeight < m_LayerList[i]->pReference->pTextureFont->height)
			fHeight = m_LayerList[i]->pReference->pTextureFont->height;
	}

	return fHeight;
}

float TextLayersModel::GetLineAscender()
{
	float fAscender = 0.0f;

	for(int i = 0; i < m_LayerList.count(); ++i)
	{
		if(fAscender < abs(m_LayerList[i]->pReference->pTextureFont->ascender))
			fAscender = abs(m_LayerList[i]->pReference->pTextureFont->ascender);
	}

	return fAscender;
}

float TextLayersModel::GetLineDescender()
{
	float fDescender = 0.0f;

	for(int i = 0; i < m_LayerList.count(); ++i)
	{
		if(fDescender < abs(m_LayerList[i]->pReference->pTextureFont->descender))
			fDescender = abs(m_LayerList[i]->pReference->pTextureFont->descender);
	}

	return fDescender;
}

float TextLayersModel::GetLeftSideNudgeAmt(QString sAvailableTypefaceGlyphs)
{
	float fLeftSideNudgeAmt = 0.0f;

	for(int i = 0; i < m_LayerList.count(); ++i)
	{
		for(int j = 0; j < sAvailableTypefaceGlyphs.count(); ++j)
		{
			// NOTE: Assumes LITTLE ENDIAN
			QString sSingleChar = sAvailableTypefaceGlyphs[j];
			texture_glyph_t *pGlyph = texture_font_get_glyph(m_LayerList[i]->pReference->pTextureFont, sSingleChar.toUtf8().data());

			// Only keep track of negative offset_x's
			if(pGlyph->offset_x < 0 && fLeftSideNudgeAmt < abs(pGlyph->offset_x))
				fLeftSideNudgeAmt = abs(pGlyph->offset_x);
		}
	}

	return fLeftSideNudgeAmt;
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
	if(beginMoveRows(QModelIndex(), iIndex, iIndex, QModelIndex(), iIndex + 2) == false)    // + 2 is here because Qt is retarded
		return;

	m_LayerList.swap(iIndex, iIndex + 1);
	endMoveRows();
}

void TextLayersModel::SetFontSize(int iSize)
{
	for(int i = 0; i < m_LayerList.count(); ++i)
		m_LayerList[i]->iSize = iSize;
}

void TextLayersModel::SetFontStageReference(int iRowIndex, FontTypeface *pStageRef)
{
	m_LayerList[iRowIndex]->pReference = pStageRef;
}

/*virtual*/ int TextLayersModel::rowCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
	return m_LayerList.count();
}

/*virtual*/ int TextLayersModel::columnCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
	return NUMCOLUMNS;
}

/*virtual*/ QVariant TextLayersModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const
{
	FontLayer *pLayer = m_LayerList[index.row()];

	if (role == Qt::TextAlignmentRole && index.column() != COLUMN_Type)
	{
		return Qt::AlignCenter;
	}

	if(role == Qt::BackgroundRole && index.column() == COLUMN_DefaultColor)
	{
		QLinearGradient gradient(0, 0, 0, 25.0f);
		gradient.setColorAt(0.0, QColor(pLayer->vTopColor.x * 255.0f, pLayer->vTopColor.y * 255.0f, pLayer->vTopColor.z * 255.0f));
		gradient.setColorAt(1.0, QColor(pLayer->vBotColor.x * 255.0f, pLayer->vBotColor.y * 255.0f, pLayer->vBotColor.z * 255.0f));

		QBrush bgColorBrush(gradient);
		return QVariant(bgColorBrush);
	}

	if(role == Qt::ForegroundRole && index.column() == COLUMN_DefaultColor)
	{
		// Counting the perceptive luminance - human eye favors green color
		double a = 1 - ( 0.299 * pLayer->vTopColor.x + 0.587 * pLayer->vTopColor.y + 0.114 * pLayer->vTopColor.z)/255;

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

	if(role == Qt::DisplayRole || role == Qt::EditRole)
	{
		switch(index.column())
		{
		case COLUMN_Type:
			return GetRenderModeString(pLayer->eMode);
		case COLUMN_Thickness:
			return QString::number(GetLayerOutlineThickness(index.row()), 'g', 2);
		case COLUMN_DefaultColor:
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
			case COLUMN_DefaultColor:
				return QString("Default Color");
			}
		}
		else
			return QString::number(iIndex);
	}

	return QVariant();
}

/*virtual*/ bool TextLayersModel::setData(const QModelIndex & index, const QVariant & value, int role /*= Qt::EditRole*/)
{
	HyGuiLog("WidgetFontModel::setData was invoked", LOGTYPE_Error);

	//    SpriteFrame *pFrame = m_FramesList[index.row()];

	//    if(role == Qt::EditRole)
	//    {
	//        switch(index.column())
	//        {
	//        case COLUMN_OffsetX:
	//            pFrame->m_vOffset.setX(value.toInt());
	//            break;
	//        case COLUMN_OffsetY:
	//            pFrame->m_vOffset.setY(value.toInt());
	//            break;
	//        case COLUMN_Duration:
	//            pFrame->m_fDuration = value.toFloat();
	//            break;
	//        }
	//    }

	//    QVector<int> vRolesChanged;
	//    vRolesChanged.append(role);
	//    dataChanged(index, index, vRolesChanged);

	return true;
}

/*virtual*/ Qt::ItemFlags TextLayersModel::flags(const QModelIndex & index) const
{
	// TODO: Make a read only version of all entries
	//    if(index.column() == COLUMN_Type)
	//        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
	//    else
	return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}
