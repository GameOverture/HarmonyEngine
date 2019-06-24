/**************************************************************************
*	TextWidget.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2019 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "global.h"
#include "TextWidget.h"
#include "Project.h"
#include "TextUndoCmds.h"

#include <QComboBox>
#include <QFontDatabase>

TextWidget::TextWidget(ProjectItem &itemRef, QWidget *parent) :
	IWidget(itemRef, parent)
{
	ui.setupUi(this);

	// Remove and re-add the main layout that holds everything. This makes the Qt Designer (.ui) files work with the base class 'IWidget'. Otherwise it jumbles them together.
	layout()->removeItem(ui.verticalLayout_3);
	layout()->addItem(ui.verticalLayout_3);

	ui.btnAddFill->setDefaultAction(ui.actionAddFill);
	ui.btnAddSDF->setDefaultAction(ui.actionAddSDF);
	ui.btnAddEdge->setDefaultAction(ui.actionAddEdge);
	ui.btnAddEdgeFill->setDefaultAction(ui.actionAddEdgeFill);
	ui.btnAddInner->setDefaultAction(ui.actionAddInner);
	ui.btnRemoveLayer->setDefaultAction(ui.actionRemoveLayer);
	ui.btnOrderLayerUp->setDefaultAction(ui.actionOrderLayerUp);
	ui.btnOrderLayerDown->setDefaultAction(ui.actionOrderLayerDown);

	QStandardItemModel *pProjectFontsModel = m_ItemRef.GetProject().GetFontListModel();
	ui.cmbFont->setModel(pProjectFontsModel);

	// Set font size combobox with the standard sizes.
	QFontDatabase fontDatabase;
	QList<int> sizeList = fontDatabase.standardSizes();
	QStringList sSizeList;
	for(int i = 0; i < sizeList.size(); ++i)
		sSizeList.append(QString::number(sizeList[i]));

	ui.cmbSize->setFixedSize(64, 20);
	ui.cmbSize->setValidator(HyGlobal::NumbersValidator());
	ui.cmbSize->clear();
	ui.cmbSize->insertItems(0, sSizeList);

	ui.layersTableView->setItemDelegate(new TextLayersDelegate(&m_ItemRef, this));
	QItemSelectionModel *pSelModel = ui.layersTableView->selectionModel();
	//connect(pSelModel, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), this, SLOT(on_layersView_selectionChanged(const QItemSelection &, const QItemSelection &)));

	ui.glyphsTreeView->setModel(static_cast<TextModel *>(m_ItemRef.GetModel())->GetGlyphsModel());
}

TextWidget::~TextWidget()
{
}

/*virtual*/ void TextWidget::OnGiveMenuActions(QMenu *pMenu) /*override*/
{
	pMenu->addAction(ui.actionAddFill);
	pMenu->addAction(ui.actionAddEdge);
	pMenu->addAction(ui.actionAddEdgeFill);
	pMenu->addAction(ui.actionAddInner);
	pMenu->addAction(ui.actionAddSDF);
	pMenu->addSeparator();
	pMenu->addAction(ui.actionOrderLayerUp);
	pMenu->addAction(ui.actionOrderLayerDown);
	pMenu->addAction(ui.actionRemoveLayer);
}

/*virtual*/ void TextWidget::OnUpdateActions() /*override*/
{
}

/*virtual*/ void TextWidget::OnFocusState(int iStateIndex, QVariant subState) /*override*/
{
	TextLayersModel *pTextLayerModel = static_cast<TextModel *>(m_ItemRef.GetModel())->GetLayersModel(GetCurStateIndex());
	if(pTextLayerModel)
	{
		ui.layersTableView->setModel(pTextLayerModel);
		ui.layersTableView->resize(ui.layersTableView->size());

		QStandardItemModel *pProjectFontsModel = m_ItemRef.GetProject().GetFontListModel();
		QString sFontName = pTextLayerModel->GetFont();
		int iNumFonts = pProjectFontsModel->rowCount();
		for(int i = 0; i < iNumFonts; ++i)
		{
			QString sTest = pProjectFontsModel->item(i)->text();
			if(pProjectFontsModel->item(i)->text().compare(sFontName, Qt::CaseInsensitive) == 0)
			{
				ui.cmbFont->setCurrentIndex(i);
				break;
			}
		}
		ui.cmbSize->setCurrentText(QString::number(static_cast<int>(pTextLayerModel->GetFontSize())));
	}
}

void TextWidget::on_cmbFont_currentIndexChanged(int index)
{
}

//RENDER_OUTLINE_EDGE,
//RENDER_OUTLINE_POSITIVE,
//RENDER_OUTLINE_NEGATIVE,
//RENDER_SIGNED_DISTANCE_FIELD
void TextWidget::on_actionAddFill_triggered()
{
	QUndoCommand *pCmd = new TextUndoCmd_AddLayer(m_ItemRef,
												  GetCurStateIndex(),
												  ui.cmbFont->currentText(),
												  RENDER_NORMAL,
												  ui.cmbSize->currentText().toFloat(),
												  0.0f);
	m_ItemRef.GetUndoStack()->push(pCmd);
}
