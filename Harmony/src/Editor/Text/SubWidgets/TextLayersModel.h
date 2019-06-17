/**************************************************************************
*	TextLayersModel.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2019 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef TEXTLAYERSMODEL_H
#define TEXTLAYERSMODEL_H

#include "IModel.h"
#include "ProjectItem.h"
#include "EntityTreeModel.h"
#include "GlobalWidgetMappers.h"
#include "ProjectItemMimeData.h"
#include "TextFontManager.h"

#include <QObject>
#include <QJsonArray>

class TextLayersModel : public QAbstractTableModel
{
	Q_OBJECT

	static TextLayerHandle			sm_hHandleCount;

	enum Column
	{
		COLUMN_Type = 0,
		COLUMN_Thickness,
		COLUMN_Color,

		NUMCOLUMNS
	};

	TextFontManager &				m_FontManagerRef;

	struct Layer
	{
		const TextLayerHandle		m_hUNIQUE_ID;
		TextFontHandle				m_hFont;

		uint						m_uiFontIndex;
		glm::vec4					m_vBotColor;
		glm::vec4					m_vTopColor;

		Layer(TextLayerHandle m_hUniqueId, uint uiFontIndex, glm::vec4 vBotColor, glm::vec4 vTopColor) :
			m_hUNIQUE_ID(m_hUniqueId),
			m_hFont(TEXTHANDLE_NotUsed),
			m_uiFontIndex(uiFontIndex),
			m_vBotColor(vBotColor),
			m_vTopColor(vTopColor)
		{ }
	};
	QList<Layer *>					m_LayerList;
	QList<QPair<int, Layer *> >		m_RemovedLayerList;

public:
	TextLayersModel(QJsonArray layerArray, TextFontManager &fontManagerRef, QObject *parent);
	virtual ~TextLayersModel();

	QJsonArray GetLayersArray();

	TextFontHandle AddNewLayer(QString sFontName, rendermode_t eRenderMode, float fSize, float fOutlineThickness);
	void RemoveLayer(TextFontHandle hHandle);
	void ReAddLayer(TextFontHandle hHandle);

	//int GetLayerId(int iRowIndex) const;
	////FontTypeface *GetStageRef(int iRowIndex);

	//rendermode_t GetLayerRenderMode(int iRowIndex) const;
	//void SetLayerRenderMode(int iId, rendermode_t eMode);

	//float GetLayerOutlineThickness(int iRowIndex) const;
	//void SetLayerOutlineThickness(int iId, float fThickness);

	//QColor GetLayerTopColor(int iRowIndex) const;
	//QColor GetLayerBotColor(int iRowIndex) const;
	//void SetLayerColors(int iId, QColor topColor, QColor botColor);

	//void MoveRowUp(int iIndex);
	//void MoveRowDown(int iIndex);

	//void SetFontSize(int iSize);

	////void SetFontStageReference(int iRowIndex, FontTypeface *pStageRef);

	//// Must generate final texture atlas/fonts before these are valid
	//float GetLineHeight();
	//float GetLineAscender();
	//float GetLineDescender();
	//float GetLeftSideNudgeAmt(QString sAvailableTypefaceGlyphs);

	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	virtual QVariant data(const QModelIndex &indexRef, int iRole = Qt::DisplayRole) const override;
	virtual QVariant headerData(int iIndex, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	virtual bool setData(const QModelIndex &indexRef, const QVariant &valueRef, int iRole = Qt::EditRole) override;
	virtual Qt::ItemFlags flags(const QModelIndex & index) const override;
};

#endif // TEXTLAYERSMODEL_H
