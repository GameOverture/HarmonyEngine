/**************************************************************************
*	TextLayersWidget.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2019 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "global.h"
#include "TextLayersWidget.h"
#include "TextLayersModel.h"
#include "TextUndoCmds.h"
#include "DlgColorPicker.h"

#include <QResizeEvent>

TextLayersWidget::TextLayersWidget(QWidget *pParent /*= nullptr*/) :
	QTableView(pParent)
{
}

/*virtual*/ void TextLayersWidget::resizeEvent(QResizeEvent *pResizeEvent)
{
	int iWidth = pResizeEvent->size().width();

	iWidth -= 144;
	setColumnWidth(TextLayersModel::COLUMN_Mode, iWidth);
	setColumnWidth(TextLayersModel::COLUMN_Thickness, 64);
	setColumnWidth(TextLayersModel::COLUMN_Color, 80);

	QTableView::resizeEvent(pResizeEvent);
}

TextLayersDelegate::TextLayersDelegate(ProjectItemData *pItem, QObject *pParent /*= nullptr*/) :
	QStyledItemDelegate(pParent),
	m_pItem(pItem)
{
}

/*virtual*/ QWidget *TextLayersDelegate::createEditor(QWidget *pParent, const QStyleOptionViewItem &option, const QModelIndex &index) const /*override*/
{
	QWidget *pReturnWidget = nullptr;

	switch(index.column())
	{
	case TextLayersModel::COLUMN_Mode:
		pReturnWidget = new QComboBox(pParent);
		static_cast<QComboBox *>(pReturnWidget)->addItem("Fill"); // RENDER_NORMAL
		static_cast<QComboBox *>(pReturnWidget)->addItem("Outline"); // RENDER_OUTLINE_EDGE
		static_cast<QComboBox *>(pReturnWidget)->addItem("Fill + Outline"); // RENDER_OUTLINE_POSITIVE
		static_cast<QComboBox *>(pReturnWidget)->addItem("Inner"); // RENDER_OUTLINE_NEGATIVE
		static_cast<QComboBox *>(pReturnWidget)->addItem("Signed Dist Field"); // RENDER_SIGNED_DISTANCE_FIELD
		break;

	case TextLayersModel::COLUMN_Thickness: {
		const TextLayersModel *pModel = static_cast<const TextLayersModel *>(index.model());
		rendermode_t eMode = pModel->GetFontManager().GetRenderMode(pModel->GetHandle(index));
		if(eMode != RENDER_NORMAL && eMode != RENDER_SIGNED_DISTANCE_FIELD)
		{
			pReturnWidget = new QDoubleSpinBox(pParent);
			static_cast<QDoubleSpinBox *>(pReturnWidget)->setRange(0.0, 4096.0);
		}
		break; }

	case TextLayersModel::COLUMN_Color: {
		const TextLayersModel *pModel = static_cast<const TextLayersModel *>(index.model());
		TextLayerHandle hLayer = pModel->GetHandle(index);
		QColor topColor, botColor;
		pModel->GetFontManager().GetColor(hLayer, topColor, botColor);

		DlgColorPicker *pDlg = new DlgColorPicker("Choose Font Layer Color", topColor, botColor, pParent);
		if(pDlg->exec() == QDialog::Accepted)
		{
			QColor newTopColor, newBotColor;
			if(pDlg->IsSolidColor())
			{
				newTopColor = pDlg->GetSolidColor();
				newBotColor = pDlg->GetSolidColor();
			}
			else
			{
				newTopColor = pDlg->GetVgTopColor();
				newBotColor = pDlg->GetVgBotColor();
			}

			m_pItem->GetUndoStack()->push(new TextUndoCmd_LayerColors(*m_pItem,
																	  m_pItem->GetWidget()->GetCurStateIndex(),
																	  hLayer,
																	  topColor,
																	  botColor,
																	  newTopColor,
																	  newBotColor));
		}
		break; }
	}

	return pReturnWidget;
}

/*virtual*/ void TextLayersDelegate::setEditorData(QWidget *pEditor, const QModelIndex &index) const /*override*/
{
	const TextLayersModel *pModel = static_cast<const TextLayersModel *>(index.model());
	
	switch(index.column())
	{
	case TextLayersModel::COLUMN_Mode:
		static_cast<QComboBox *>(pEditor)->setCurrentIndex(pModel->data(index, Qt::EditRole).toInt());
		break;

	case TextLayersModel::COLUMN_Thickness:
		static_cast<QDoubleSpinBox *>(pEditor)->setValue(pModel->data(index, Qt::EditRole).toFloat());
		break;

	case TextLayersModel::COLUMN_Color:
		break;
	}
}

/*virtual*/ void TextLayersDelegate::setModelData(QWidget *pEditor, QAbstractItemModel *pItemModel, const QModelIndex &index) const /*override*/
{
	TextLayersModel *pModel = static_cast<TextLayersModel *>(pItemModel);
	TextLayerHandle hLayer = pModel->GetHandle(index);

	switch(index.column())
	{
	case TextLayersModel::COLUMN_Mode:
		if(pModel->GetFontManager().GetRenderMode(hLayer) != static_cast<rendermode_t>(static_cast<QComboBox *>(pEditor)->currentIndex()))
		{
			m_pItem->GetUndoStack()->push(new TextUndoCmd_LayerRenderMode(*m_pItem,
																		  m_pItem->GetWidget()->GetCurStateIndex(),
																		  hLayer,
																		  pModel->GetFontManager().GetRenderMode(hLayer),
																		  static_cast<rendermode_t>(static_cast<QComboBox *>(pEditor)->currentIndex())));
		}
		break;

	case TextLayersModel::COLUMN_Thickness:
		if(HyCompareFloat(static_cast<double>(pModel->GetFontManager().GetOutlineThickness(hLayer)), static_cast<QDoubleSpinBox *>(pEditor)->value()) == false)
		{
			m_pItem->GetUndoStack()->push(new TextUndoCmd_LayerOutlineThickness(*m_pItem,
																				m_pItem->GetWidget()->GetCurStateIndex(),
																				hLayer,
																				pModel->GetFontManager().GetOutlineThickness(hLayer),
																				static_cast<QDoubleSpinBox *>(pEditor)->value()));
		}
		break;

	case TextLayersModel::COLUMN_Color:
		break;
	}
}

/*virtual*/ void TextLayersDelegate::updateEditorGeometry(QWidget *pEditor, const QStyleOptionViewItem &option, const QModelIndex &index) const /*override*/
{
	pEditor->setGeometry(option.rect);
}
