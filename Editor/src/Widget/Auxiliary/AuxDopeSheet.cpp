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
#include "DlgInputNumber.h"
#include "ui_AuxDopeSheet.h"
#include <QClipboard>
#include <QApplication>

AuxDopeSheet::AuxDopeSheet(QWidget *pParent /*= nullptr*/) :
	QWidget(pParent),
	ui(new Ui::AuxDopeSheet)
{
	ui->setupUi(this);

	ui->btnRewind->setDefaultAction(ui->actionRewind);
	ui->btnPreviousKeyFrame->setDefaultAction(ui->actionPreviousKeyFrame);
	ui->btnPlayAnimations->setDefaultAction(ui->actionPlayAnimations);
	ui->btnNextKeyFrame->setDefaultAction(ui->actionNextKeyFrame);
	ui->btnLastKeyFrame->setDefaultAction(ui->actionLastKeyFrame);

	ui->btnCopyKeyFrames->setDefaultAction(ui->actionCopyFrames);
	ui->btnPasteKeyFrames->setDefaultAction(ui->actionPasteFrames);
	ui->btnDeleteKeyFrames->setDefaultAction(ui->actionDeleteFrames);

	ui->btnTweenBreak->setDefaultAction(ui->actionBreakTween);
	ui->btnTweenPos->setDefaultAction(ui->actionCreatePositionTween);
	ui->btnTweenRot->setDefaultAction(ui->actionCreateRotationTween);
	ui->btnTweenScale->setDefaultAction(ui->actionCreateScaleTween);
	ui->btnTweenAlpha->setDefaultAction(ui->actionCreateAlphaTween);

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
		m_WidgetMapper.setModel(static_cast<EntityModel &>(pEntStateData->GetModel()).GetAuxWidgetsModel());
		m_WidgetMapper.addMapping(ui->sbFramesPerSecond, AUXDOPEWIDGETSECTION_FramesPerSecond);
		m_WidgetMapper.addMapping(ui->chkAutoInitialize, AUXDOPEWIDGETSECTION_AutoInitialize);

		m_WidgetMapper.toFirst();
		m_WidgetMapper.revert();
	}
	else
		m_WidgetMapper.setModel(nullptr);

	UpdateWidgets();
}

