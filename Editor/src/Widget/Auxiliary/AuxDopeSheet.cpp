/**************************************************************************
 *	AuxDopeSheet.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2023 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "AuxDopeSheet.h"
#include "EntityModel.h"
#include "EntityUndoCmds.h"
#include "DlgInputName.h"
#include "ui_AuxDopeSheet.h"

AuxDopeSheet::AuxDopeSheet(QWidget *pParent /*= nullptr*/) :
	QWidget(pParent),
	ui(new Ui::AuxDopeSheet),
	m_pContextTweenTreeItemData(nullptr),
	m_iContextTweenStartFrame(-1),
	m_iContextTweenEndFrame(-1)
{
	ui->setupUi(this);

	ui->btnRewind->setDefaultAction(ui->actionRewind);
	ui->btnPreviousKeyFrame->setDefaultAction(ui->actionPreviousKeyFrame);
	ui->btnPlayAnimations->setDefaultAction(ui->actionPlayAnimations);
	ui->btnNextKeyFrame->setDefaultAction(ui->actionNextKeyFrame);
	ui->btnLastKeyFrame->setDefaultAction(ui->actionLastKeyFrame);

	m_WidgetMapper.setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
	UpdateWidgets();
}

/*virtual*/ AuxDopeSheet::~AuxDopeSheet()
{
	delete ui;
}

EntityStateData *AuxDopeSheet::GetEntityStateModel() const
{
	if(ui->graphicsView->GetScene() == nullptr)
		return nullptr;

	return ui->graphicsView->GetScene()->GetStateData();
}

void AuxDopeSheet::SetEntityStateModel(EntityStateData *pEntStateData)
{
	ui->graphicsView->SetScene(this, pEntStateData);

	m_WidgetMapper.clearMapping();

	if(pEntStateData)
	{
		m_WidgetMapper.setModel(pEntStateData->GetDopeSheetScene().GetAuxWidgetsModel());
		m_WidgetMapper.addMapping(ui->sbFramesPerSecond, AUXDOPEWIDGETSECTION_FramesPerSecond);
		m_WidgetMapper.addMapping(ui->chkAutoInitialize, AUXDOPEWIDGETSECTION_AutoInitialize);

		m_WidgetMapper.toFirst();
		m_WidgetMapper.revert();
	}
	else
		m_WidgetMapper.setModel(nullptr);

	UpdateWidgets();
}

