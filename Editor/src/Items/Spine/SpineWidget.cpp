/**************************************************************************
*	SpineWidget.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "global.h"
#include "SpineWidget.h"
#include "Project.h"
#include "SpineUndoCmds.h"
#include "SpineDraw.h"
#include "GlobalUndoCmds.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//SpineCrossFadesTableView::SpineCrossFadesTableView(QWidget *pParent /*= 0*/) :
//	QTableView(pParent)
//{
//}

///*virtual*/ void SpineCrossFadesTableView::resizeEvent(QResizeEvent *pResizeEvent)
//{
//	// TODO: Use formula to account for device pixels and scaling using QWindow::devicePixelRatio()
//	int iWidth = pResizeEvent->size().width();
//
//	iWidth -= 64 + 64 + 64;
//	setColumnWidth(SpineCrossFadeModel::COLUMN_AnimOne, iWidth);
//	setColumnWidth(SpriteFramesModel::COLUMN_OffsetX, 64);
//	setColumnWidth(SpriteFramesModel::COLUMN_OffsetY, 64);
//	setColumnWidth(SpriteFramesModel::COLUMN_Duration, 64);
//
//	QTableView::resizeEvent(pResizeEvent);
//}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SpineCrossFadesDelegate::SpineCrossFadesDelegate(ProjectItemData *pItem, QTableView *pTableView, QObject *pParent /*= 0*/) :
	QStyledItemDelegate(pParent),
	m_pItem(pItem),
	m_pTableView(pTableView)
{
}

/*virtual*/ QWidget *SpineCrossFadesDelegate::createEditor(QWidget *pParent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QWidget *pReturnWidget = nullptr;

	switch(index.column())
	{
	case SpineCrossFadeModel::COLUMN_AnimOne:
	case SpineCrossFadeModel::COLUMN_AnimTwo: {
		QList<QPair<QString, QString>> crossFadePairList;
		static_cast<SpineModel *>(m_pItem->GetModel())->GetNextCrossFadeAnims(crossFadePairList);
		
		QStringList sValidAnimsList;
		sValidAnimsList.push_back(static_cast<SpineModel *>(m_pItem->GetModel())->GetCrossFadeModel().data(index, Qt::DisplayRole).toString());
		if(index.column() == SpineCrossFadeModel::COLUMN_AnimOne)
		{
			for(int i = 0; i < crossFadePairList.size(); ++i)
				sValidAnimsList.push_back(crossFadePairList[i].first);
		}
		else
		{
			for(int i = 0; i < crossFadePairList.size(); ++i)
				sValidAnimsList.push_back(crossFadePairList[i].second);
		}
		sValidAnimsList.removeDuplicates();

		pReturnWidget = new QComboBox(pParent);
		for(int i = 0; i < sValidAnimsList.size(); ++i)
			static_cast<QComboBox *>(pReturnWidget)->addItem(sValidAnimsList[i]);
		break; }

	case SpineCrossFadeModel::COLUMN_Mix:
		pReturnWidget = new QDoubleSpinBox(pParent);
		static_cast<QDoubleSpinBox *>(pReturnWidget)->setSingleStep(0.01);
		static_cast<QDoubleSpinBox *>(pReturnWidget)->setDecimals(3);
		//static_cast<QDoubleSpinBox *>(pReturnWidget)->setSuffix("sec");
		break;
	}

	return pReturnWidget;
}

/*virtual*/ void SpineCrossFadesDelegate::setEditorData(QWidget *pEditor, const QModelIndex &index) const
{
	QString sCurValue = index.model()->data(index, Qt::EditRole).toString();

	switch(index.column())
	{
	case SpineCrossFadeModel::COLUMN_AnimOne:
	case SpineCrossFadeModel::COLUMN_AnimTwo:
		static_cast<QComboBox *>(pEditor)->setCurrentText(sCurValue);
		break;

	case SpineCrossFadeModel::COLUMN_Mix:
		static_cast<QDoubleSpinBox *>(pEditor)->setValue(sCurValue.toDouble());
		break;
	}
}

/*virtual*/ void SpineCrossFadesDelegate::setModelData(QWidget *pEditor, QAbstractItemModel *pModel, const QModelIndex &index) const
{
	switch(index.column())
	{
	case SpineCrossFadeModel::COLUMN_AnimOne:
	case SpineCrossFadeModel::COLUMN_AnimTwo:
		m_pItem->GetUndoStack()->push(new SpineUndoCmd_ModifyCrossFade(*m_pItem, m_pTableView, index, static_cast<QComboBox *>(pEditor)->currentText()));
		break;

	case SpineCrossFadeModel::COLUMN_Mix:
		m_pItem->GetUndoStack()->push(new SpineUndoCmd_ModifyCrossFade(*m_pItem, m_pTableView, index, static_cast<QDoubleSpinBox *>(pEditor)->value()));
		break;
	}
}