QMenu *AuxDopeSheet::AllocContextMenu(bool bOnTimeline, EntityTreeItemData *pContextItem, int iContextFrameIndex)
{
	EntityDopeSheetScene *pScene = ui->graphicsView->GetScene();
	if(pScene == nullptr)
		return nullptr;

	QMenu *pNewMenu = new QMenu(nullptr);

	if(bOnTimeline)
	{
		// Create Callback
		QAction *pCreateCallback = new QAction(QIcon(":/icons16x16/callback.png"),
			"Create Frame " % QString::number(iContextFrameIndex) % " Callback",
			this);
		QJsonObject dataObj;
		dataObj.insert("contextAction", CONTEXTACTION_CallbackCreate);
		dataObj.insert("frame", iContextFrameIndex);
		pCreateCallback->setData(QVariant(dataObj));
		pCreateCallback->setToolTip("Set a callback function at the currently selected frame, that derived classes may override and implement");
		pNewMenu->addAction(pCreateCallback);

		// Rename Callback(s)
		QStringList sCallbackList = pScene->GetCallbackList(iContextFrameIndex);
		for(QString sCallback : sCallbackList)
		{
			QAction *pRenameCallback = new QAction(QIcon(":/icons16x16/callback-rename.png"),
												   "Rename '" % sCallback % "'");
			QJsonObject dataObj;
			dataObj.insert("contextAction", CONTEXTACTION_CallbackRename);
			dataObj.insert("frame", iContextFrameIndex);
			dataObj.insert("serializedData", sCallback);
			pRenameCallback->setData(QVariant(dataObj));
			pRenameCallback->setToolTip("Rename the callback function set at the currently selected frame");
			pNewMenu->addAction(pRenameCallback);
		}

		// Delete Callback(s)
		for(QString sCallback : sCallbackList)
		{
			QAction *pDeleteCallback = new QAction(QIcon(":/icons16x16/callback-delete.png"),
												   "Delete '" % sCallback % "'");
			QJsonObject dataObj;
			dataObj.insert("contextAction", CONTEXTACTION_CallbackDelete);
			dataObj.insert("frame", iContextFrameIndex);
			dataObj.insert("serializedData", sCallback);
			pDeleteCallback->setData(QVariant(dataObj));
			pDeleteCallback->setToolTip("Removes the callback function at the currently selected frame");
			pNewMenu->addAction(pDeleteCallback);
		}
		pNewMenu->addSeparator();

		//// Timeline Events - Insert appropriate context actions for each dope event type, based on what's already set
		//QAction *pUnpauseAction = nullptr;
		//QAction *pGotoFrameAction = nullptr;
		//QAction *pRemoveGotoFrameAction = nullptr;
		//QAction *pGotoStateAction = nullptr;
		//QAction *pRemoveGotoStateAction = nullptr;
		//QList<DopeSheetEvent> eventList = pScene->GetEventList(iContextFrameIndex);
		//for(DopeSheetEvent dopeEvent : eventList)
		//{
		//	switch(dopeEvent.GetDopeEventType())
		//	{
		//	case DOPEEVENT_PauseTimeline: {
		//		pUnpauseAction = new QAction(QIcon(":/icons16x16/media-play.png"), "Unpause Timeline On Frame " % QString::number(iContextFrameIndex));
		//		pUnpauseAction->setToolTip("Unpause the timeline at the currently selected frame");
		//		QJsonObject pauseDataObj;
		//		pauseDataObj.insert("contextAction", CONTEXTACTION_UnpauseTimeline);
		//		pauseDataObj.insert("frame", iContextFrameIndex);
		//		pauseDataObj.insert("serializedData", dopeEvent.GetSerialized());
		//		pUnpauseAction->setData(QVariant(pauseDataObj));
		//		break; }

		//	case DOPEEVENT_GotoFrame: {
		//		pGotoFrameAction = new QAction(QIcon(":/icons16x16/media-seek-forward.png"), "Modify Goto Frame " + dopeEvent.GetOptionalData());
		//		pGotoFrameAction->setToolTip("Change which frame to jump to. Currently set to " % dopeEvent.GetOptionalData());
		//		QJsonObject gotoDataObj;
		//		gotoDataObj.insert("contextAction", CONTEXTACTION_GotoFrame);
		//		gotoDataObj.insert("frame", iContextFrameIndex);
		//		gotoDataObj.insert("serializedData", dopeEvent.GetSerialized());
		//		pGotoFrameAction->setData(QVariant(gotoDataObj));

		//		pRemoveGotoFrameAction = new QAction(QIcon(":/icons16x16/edit-delete.png"), "Remove Goto Frame " % dopeEvent.GetOptionalData());
		//		pRemoveGotoFrameAction->setToolTip("Remove the Goto Frame " % dopeEvent.GetOptionalData());
		//		QJsonObject removeGotoDataObj;
		//		removeGotoDataObj.insert("contextAction", CONTEXTACTION_RemoveGotoFrame);
		//		removeGotoDataObj.insert("frame", iContextFrameIndex);
		//		removeGotoDataObj.insert("serializedData", dopeEvent.GetSerialized());
		//		pRemoveGotoFrameAction->setData(QVariant(removeGotoDataObj));
		//		break; }

		//	case DOPEEVENT_GotoState: {
		//		int iStateIndex = dopeEvent.GetOptionalData().toInt();
		//		const IStateData *pGotoStateData = static_cast<EntityModel &>(GetEntityStateModel()->GetModel()).GetStateData(iStateIndex);
		//		if(pGotoStateData)
		//		{
		//			pGotoStateAction = new QAction(QIcon(":/icons16x16/media-seek-forward.png"), "Modify Goto State " % pGotoStateData->GetName());
		//			pGotoStateAction->setToolTip("Change which state to jump to. Currently set to " % pGotoStateData->GetName());
		//			QJsonObject gotoDataObj;
		//			gotoDataObj.insert("contextAction", CONTEXTACTION_GotoState);
		//			gotoDataObj.insert("frame", iContextFrameIndex);
		//			gotoDataObj.insert("serializedData", dopeEvent.GetSerialized());
		//			pGotoStateAction->setData(QVariant(gotoDataObj));

		//			pRemoveGotoStateAction = new QAction(QIcon(":/icons16x16/edit-delete.png"), "Remove Goto State " % pGotoStateData->GetName());
		//			pRemoveGotoStateAction->setToolTip("Remove the Goto State " % pGotoStateData->GetName());
		//			QJsonObject removeGotoDataObj;
		//			removeGotoDataObj.insert("contextAction", CONTEXTACTION_RemoveGotoState);
		//			removeGotoDataObj.insert("frame", iContextFrameIndex);
		//			removeGotoDataObj.insert("serializedData", dopeEvent.GetSerialized());
		//			pRemoveGotoStateAction->setData(QVariant(removeGotoDataObj));
		//		}
		//		break; }

		//	default:
		//		break;
		//	} // switch(dopeEvent.m_eType)
		//} // for each dopeEvent set on this frame

		//// Insert all the actions into the context menu
		//if(pUnpauseAction)
		//	pNewMenu->addAction(pUnpauseAction);
		//else
		//{
		//	QAction *pPauseTimeline = new QAction(QIcon(":/icons16x16/media-pause.png"),
		//		"Pause Timeline On Frame " % QString::number(iContextFrameIndex));
		//	QJsonObject pauseDataObj;
		//	pauseDataObj.insert("contextAction", CONTEXTACTION_PauseTimeline);
		//	pauseDataObj.insert("frame", iContextFrameIndex);
		//	pauseDataObj.insert("serializedData", DOPEEVENT_STRINGS[DOPEEVENT_PauseTimeline]);
		//	pPauseTimeline->setData(QVariant(pauseDataObj));
		//	pPauseTimeline->setToolTip("Pause the timeline at the currently selected frame");
		//	pNewMenu->addAction(pPauseTimeline);
		//}

		//if(pGotoFrameAction)
		//{
		//	pNewMenu->addAction(pGotoFrameAction);
		//	pNewMenu->addAction(pRemoveGotoFrameAction);
		//}
		//else
		//{
		//	QAction *pGotoFrame = new QAction(QIcon(":/icons16x16/media-seek-forward.png"),
		//		"Set Goto Frame " % QString::number(iContextFrameIndex));
		//	QJsonObject gotoDataObj;
		//	gotoDataObj.insert("contextAction", CONTEXTACTION_GotoFrame);
		//	gotoDataObj.insert("frame", iContextFrameIndex);
		//	gotoDataObj.insert("serializedData", QString(DOPEEVENT_STRINGS[DOPEEVENT_GotoFrame] + "0"));
		//	pGotoFrame->setData(QVariant(gotoDataObj));
		//	pGotoFrame->setToolTip("Set which frame to jump to");
		//	pNewMenu->addAction(pGotoFrame);
		//}

		//if(pGotoStateAction)
		//{
		//	pNewMenu->addAction(pGotoStateAction);
		//	pNewMenu->addAction(pRemoveGotoStateAction);
		//}
		//else
		//{
		//	QAction *pGotoState = new QAction(QIcon(":/icons16x16/media-seek-forward.png"),
		//		"Set Goto State");
		//	QJsonObject gotoDataObj;
		//	gotoDataObj.insert("contextAction", CONTEXTACTION_GotoState);
		//	gotoDataObj.insert("frame", iContextFrameIndex);
		//	gotoDataObj.insert("serializedData", QString(DOPEEVENT_STRINGS[DOPEEVENT_GotoState] + "0"));
		//	pGotoState->setData(QVariant(gotoDataObj));
		//	pGotoState->setToolTip("Set which state to jump to");
		//	pNewMenu->addAction(pGotoState);
		//}
	}
	else // Context not on timeline
	{
		// Copy/Paste Frames actions
		pNewMenu->addAction(ui->actionCopyFrames);
		pNewMenu->addAction(ui->actionPasteFrames);
		pNewMenu->addAction(ui->actionPasteOnFrame);
		pNewMenu->addSeparator();

		QString sSelectAll;
		if(pContextItem)
			sSelectAll = "Select All '" % pContextItem->GetCodeName() % "' Key Frames";
		else
			sSelectAll = "Select All Key Frames";
		QAction *pSelectAllCallback = new QAction(sSelectAll);
		pSelectAllCallback->setEnabled(pContextItem != nullptr);
		QJsonObject selectDataObj;
		selectDataObj.insert("contextAction", CONTEXTACTION_SelectAllItemKeyFrames);
		pSelectAllCallback->setData(QVariant(selectDataObj));
		pNewMenu->addAction(pSelectAllCallback);

		QAction *pDeselectAllCallback = new QAction("Deselect Key Frames");
		pDeselectAllCallback->setEnabled(pScene->selectedItems().size() > 0);
		QJsonObject deselectDataObj;
		deselectDataObj.insert("contextAction", CONTEXTACTION_DeselectAllKeyFrames);
		pDeselectAllCallback->setData(QVariant(deselectDataObj));
		pNewMenu->addAction(pDeselectAllCallback);

		pNewMenu->addSeparator();

		// Delete Frames action
		pNewMenu->addAction(ui->actionDeleteFrames);
	}

	connect(pNewMenu, &QMenu::triggered, this, &AuxDopeSheet::OnEventActionTriggered);

	return pNewMenu;
}