void AuxDopeSheet::UpdateWidgets()
{
	if(ui->graphicsView->scene())
	{
		ui->sbFramesPerSecond->setEnabled(true);
		ui->chkAutoInitialize->setEnabled(true);
		ui->btnCallback->setVisible(true);

		EntityDopeSheetScene &dopeSheetSceneRef = GetEntityStateModel()->GetDopeSheetScene();
		QString sCallback = dopeSheetSceneRef.GetCallback(dopeSheetSceneRef.GetCurrentFrame());
		if(sCallback.isEmpty())
		{
			ui->actionCreateCallback->setText("Create Frame " % QString::number(dopeSheetSceneRef.GetCurrentFrame()) % " Callback");
			ui->btnCallback->setDefaultAction(ui->actionCreateCallback);
		}
		else
		{
			ui->actionRenameCallback->setText("Rename " % sCallback);
			ui->btnCallback->setDefaultAction(ui->actionRenameCallback);
		}

		m_pContextTweenTreeItemData = nullptr;
		m_iContextTweenStartFrame = m_iContextTweenEndFrame = -1;
		ui->btnTween->setDefaultAction(nullptr);
		ui->btnTween->setVisible(false);
		if(dopeSheetSceneRef.selectedItems().count() == 2)
		{
			if(dopeSheetSceneRef.selectedItems()[0]->data(GraphicsKeyFrameItem::DATAKEY_Type).toInt() == DOPESHEETITEMTYPE_PropertyKeyFrame &&
				dopeSheetSceneRef.selectedItems()[1]->data(GraphicsKeyFrameItem::DATAKEY_Type).toInt() == DOPESHEETITEMTYPE_PropertyKeyFrame)
			{
				KeyFrameKey tupleKey0 = static_cast<GraphicsKeyFrameItem *>(dopeSheetSceneRef.selectedItems()[0])->GetKey();
				KeyFrameKey tupleKey1 = static_cast<GraphicsKeyFrameItem *>(dopeSheetSceneRef.selectedItems()[1])->GetKey();

				EntityTreeItemData *pTreeItemData0 = std::get<GraphicsKeyFrameItem::DATAKEY_TreeItemData>(tupleKey0);
				EntityTreeItemData *pTreeItemData1 = std::get<GraphicsKeyFrameItem::DATAKEY_TreeItemData>(tupleKey1);
				
				m_iContextTweenStartFrame = std::get<GraphicsKeyFrameItem::DATAKEY_FrameIndex>(tupleKey0);
				m_iContextTweenEndFrame = std::get<GraphicsKeyFrameItem::DATAKEY_FrameIndex>(tupleKey1);
				if(m_iContextTweenStartFrame > m_iContextTweenEndFrame)
					std::swap(m_iContextTweenStartFrame, m_iContextTweenEndFrame);

				QString sCategoryProp0 = std::get<GraphicsKeyFrameItem::DATAKEY_CategoryPropString>(tupleKey0);
				QString sCategoryProp1 = std::get<GraphicsKeyFrameItem::DATAKEY_CategoryPropString>(tupleKey1);

				if(pTreeItemData0 == pTreeItemData1 &&
				   m_iContextTweenStartFrame != m_iContextTweenEndFrame &&
				   sCategoryProp0 == sCategoryProp1)
				{
					m_pContextTweenTreeItemData = pTreeItemData0;

					ui->btnTween->setVisible(true);

					TweenProperty eCurrentTweenProp = HyGlobal::GetTweenPropFromString(sCategoryProp0.split('/')[1]);
					switch(eCurrentTweenProp)
					{
					case TWEENPROP_Position:
						ui->btnTween->setDefaultAction(ui->actionCreatePositionTween);
						break;
					case TWEENPROP_Rotation:
						ui->btnTween->setDefaultAction(ui->actionCreateRotationTween);
						break;
					case TWEENPROP_Scale:
						ui->btnTween->setDefaultAction(ui->actionCreateScaleTween);
						break;
					case TWEENPROP_Alpha:
						ui->btnTween->setDefaultAction(ui->actionCreateAlphaTween);
						break;
					}
				}
			}
		}
	}
	else
	{
		m_pContextTweenTreeItemData = nullptr;
		ui->sbFramesPerSecond->setEnabled(false);
		ui->chkAutoInitialize->setEnabled(false);
		ui->btnCallback->setVisible(false);
		ui->btnTween->setVisible(false);
	}
}

QList<QAction *> AuxDopeSheet::GetContextActions()
{
	QList<QAction *> actionList;
	actionList.push_back(ui->btnCallback->defaultAction());
	if(actionList[0] == ui->actionRenameCallback)
		actionList.push_back(ui->actionDeleteCallback);

	if(ui->btnTween->isVisible())
		actionList.push_back(ui->btnTween->defaultAction());

	return actionList;
}

void AuxDopeSheet::on_actionRewind_triggered()
{
	if(GetEntityStateModel() == nullptr)
		return;

	GetEntityStateModel()->GetDopeSheetScene().SetCurrentFrame(0);
	UpdateWidgets();
}

void AuxDopeSheet::on_actionPreviousKeyFrame_triggered()
{
	if(GetEntityStateModel() == nullptr)
		return;

	GetEntityStateModel()->GetDopeSheetScene().SetCurrentFrame(GetEntityStateModel()->GetDopeSheetScene().GetCurrentFrame() - 1);
	UpdateWidgets();
}

void AuxDopeSheet::on_actionPlayAnimations_triggered()
{
}

void AuxDopeSheet::on_actionNextKeyFrame_triggered()
{
	if(GetEntityStateModel() == nullptr)
		return;

	GetEntityStateModel()->GetDopeSheetScene().SetCurrentFrame(GetEntityStateModel()->GetDopeSheetScene().GetCurrentFrame() + 1);
	UpdateWidgets();
}