/*virtual*/ void SpineCrossFadesDelegate::updateEditorGeometry(QWidget *pEditor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	pEditor->setGeometry(option.rect);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SpineWidget::SpineWidget(ProjectItemData &itemRef, QWidget *parent) :
	IWidget(itemRef, parent),
	ui(new Ui::SpineWidget)
{
	ui->setupUi(this);

	// Remove and re-add the main layout that holds everything. This makes the Qt Designer (.ui) files work with the base class 'IWidget'. Otherwise it jumbles them together.
	layout()->removeItem(ui->lytMainVerticalLayout);
	GetAboveStatesLayout()->addWidget(ui->grpAnimationMixing);
	GetBelowStatesLayout()->addItem(ui->lytMainVerticalLayout);

	SpineModel *pSpineModel = static_cast<SpineModel *>(m_ItemRef.GetModel());
	pSpineModel->GetDefaultMixMapper()->AddSpinBoxMapping(ui->sbDefaultMix);
	ui->sbDefaultMix->setSingleStep(0.01);
	ui->sbDefaultMix->setDecimals(3);

	ui->btnAddMix->setDefaultAction(ui->actionAddMix);
	ui->btnRemoveMix->setDefaultAction(ui->actionRemoveMix);
	ui->btnOrderMixUp->setDefaultAction(ui->actionOrderMixUp);
	ui->btnOrderMixDown->setDefaultAction(ui->actionOrderMixDown);

	ui->mixTableView->setModel(&pSpineModel->GetCrossFadeModel());
	ui->mixTableView->setItemDelegate(new SpineCrossFadesDelegate(&m_ItemRef, ui->mixTableView, this));
	ui->mixTableView->setColumnWidth(1, 50);
	ui->mixTableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
	ui->mixTableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
	ui->mixTableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
	QItemSelectionModel *pSelModel = ui->mixTableView->selectionModel();
	connect(pSelModel, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
		this, SLOT(on_mixTableView_selectionChanged(const QItemSelection &, const QItemSelection &)));
}

SpineWidget::~SpineWidget()
{
	delete ui;
}

/*virtual*/ void SpineWidget::OnGiveMenuActions(QMenu *pMenu) /*override*/
{
	//pMenu->addAction(ui.actionAddFill);
	//pMenu->addAction(ui.actionAddEdge);
	//pMenu->addAction(ui.actionAddEdgeFill);
	//pMenu->addAction(ui.actionAddInner);
	//pMenu->addAction(ui.actionAddSDF);
	//pMenu->addSeparator();
	//pMenu->addAction(ui.actionOrderLayerUp);
	//pMenu->addAction(ui.actionOrderLayerDown);
	//pMenu->addAction(ui.actionRemoveLayer);
}

/*virtual*/ void SpineWidget::OnUpdateActions() /*override*/
{
}

/*virtual*/ void SpineWidget::OnFocusState(int iStateIndex, QVariant subState) /*override*/
{
	SpineStateData *pCurStateData = static_cast<SpineStateData *>(static_cast<SpineModel *>(m_ItemRef.GetModel())->GetStateData(iStateIndex));
	ui->skinTreeView->setModel(&pCurStateData->GetSkinTreeModel());

	//SpineDraw *pDraw = static_cast<SpineDraw *>(m_ItemRef.GetDraw());
}

void SpineWidget::on_sbDefaultMix_valueChanged(double dValue)
{
	QUndoCommand *pCmd = new UndoCmd_DoubleSpinBox("Default Mix changed",
												   GetItem(),
												   static_cast<SpineModel *>(m_ItemRef.GetModel())->GetDefaultMixMapper(),
												   -1,
												   dValue);
	GetItem().GetUndoStack()->push(pCmd);
}

void SpineWidget::on_mixTableView_selectionChanged(const QItemSelection &newSelection, const QItemSelection &oldSelection)
{
	if(m_ItemRef.GetDraw())
	{
		int iRow = ui->mixTableView->currentIndex().row();
		SpineCrossFade *pCrossFade = static_cast<SpineCrossFadeModel *>(ui->mixTableView->model())->GetCrossFadeAt(iRow);
		static_cast<SpineDraw *>(m_ItemRef.GetDraw())->SetCrossFadePreview(pCrossFade);
	}

	UpdateActions();
}

void SpineWidget::on_btnAddMix_clicked()
{
	//  ui->mixTableWidget

	//SpineUndoCmd_AddNewMix *pCmd = new SpineUndoCmd_AddNewMix(GetItem(), sAnimOne, sAnimTwo, ui->sbDefaultMix->value());
	//GetItem().GetUndoStack->push(pCmd);
}

void SpineWidget::on_actionAddMix_triggered()
{
	SpineModel *pSpineModel = static_cast<SpineModel *>(m_ItemRef.GetModel());

	QList<QPair<QString, QString>> crossFadePairList;
	if(pSpineModel->GetNextCrossFadeAnims(crossFadePairList) && crossFadePairList.isEmpty() == false)
	{
		QString sAnimOne = crossFadePairList[0].first;
		QString sAnimTwo = crossFadePairList[0].second;
		float fMix = pSpineModel->GetDefaultMixMapper()->GetValue();

		QUndoCommand *pCmd = new SpineUndoCmd_AddNewCrossFade(GetItem(), sAnimOne, sAnimTwo, fMix);
		GetItem().GetUndoStack()->push(pCmd);
	}
}

void SpineWidget::on_actionRemoveMix_triggered()
{

}

void SpineWidget::on_actionOrderMixUp_triggered()
{
}

void SpineWidget::on_actionOrderMixDown_triggered()
{
}