void AuxDopeSheet::UpdateWidgets()
{
	// Context widgets are shown when appropriate
	ui->btnTweenBreak->hide();
	ui->btnTweenPos->hide();
	ui->btnTweenRot->hide();
	ui->btnTweenScale->hide();
	ui->btnTweenAlpha->hide();

	if(ui->graphicsView->scene())
	{
		ui->sbFramesPerSecond->setEnabled(true);
		ui->chkAutoInitialize->setEnabled(true);

		EntityDopeSheetScene &dopeSheetSceneRef = GetEntityStateModel()->GetDopeSheetScene();

		// Determine if a context quick-tween button(s) should be shown
		QList<ContextTweenData> quickTweenPropList = dopeSheetSceneRef.DetermineIfContextQuickTween();

		for(ContextTweenData contextTweenData : quickTweenPropList)
		{
			if(contextTweenData.m_bIsBreakTween)
			{
				ui->btnTweenBreak->setText(ui->actionBreakTween->text() + (ui->btnTweenBreak->isHidden() ? "" : "s"));
				ui->btnTweenBreak->show();
			}
			else
			{
				switch(contextTweenData.m_eTweenProperty)
				{
				case TWEENPROP_Position:
					ui->btnTweenPos->setText(ui->actionCreatePositionTween->text() + (ui->btnTweenPos->isHidden() ? "" : "s"));
					ui->btnTweenPos->show();
					break;
				case TWEENPROP_Rotation:
					ui->btnTweenRot->setText(ui->actionCreateRotationTween->text() + (ui->btnTweenRot->isHidden() ? "" : "s"));
					ui->btnTweenRot->show();
					break;
				case TWEENPROP_Scale:
					ui->btnTweenScale->setText(ui->actionCreateScaleTween->text() + (ui->btnTweenScale->isHidden() ? "" : "s"));
					ui->btnTweenScale->show();
					break;
				case TWEENPROP_Alpha:
					ui->btnTweenAlpha->setText(ui->actionCreateAlphaTween->text() + (ui->btnTweenAlpha->isHidden() ? "" : "s"));
					ui->btnTweenAlpha->show();
					break;
				default:
					HyGuiLog("AuxDopeSheet::UpdateWidgets - Unknown TweenProperty", LOGTYPE_Error);
					break;
				}
			}
		}
		
		QList<EntityTreeItemData *> selectedFrameItemList = ui->graphicsView->GetScene()->GetItemsFromSelectedFrames();
		ui->actionCopyFrames->setEnabled(selectedFrameItemList.size() == 1);

		const QMimeData *pMimeData = QApplication::clipboard()->mimeData();

		bool bCanPaste = ui->graphicsView->GetContextClickItem() && pMimeData->hasFormat(HyGlobal::MimeTypeString(MIMETYPE_EntityFrames));
		ui->actionPasteFrames->setEnabled(bCanPaste);
		ui->actionPasteOnFrame->setEnabled(bCanPaste);

		ui->actionDeleteFrames->setEnabled(selectedFrameItemList.size() > 0);
	}
	else
	{
		ui->sbFramesPerSecond->setEnabled(false);
		ui->chkAutoInitialize->setEnabled(false);

		ui->actionCopyFrames->setEnabled(false);
		ui->actionPasteFrames->setEnabled(false);
		ui->actionPasteOnFrame->setEnabled(false);
		ui->actionDeleteFrames->setEnabled(false);
	}
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

void AuxDopeSheet::OnEventActionTriggered(QAction *pEventAction)
{
	if(GetEntityStateModel() == nullptr)
	{
		HyGuiLog("AuxDopeSheet::OnEventActionTriggered() - GetEntityStateModel() == nullptr", LOGTYPE_Error);
		return;
	}
	QJsonObject dataObj = pEventAction->data().toJsonObject();
	if(dataObj.isEmpty())
		return; // No data to process, it was handled with the action's trigger signal

	ContextActionType eActionType = static_cast<ContextActionType>(dataObj["contextAction"].toInt());
	switch(eActionType)
	{
	case CONTEXTACTION_CallbackCreate:
	case CONTEXTACTION_CallbackRename: {
		int iFrameIndex = dataObj["frame"].toInt();

		QString sDefaultName;
		QString sTitle;
		if(eActionType == CONTEXTACTION_CallbackRename)
		{
			sDefaultName = dataObj["serializedData"].toString();
			sTitle = "Renaming '" % sDefaultName % "' Callback Function";
		}
		else
		{
			if(iFrameIndex == 0)
			{
				if(GetEntityStateModel()->GetName().isEmpty() == false)
					sDefaultName = "OnState" % GetEntityStateModel()->GetName();
				else
					sDefaultName = "OnState" % QString::number(GetEntityStateModel()->GetIndex());
			}
			else
				sDefaultName = "OnFrame" % QString::number(iFrameIndex);

			sTitle = "Create New Callback";
		}

		DlgInputName dlg(sTitle, sDefaultName, HyGlobal::CodeNameValidator(), [&](QString sName) -> QString {
			if(GetEntityStateModel()->GetDopeSheetScene().GetCallbackList(iFrameIndex).contains(sName))
				return "This frame (" % QString::number(iFrameIndex) % ") already has this callback function";
			return "";
			});
		if(dlg.exec() == QDialog::Accepted)
		{
			if(eActionType == CONTEXTACTION_CallbackCreate)
			{
				EntityUndoCmd_AddCallback *pNewCmd = new EntityUndoCmd_AddCallback(GetEntityStateModel()->GetDopeSheetScene(), iFrameIndex, dlg.GetName());
				GetEntityStateModel()->GetModel().GetItem().GetUndoStack()->push(pNewCmd);
			}
			else // EVENTACTION_CallbackRename
			{
				EntityUndoCmd_RenameCallback *pNewCmd = new EntityUndoCmd_RenameCallback(GetEntityStateModel()->GetDopeSheetScene(), iFrameIndex, sDefaultName, dlg.GetName());
				GetEntityStateModel()->GetModel().GetItem().GetUndoStack()->push(pNewCmd);
			}
		}
		break; }
	
	case CONTEXTACTION_CallbackDelete: {
		int iFrameIndex = dataObj["frame"].toInt();
		QString sCallbackName = dataObj["serializedData"].toString();
		EntityUndoCmd_RemoveCallback *pNewCmd = new EntityUndoCmd_RemoveCallback(GetEntityStateModel()->GetDopeSheetScene(), iFrameIndex, sCallbackName);
		GetEntityStateModel()->GetModel().GetItem().GetUndoStack()->push(pNewCmd);
		break; }

	//case CONTEXTACTION_PauseTimeline: {
	//	EntityUndoCmd_AddEvent *pNewCmd = new EntityUndoCmd_AddEvent(GetEntityStateModel()->GetDopeSheetScene(), dataObj["frame"].toInt(), "_PauseTimeline");
	//	GetEntityStateModel()->GetModel().GetItem().GetUndoStack()->push(pNewCmd);
	//	break; }

	//case CONTEXTACTION_UnpauseTimeline: {
	//	EntityUndoCmd_RemoveEvent *pNewCmd = new EntityUndoCmd_RemoveEvent(GetEntityStateModel()->GetDopeSheetScene(), dataObj["frame"].toInt(), "_PauseTimeline");
	//	GetEntityStateModel()->GetModel().GetItem().GetUndoStack()->push(pNewCmd);
	//	break; }

	//case CONTEXTACTION_GotoFrame: {
	//	if(dataObj["serializedData"].isString())
	//	{
	//		DopeSheetEvent dopeSheetEvent(dataObj["serializedData"].toString());

	//		DlgInputNumber dlg("Jump to Frame", "Frame", QIcon(), dopeSheetEvent.GetOptionalData().toInt(), 0, 99999, [](int) {return QString(""); });
	//		if(dlg.exec() == QDialog::Accepted)
	//		{
	//			dlg.GetValue();
	//			EntityUndoCmd_AddEvent *pNewCmd = new EntityUndoCmd_AddEvent(GetEntityStateModel()->GetDopeSheetScene(), dataObj["frame"].toInt(), DOPEEVENT_STRINGS[DOPEEVENT_GotoFrame] + QString::number(dlg.GetValue()));
	//			GetEntityStateModel()->GetModel().GetItem().GetUndoStack()->push(pNewCmd);
	//		}
	//	}
	//	break; }

	//case CONTEXTACTION_RemoveGotoFrame: {
	//	EntityUndoCmd_RemoveEvent *pNewCmd = new EntityUndoCmd_RemoveEvent(GetEntityStateModel()->GetDopeSheetScene(), dataObj["frame"].toInt(), dataObj["serializedData"].toString());
	//	GetEntityStateModel()->GetModel().GetItem().GetUndoStack()->push(pNewCmd);
	//	break; }

	//case CONTEXTACTION_GotoState: {
	//	if(dataObj["serializedData"].isString())
	//	{
	//		DopeSheetEvent dopeSheetEvent(dataObj["serializedData"].toString());
	//		int iNumStates = GetEntityStateModel()->GetModel().GetNumStates();
	//		
	//		// TODO: Make a DropDown for the state selection
	//		DlgInputNumber dlg("Jump to State", "State", QIcon(), dopeSheetEvent.GetOptionalData().toInt(), 0, iNumStates - 1, [](int) {return QString(""); });
	//		if(dlg.exec() == QDialog::Accepted)
	//		{
	//			dlg.GetValue();
	//			EntityUndoCmd_AddEvent *pNewCmd = new EntityUndoCmd_AddEvent(GetEntityStateModel()->GetDopeSheetScene(), dataObj["frame"].toInt(), DOPEEVENT_STRINGS[DOPEEVENT_GotoState] + QString::number(dlg.GetValue()));
	//			GetEntityStateModel()->GetModel().GetItem().GetUndoStack()->push(pNewCmd);
	//		}
	//	}
	//	break; }

	//case CONTEXTACTION_RemoveGotoState: {
	//	EntityUndoCmd_RemoveEvent *pNewCmd = new EntityUndoCmd_RemoveEvent(GetEntityStateModel()->GetDopeSheetScene(), dataObj["frame"].toInt(), dataObj["serializedData"].toString());
	//	GetEntityStateModel()->GetModel().GetItem().GetUndoStack()->push(pNewCmd);
	//	break; }

	case CONTEXTACTION_SelectAllItemKeyFrames:
		GetEntityStateModel()->GetDopeSheetScene().SelectAllItemKeyFrames(ui->graphicsView->GetContextClickItem());
		break;

	case CONTEXTACTION_DeselectAllKeyFrames:
		GetEntityStateModel()->GetDopeSheetScene().clearSelection();
		break;

	default:
		HyGuiLog("AuxDopeSheet::OnEventActionTriggered() - Unknown ContextActionType", LOGTYPE_Error);
		break;
	}
}

void AuxDopeSheet::on_actionBreakTween_triggered()
{
	CreateContextTween(TWEENPROP_None);
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

void AuxDopeSheet::on_actionCopyFrames_triggered()
{
	// Get selected items
	QList<QGraphicsItem *> selectedItems = ui->graphicsView->GetScene()->selectedItems();

	// Serialize all the selected items to the clipboard
	QJsonObject serializedKeyFramesObj = ui->graphicsView->GetScene()->SerializeSpecifiedKeyFrames(selectedItems);

	// Copy the serialized data to the clipboard
	EntityFrameMimeData *pFrameMimeData = new EntityFrameMimeData(serializedKeyFramesObj);
	QApplication::clipboard()->setMimeData(pFrameMimeData);
}

void AuxDopeSheet::on_actionPasteFrames_triggered()
{
	PasteFrames(-1);
}

void AuxDopeSheet::on_actionPasteOnFrame_triggered()
{
	PasteFrames(ui->graphicsView->GetScene()->GetCurrentFrame());
}

void AuxDopeSheet::on_actionDeleteFrames_triggered()
{
	// Get selected items
	QList<QGraphicsItem *> selectedItems = ui->graphicsView->GetScene()->selectedItems();

	// Serialize all the selected items to the clipboard
	QJsonObject serializedKeyFramesObj = ui->graphicsView->GetScene()->SerializeSpecifiedKeyFrames(selectedItems);

	EntityUndoCmd_PopKeyFrames *pNewCmd = new EntityUndoCmd_PopKeyFrames(*ui->graphicsView->GetScene(), serializedKeyFramesObj);
	GetEntityStateModel()->GetModel().GetItem().GetUndoStack()->push(pNewCmd);
}

void AuxDopeSheet::CreateContextTween(TweenProperty eTweenProp)
{
	if(GetEntityStateModel() == nullptr)
	{
		HyGuiLog("AuxDopeSheet::CreateContextTween() - GetEntityStateModel() == nullptr", LOGTYPE_Error);
		return;
	}

	QList<ContextTweenData> fullContextTweenDataList = GetEntityStateModel()->GetDopeSheetScene().DetermineIfContextQuickTween();
	if(fullContextTweenDataList.isEmpty())
	{
		HyGuiLog("AuxDopeSheet::CreateContextTween() - No valid " + HyGlobal::TweenPropName(eTweenProp) + " context tweens found", LOGTYPE_Error);
		UpdateWidgets(); // UpdateWidgets() should have been called to hide invalid context tool buttons
		return;
	}

	if(eTweenProp == TWEENPROP_None) // Indicates break tweens when used here
	{
		QList<ContextTweenData> breakTweenDataList;
		for(int i = 0; i < fullContextTweenDataList.size(); ++i)
		{
			if(fullContextTweenDataList[i].m_bIsBreakTween)
				breakTweenDataList.push_back(fullContextTweenDataList[i]);
		}

		EntityUndoCmd_BreakTween *pNewCmd = new EntityUndoCmd_BreakTween(GetEntityStateModel()->GetDopeSheetScene(), breakTweenDataList);
		GetEntityStateModel()->GetModel().GetItem().GetUndoStack()->push(pNewCmd);
	}
	else
	{
		QList<ContextTweenData> contextTweenDataList;
		for(int i = 0; i < fullContextTweenDataList.size(); ++i)
		{
			if(fullContextTweenDataList[i].m_eTweenProperty == eTweenProp)
				contextTweenDataList.push_back(fullContextTweenDataList[i]);
		}
		EntityUndoCmd_ConvertToTween *pNewCmd = new EntityUndoCmd_ConvertToTween(GetEntityStateModel()->GetDopeSheetScene(), contextTweenDataList);
		GetEntityStateModel()->GetModel().GetItem().GetUndoStack()->push(pNewCmd);
	}
}

void AuxDopeSheet::PasteFrames(int iStartFrameIndex)
{
	// Get the serialized data from the clipboard
	const QMimeData *pMimeData = QApplication::clipboard()->mimeData();
	if(pMimeData->hasFormat(HyGlobal::MimeTypeString(MIMETYPE_EntityFrames)))
	{
		const EntityFrameMimeData &entityFrameMimeDataRef = static_cast<const EntityFrameMimeData &>(*pMimeData);
		if(entityFrameMimeDataRef.IsValidForPaste() == false)
		{
			HyGuiLog("AuxDopeSheet::on_actionPasteFrames_triggered() - entityFrameMimeDataRef.IsValidForPaste() == false", LOGTYPE_Error);
			return;
		}

		QJsonDocument jsonDocument = QJsonDocument::fromJson(pMimeData->data(HyGlobal::MimeTypeString(MIMETYPE_EntityFrames)));
		QJsonObject serializedKeyFrameObj = jsonDocument.object();

		if(ui->graphicsView->GetContextClickItem() == nullptr)
		{
			HyGuiLog("AuxDopeSheet::on_actionPasteFrames_triggered() - ui->graphicsView->GetContextClickItem() == nullptr", LOGTYPE_Error);
			return;
		}

		EntityUndoCmd_PasteKeyFrames *pCmd = new EntityUndoCmd_PasteKeyFrames(*ui->graphicsView->GetScene(),
																			  ui->graphicsView->GetContextClickItem(),
																			  serializedKeyFrameObj,
																			  iStartFrameIndex);
		GetEntityStateModel()->GetModel().GetItem().GetUndoStack()->push(pCmd);
	}
}