void AuxDopeSheet::on_actionLastKeyFrame_triggered()
{

}

void AuxDopeSheet::on_actionCreateCallback_triggered()
{
	SetCallbackName();
}

void AuxDopeSheet::on_actionRenameCallback_triggered()
{
	SetCallbackName();
}

void AuxDopeSheet::SetCallbackName()
{
	if(GetEntityStateModel() == nullptr)
	{
		HyGuiLog("AuxDopeSheet::on_actionCreateCallback_triggered() - GetEntityStateModel() == nullptr", LOGTYPE_Error);
		return;
	}

	int iFrame = GetEntityStateModel()->GetDopeSheetScene().GetCurrentFrame();

	QString sDefaultName = GetEntityStateModel()->GetDopeSheetScene().GetCallback(iFrame);
	if(sDefaultName.isEmpty())
	{
		if(iFrame == 0)
		{
			if(GetEntityStateModel()->GetName().isEmpty() == false)
				sDefaultName = "OnState" % GetEntityStateModel()->GetName();
			else
				sDefaultName = "OnState" % QString::number(GetEntityStateModel()->GetIndex());
		}
		else
			sDefaultName = "OnFrame" % QString::number(iFrame);
	}

	DlgInputName dlg("Set Callback Function Name", sDefaultName, HyGlobal::CodeNameValidator(), [&](QString sName) -> QString {
			if(static_cast<EntityModel &>(GetEntityStateModel()->GetModel()).IsCallbackNameUnique(sName) == false)
				return "This callback function name already exists for this entity";
			return "";
		});
	if(dlg.exec() == QDialog::Accepted)
	{
		EntityUndoCmd_SetCallback *pNewCmd = new EntityUndoCmd_SetCallback(GetEntityStateModel()->GetDopeSheetScene(), dlg.GetName(), iFrame);
		GetEntityStateModel()->GetModel().GetItem().GetUndoStack()->push(pNewCmd);
	}
}

void AuxDopeSheet::on_actionDeleteCallback_triggered()
{
	if(GetEntityStateModel() == nullptr)
	{
		HyGuiLog("AuxDopeSheet::on_actionDeleteCallback_triggered() - GetEntityStateModel() == nullptr", LOGTYPE_Error);
		return;
	}

	int iFrame = GetEntityStateModel()->GetDopeSheetScene().GetCurrentFrame();
	if(GetEntityStateModel()->GetDopeSheetScene().GetCallback(iFrame).isEmpty())
		return;

	EntityUndoCmd_SetCallback *pNewCmd = new EntityUndoCmd_SetCallback(GetEntityStateModel()->GetDopeSheetScene(), "", iFrame);
	GetEntityStateModel()->GetModel().GetItem().GetUndoStack()->push(pNewCmd);
}

void AuxDopeSheet::on_actionCreatePositionTween_triggered()
{
	CreateContextTween(TWEENPROP_Position);
}

void AuxDopeSheet::on_actionCreateRotationTween_triggered()
{
	CreateContextTween(TWEENPROP_Rotation);
}

void AuxDopeSheet::on_actionCreateScaleTween_triggered()
{
	CreateContextTween(TWEENPROP_Scale);
}

void AuxDopeSheet::on_actionCreateAlphaTween_triggered()
{
	CreateContextTween(TWEENPROP_Alpha);
}

void AuxDopeSheet::CreateContextTween(TweenProperty eTweenProp)
{
	if(GetEntityStateModel() == nullptr)
	{
		HyGuiLog("AuxDopeSheet::CreateContextTween() - GetEntityStateModel() == nullptr", LOGTYPE_Error);
		return;
	}
	EntityUndoCmd_CreateTween *pNewCmd = new EntityUndoCmd_CreateTween(GetEntityStateModel()->GetDopeSheetScene(),
		m_pContextTweenTreeItemData,
		eTweenProp,
		m_iContextTweenStartFrame,
		m_iContextTweenEndFrame);
	GetEntityStateModel()->GetModel().GetItem().GetUndoStack()->push(pNewCmd);
}
