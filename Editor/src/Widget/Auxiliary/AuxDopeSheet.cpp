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
#include <QShortcut>

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
	//ui->btnPasteKeyFrames->setDefaultAction(ui->actionPasteFrames);
	ui->btnDeleteKeyFrames->setDefaultAction(ui->actionDeleteFrames);

	ui->btnTweenBreak->setDefaultAction(ui->actionBreakTween);
	ui->btnTweenPos->setDefaultAction(ui->actionCreatePositionTween);
	ui->btnTweenRot->setDefaultAction(ui->actionCreateRotationTween);
	ui->btnTweenScale->setDefaultAction(ui->actionCreateScaleTween);
	ui->btnTweenAlpha->setDefaultAction(ui->actionCreateAlphaTween);

	m_WidgetMapper.setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
	UpdateWidgets();

	new QShortcut(QKeySequence(Qt::Key_Space), this, SLOT(on_actionPlayAnimations_triggered()));
	new QShortcut(QKeySequence(Qt::Key_Q), this, SLOT(on_actionRewind_triggered()));
	new QShortcut(QKeySequence(Qt::Key_E), this, SLOT(on_actionLastKeyFrame_triggered()));
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
			dataObj.insert("contextData", sCallback);
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
			dataObj.insert("contextData", sCallback);
			pDeleteCallback->setData(QVariant(dataObj));
			pDeleteCallback->setToolTip("Removes the callback function at the currently selected frame");
			pNewMenu->addAction(pDeleteCallback);
		}
		pNewMenu->addSeparator();

		// Timeline Events - Insert appropriate context actions for each dope event type, based on what's already set
		QAction *pTimeInsertAction = new QAction(QIcon(":/icons16x16/time-stopwatch.png"),
			"Insert Time After Frame " % QString::number(iContextFrameIndex),
			this);
		QJsonObject timeInsertDataObj;
		timeInsertDataObj.insert("contextAction", CONTEXTACTION_TimeInsert);
		timeInsertDataObj.insert("frame", iContextFrameIndex);
		pTimeInsertAction->setData(QVariant(timeInsertDataObj));
		pTimeInsertAction->setToolTip("All tweens active on frame " % QString::number(iContextFrameIndex) % " have their durations extended, and all frames beyond this one are offset later");
		pNewMenu->addAction(pTimeInsertAction);
		
		EntityDopeSheetScene &dopeSheetSceneRef = GetEntityStateModel()->GetDopeSheetScene();
		if(dopeSheetSceneRef.DetermineEmptyTimeFromFrame(iContextFrameIndex) > 0)
		{
			QAction *pTimeRemoveAction = new QAction(QIcon(":/icons16x16/time-stopwatchDelete.png"),
				"Remove Time After Frame " % QString::number(iContextFrameIndex),
				this);
			QJsonObject timeRemoveDataObj;
			timeRemoveDataObj.insert("contextAction", CONTEXTACTION_TimeRemove);
			timeRemoveDataObj.insert("frame", iContextFrameIndex);
			pTimeRemoveAction->setData(QVariant(timeRemoveDataObj));
			pTimeRemoveAction->setToolTip("All tweens active on frame " % QString::number(iContextFrameIndex) % " have their durations shortened, and all frames beyond this one are offset earlier");
			pNewMenu->addAction(pTimeRemoveAction);
		}

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
		//		pauseDataObj.insert("contextData", dopeEvent.GetSerialized());
		//		pUnpauseAction->setData(QVariant(pauseDataObj));
		//		break; }

		//	case DOPEEVENT_GotoFrame: {
		//		pGotoFrameAction = new QAction(QIcon(":/icons16x16/media-seek-forward.png"), "Modify Goto Frame " + dopeEvent.GetOptionalData());
		//		pGotoFrameAction->setToolTip("Change which frame to jump to. Currently set to " % dopeEvent.GetOptionalData());
		//		QJsonObject gotoDataObj;
		//		gotoDataObj.insert("contextAction", CONTEXTACTION_GotoFrame);
		//		gotoDataObj.insert("frame", iContextFrameIndex);
		//		gotoDataObj.insert("contextData", dopeEvent.GetSerialized());
		//		pGotoFrameAction->setData(QVariant(gotoDataObj));

		//		pRemoveGotoFrameAction = new QAction(QIcon(":/icons16x16/edit-delete.png"), "Remove Goto Frame " % dopeEvent.GetOptionalData());
		//		pRemoveGotoFrameAction->setToolTip("Remove the Goto Frame " % dopeEvent.GetOptionalData());
		//		QJsonObject removeGotoDataObj;
		//		removeGotoDataObj.insert("contextAction", CONTEXTACTION_RemoveGotoFrame);
		//		removeGotoDataObj.insert("frame", iContextFrameIndex);
		//		removeGotoDataObj.insert("contextData", dopeEvent.GetSerialized());
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
		//			gotoDataObj.insert("contextData", dopeEvent.GetSerialized());
		//			pGotoStateAction->setData(QVariant(gotoDataObj));

		//			pRemoveGotoStateAction = new QAction(QIcon(":/icons16x16/edit-delete.png"), "Remove Goto State " % pGotoStateData->GetName());
		//			pRemoveGotoStateAction->setToolTip("Remove the Goto State " % pGotoStateData->GetName());
		//			QJsonObject removeGotoDataObj;
		//			removeGotoDataObj.insert("contextAction", CONTEXTACTION_RemoveGotoState);
		//			removeGotoDataObj.insert("frame", iContextFrameIndex);
		//			removeGotoDataObj.insert("contextData", dopeEvent.GetSerialized());
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
		//	pauseDataObj.insert("contextData", DOPEEVENT_STRINGS[DOPEEVENT_PauseTimeline]);
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
		//	gotoDataObj.insert("contextData", QString(DOPEEVENT_STRINGS[DOPEEVENT_GotoFrame] + "0"));
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
		//	gotoDataObj.insert("contextData", QString(DOPEEVENT_STRINGS[DOPEEVENT_GotoState] + "0"));
		//	pGotoState->setData(QVariant(gotoDataObj));
		//	pGotoState->setToolTip("Set which state to jump to");
		//	pNewMenu->addAction(pGotoState);
		//}
	}
	else // Context not on timeline
	{
		// Copy Frames action
		pNewMenu->addAction(ui->actionCopyFrames);

		// Paste Frames actions
		const QMimeData *pMimeData = QApplication::clipboard()->mimeData();
		bool bCanPaste = pMimeData && pMimeData->hasFormat(HyGlobal::MimeTypeString(MIMETYPE_EntityFrames));

		QAction *pPasteAction = new QAction("Paste Frames");
		if(bCanPaste)
		{
			//if(bBestGuess)
			//	pPasteAction->setText("Paste Frames (Best Guess)");

			QJsonObject pasteDataObj;
			pasteDataObj.insert("contextAction", CONTEXTACTION_Paste);
			pPasteAction->setData(QVariant(pasteDataObj));
			pNewMenu->addAction(pPasteAction);

			QAction *pPasteAtFrameAction = new QAction("Paste Frames starting at " % QString::number(iContextFrameIndex));
			QJsonObject pasteAtFrameDataObj;
			pasteAtFrameDataObj.insert("contextAction", CONTEXTACTION_PasteAtFrame);
			pasteAtFrameDataObj.insert("frame", iContextFrameIndex);
			pPasteAtFrameAction->setData(QVariant(pasteAtFrameDataObj));
			pNewMenu->addAction(pPasteAtFrameAction);

			if(pContextItem)
			{
				QAction *pPasteIntoItemAction = new QAction("Paste Frames into " % pContextItem->GetCodeName());
				QJsonObject pasteIntoItemDataObj;
				pasteIntoItemDataObj.insert("contextAction", CONTEXTACTION_PasteIntoItem);
				pasteIntoItemDataObj.insert("contextData", pContextItem->GetThisUuid().toString(QUuid::WithoutBraces));
				pPasteIntoItemAction->setData(QVariant(pasteIntoItemDataObj));
				pNewMenu->addAction(pPasteIntoItemAction);

				QAction *pPasteIntoItemAtFrameAction = new QAction("Paste Frames onto " % pContextItem->GetCodeName() % " starting at " % QString::number(iContextFrameIndex));
				QJsonObject pasteIntoItemAtFrameDataObj;
				pasteIntoItemAtFrameDataObj.insert("contextAction", CONTEXTACTION_PasteIntoItemAtFrame);
				pasteIntoItemAtFrameDataObj.insert("frame", iContextFrameIndex);
				pasteIntoItemAtFrameDataObj.insert("contextData", pContextItem->GetThisUuid().toString(QUuid::WithoutBraces));
				pPasteIntoItemAtFrameAction->setData(QVariant(pasteIntoItemAtFrameDataObj));
				pNewMenu->addAction(pPasteIntoItemAtFrameAction);
			}
		}
		else // bCanPaste == false
		{
			pPasteAction->setEnabled(false);
			pNewMenu->addAction(pPasteAction);
		}
		
		pNewMenu->addSeparator();
		
		QAction *pSelectAllAction = new QAction("Select All KeyFrames");
		QJsonObject selectAllDataObj;
		selectAllDataObj.insert("contextAction", CONTEXTACTION_SelectAll);
		pSelectAllAction->setData(QVariant(selectAllDataObj));
		pNewMenu->addAction(pSelectAllAction);

		QAction *pSelectAllPriorAction = new QAction("Select KeyFrames <= " % QString::number(iContextFrameIndex));
		QJsonObject selectAllPriorDataObj;
		selectAllPriorDataObj.insert("contextAction", CONTEXTACTION_SelectAllPrior);
		selectAllPriorDataObj.insert("frame", iContextFrameIndex);
		pSelectAllPriorAction->setData(QVariant(selectAllPriorDataObj));
		pNewMenu->addAction(pSelectAllPriorAction);

		QAction *pSelectAllAfterAction = new QAction("Select KeyFrames >= " % QString::number(iContextFrameIndex));
		QJsonObject selectAllAfterDataObj;
		selectAllAfterDataObj.insert("contextAction", CONTEXTACTION_SelectAllAfter);
		selectAllAfterDataObj.insert("frame", iContextFrameIndex);
		pSelectAllAfterAction->setData(QVariant(selectAllAfterDataObj));
		pNewMenu->addAction(pSelectAllAfterAction);

		if(pContextItem)
		{
			QAction *pSelectAllItemAction = new QAction("Select All '" % pContextItem->GetCodeName() % "' KeyFrames");
			QJsonObject selectAllItemDataObj;
			selectAllItemDataObj.insert("contextAction", CONTEXTACTION_SelectAllItem);
			selectAllItemDataObj.insert("contextData", pContextItem->GetThisUuid().toString(QUuid::WithoutBraces));
			pSelectAllItemAction->setData(QVariant(selectAllItemDataObj));
			pNewMenu->addAction(pSelectAllItemAction);

			QAction *pSelectAllItemPriorAction = new QAction("Select '" % pContextItem->GetCodeName() % "' KeyFrames <= " % QString::number(iContextFrameIndex));
			QJsonObject selectAllItemPriorDataObj;
			selectAllItemPriorDataObj.insert("contextAction", CONTEXTACTION_SelectAllItemPrior);
			selectAllItemPriorDataObj.insert("frame", iContextFrameIndex);
			selectAllItemPriorDataObj.insert("contextData", pContextItem->GetThisUuid().toString(QUuid::WithoutBraces));
			pSelectAllItemPriorAction->setData(QVariant(selectAllItemPriorDataObj));
			pNewMenu->addAction(pSelectAllItemPriorAction);

			QAction *pSelectAllItemAfterAction = new QAction("Select '" % pContextItem->GetCodeName() % "' KeyFrames >= " % QString::number(iContextFrameIndex));
			QJsonObject selectAllItemAfterDataObj;
			selectAllItemAfterDataObj.insert("contextAction", CONTEXTACTION_SelectAllItemAfter);
			selectAllItemAfterDataObj.insert("frame", iContextFrameIndex);
			selectAllItemAfterDataObj.insert("contextData", pContextItem->GetThisUuid().toString(QUuid::WithoutBraces));
			pSelectAllItemAfterAction->setData(QVariant(selectAllItemAfterDataObj));
			pNewMenu->addAction(pSelectAllItemAfterAction);
		}

		QAction *pDeselectAllCallback = new QAction("Deselect KeyFrames");
		pDeselectAllCallback->setEnabled(pScene->selectedItems().size() > 0);
		QJsonObject deselectDataObj;
		deselectDataObj.insert("contextAction", CONTEXTACTION_DeselectAll);
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
		//ui->actionPasteFrames->setEnabled(bCanPaste);
		//ui->actionPasteOnFrame->setEnabled(bCanPaste);

		ui->actionDeleteFrames->setEnabled(selectedFrameItemList.size() > 0);
	}
	else
	{
		ui->sbFramesPerSecond->setEnabled(false);
		ui->chkAutoInitialize->setEnabled(false);

		ui->actionCopyFrames->setEnabled(false);
		//ui->actionPasteFrames->setEnabled(false);
		//ui->actionPasteOnFrame->setEnabled(false);
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
	int i = 0;
	i++;
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
	if(GetEntityStateModel() == nullptr)
		return;

	GetEntityStateModel()->GetDopeSheetScene().SetCurrentFrame(GetEntityStateModel()->GetDopeSheetScene().GetFinalFrame());
	UpdateWidgets();
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
			sDefaultName = dataObj["contextData"].toString();
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
		QString sCallbackName = dataObj["contextData"].toString();
		EntityUndoCmd_RemoveCallback *pNewCmd = new EntityUndoCmd_RemoveCallback(GetEntityStateModel()->GetDopeSheetScene(), iFrameIndex, sCallbackName);
		GetEntityStateModel()->GetModel().GetItem().GetUndoStack()->push(pNewCmd);
		break; }

	case CONTEXTACTION_TimeInsert: {
		int iFrameIndex = dataObj["frame"].toInt();
		int iMaxFrames = 16777215; // Uses 3 bytes (0xFFFFFF)... Qt uses this value for their default ranges in QSpinBox

		DlgInputNumber dlg("Insert empty frames starting at " % QString::number(iFrameIndex), "Frames", QIcon(":/icons16x16/time-stopwatch.png"), 1, 1, iMaxFrames, [](int iValue) { return ""; }, true);
		if(dlg.exec() == QDialog::Accepted)
		{
			GetEntityStateModel()->GetDopeSheetScene().SelectKeyFrames(false, nullptr, iFrameIndex, false);

			int iOffsetAmt = dlg.GetValue();
			EntityUndoCmd_NudgeSelectedKeyFrames *pNewCmd = new EntityUndoCmd_NudgeSelectedKeyFrames(GetEntityStateModel()->GetDopeSheetScene(), iOffsetAmt);
			GetEntityStateModel()->GetModel().GetItem().GetUndoStack()->push(pNewCmd);
		}
		break; }

	case CONTEXTACTION_TimeRemove: {
		int iFrameIndex = dataObj["frame"].toInt();
		int iMaxFrames = GetEntityStateModel()->GetDopeSheetScene().DetermineEmptyTimeFromFrame(iFrameIndex);
		if(iMaxFrames <= 0)
		{
			HyGuiLog("AuxDopeSheet::OnEventActionTriggered() - CONTEXTACTION_TimeRemove - Invalid pivot frame", LOGTYPE_Error);
			break;
		}
		DlgInputNumber dlg("Remove empty frames starting at " % QString::number(iFrameIndex), "Frames", QIcon(":/icons16x16/time-stopwatch.png"), iMaxFrames, 1, iMaxFrames, [](int iValue) { return ""; }, true);
		if(dlg.exec() == QDialog::Accepted)
		{
			GetEntityStateModel()->GetDopeSheetScene().SelectKeyFrames(false, nullptr, iFrameIndex, false);

			int iOffsetAmt = -dlg.GetValue();
			EntityUndoCmd_NudgeSelectedKeyFrames *pNewCmd = new EntityUndoCmd_NudgeSelectedKeyFrames(GetEntityStateModel()->GetDopeSheetScene(), iOffsetAmt);
			GetEntityStateModel()->GetModel().GetItem().GetUndoStack()->push(pNewCmd);
		}
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
	//	if(dataObj["contextData"].isString())
	//	{
	//		DopeSheetEvent dopeSheetEvent(dataObj["contextData"].toString());

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
	//	EntityUndoCmd_RemoveEvent *pNewCmd = new EntityUndoCmd_RemoveEvent(GetEntityStateModel()->GetDopeSheetScene(), dataObj["frame"].toInt(), dataObj["contextData"].toString());
	//	GetEntityStateModel()->GetModel().GetItem().GetUndoStack()->push(pNewCmd);
	//	break; }

	//case CONTEXTACTION_GotoState: {
	//	if(dataObj["contextData"].isString())
	//	{
	//		DopeSheetEvent dopeSheetEvent(dataObj["contextData"].toString());
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
	//	EntityUndoCmd_RemoveEvent *pNewCmd = new EntityUndoCmd_RemoveEvent(GetEntityStateModel()->GetDopeSheetScene(), dataObj["frame"].toInt(), dataObj["contextData"].toString());
	//	GetEntityStateModel()->GetModel().GetItem().GetUndoStack()->push(pNewCmd);
	//	break; }

	case CONTEXTACTION_Paste:
		PasteFrames(-1, nullptr);
		break;
				
	case CONTEXTACTION_PasteAtFrame:
		PasteFrames(dataObj["frame"].toInt(), nullptr);
		break;
		
	case CONTEXTACTION_PasteIntoItem:
		PasteFrames(-1, static_cast<EntityModel &>(GetEntityStateModel()->GetModel()).GetTreeModel().FindTreeItemData(QUuid(dataObj["contextData"].toString())));
		break;

	case CONTEXTACTION_PasteIntoItemAtFrame:
		PasteFrames(dataObj["frame"].toInt(), static_cast<EntityModel &>(GetEntityStateModel()->GetModel()).GetTreeModel().FindTreeItemData(QUuid(dataObj["contextData"].toString())));
		break;

	case CONTEXTACTION_SelectAll:
		GetEntityStateModel()->GetDopeSheetScene().SelectKeyFrames(QGuiApplication::queryKeyboardModifiers().testFlag(Qt::KeyboardModifier::ShiftModifier),
																	nullptr,
																	-1,
																	false);
		break;

	case CONTEXTACTION_SelectAllPrior:
		GetEntityStateModel()->GetDopeSheetScene().SelectKeyFrames(QGuiApplication::queryKeyboardModifiers().testFlag(Qt::KeyboardModifier::ShiftModifier), 
																	nullptr,
																	dataObj["frame"].toInt(),
																	true);
		break;

	case CONTEXTACTION_SelectAllAfter:
		GetEntityStateModel()->GetDopeSheetScene().SelectKeyFrames(QGuiApplication::queryKeyboardModifiers().testFlag(Qt::KeyboardModifier::ShiftModifier),
																	nullptr,
																	dataObj["frame"].toInt(),
																	false);
		break;

	case CONTEXTACTION_SelectAllItem:
		GetEntityStateModel()->GetDopeSheetScene().SelectKeyFrames(QGuiApplication::queryKeyboardModifiers().testFlag(Qt::KeyboardModifier::ShiftModifier),
																	static_cast<EntityModel &>(GetEntityStateModel()->GetModel()).GetTreeModel().FindTreeItemData(QUuid(dataObj["contextData"].toString())),
																	-1,
																	false);
		break;

	case CONTEXTACTION_SelectAllItemPrior:
		GetEntityStateModel()->GetDopeSheetScene().SelectKeyFrames(QGuiApplication::queryKeyboardModifiers().testFlag(Qt::KeyboardModifier::ShiftModifier),
																	static_cast<EntityModel &>(GetEntityStateModel()->GetModel()).GetTreeModel().FindTreeItemData(QUuid(dataObj["contextData"].toString())),
																	dataObj["frame"].toInt(),
																	true);
		break;

	case CONTEXTACTION_SelectAllItemAfter:
		GetEntityStateModel()->GetDopeSheetScene().SelectKeyFrames(QGuiApplication::queryKeyboardModifiers().testFlag(Qt::KeyboardModifier::ShiftModifier),
																	static_cast<EntityModel &>(GetEntityStateModel()->GetModel()).GetTreeModel().FindTreeItemData(QUuid(dataObj["contextData"].toString())),
																	dataObj["frame"].toInt(),
																	false);
		break;

	case CONTEXTACTION_DeselectAll:
		GetEntityStateModel()->GetDopeSheetScene().clearSelection();
		break;

	default:
		HyGuiLog("AuxDopeSheet::OnEventActionTriggered() - Unknown ContextActionType", LOGTYPE_Error);
		break;
	}

	UpdateWidgets();
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
	// Serialize all the selected items to the clipboard
	QJsonObject serializedKeyFramesObj = ui->graphicsView->GetScene()->SerializeSelectedKeyFrames();

	// Copy the serialized data to the clipboard
	EntityFrameMimeData *pFrameMimeData = new EntityFrameMimeData(serializedKeyFramesObj);
	QApplication::clipboard()->setMimeData(pFrameMimeData);
}

void AuxDopeSheet::on_actionDeleteFrames_triggered()
{
	// Serialize all the selected items to be deleted
	QJsonObject serializedKeyFramesObj = ui->graphicsView->GetScene()->SerializeSelectedKeyFrames();

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

void AuxDopeSheet::PasteFrames(int iStartFrameIndex, EntityTreeItemData *pContextTreeItemData)
{
	// Get the serialized data from the clipboard
	const QMimeData *pMimeData = QApplication::clipboard()->mimeData();
	if(false == pMimeData->hasFormat(HyGlobal::MimeTypeString(MIMETYPE_EntityFrames)))
	{
		HyGuiLog("AuxDopeSheet::PasteFrames() - pMimeData->hasFormat(MIMETYPE_EntityFrames) == false", LOGTYPE_Error);
		return;
	}
	const EntityFrameMimeData &entityFrameMimeDataRef = static_cast<const EntityFrameMimeData &>(*pMimeData);
	if(entityFrameMimeDataRef.IsValidForPaste() == false)
	{
		HyGuiLog("AuxDopeSheet::on_actionPasteFrames_triggered() - entityFrameMimeDataRef.IsValidForPaste() == false", LOGTYPE_Error);
		return;
	}

	QJsonDocument jsonDocument = QJsonDocument::fromJson(pMimeData->data(HyGlobal::MimeTypeString(MIMETYPE_EntityFrames)));
	QJsonObject keyFrameMimeObj = jsonDocument.object();


	QList<QPair<EntityTreeItemData *, QJsonArray>> pasteKeyFramesPairList;

	if(pContextTreeItemData)
	{
		// Determine if there's a priority frame array for 'pContextTreeItemData' (one that has copied frames from the same UUID)
		QJsonArray priorityFrameArray;
		for(QString sKey : keyFrameMimeObj.keys())
		{
			QUuid keyFrameUuid(sKey);
			if(pContextTreeItemData->GetThisUuid() == keyFrameUuid)
			{
				priorityFrameArray = keyFrameMimeObj[sKey].toArray();
				break;
			}
		}

		// Iterate through all the 'keyFrameMimeObj' properties in order, and only insert them into 'priorityFrameArray' if they don't already exist AND it's a compatible property
		for(QString sKey : keyFrameMimeObj.keys())
		{
			QUuid keyFrameUuid(sKey);
			if(pContextTreeItemData->GetThisUuid() == keyFrameUuid)
				continue;

			QJsonArray frameDataArray = keyFrameMimeObj[sKey].toArray();
			for(int iArrayIndex = 0; iArrayIndex < frameDataArray.size(); ++iArrayIndex)
			{
				bool bPastePropertyAccountedFor = false;
				QJsonObject pasteFrameObj = frameDataArray[iArrayIndex].toObject();
				int iPasteFrameIndex = pasteFrameObj["frame"].toInt();
				QJsonObject pastePropsObj = pasteFrameObj["props"].toObject();

				// Check if this frame index already exists in 'priorityFrameArray', if it does merge property in
				for(int iProrityFrameArrayIndex = 0; iProrityFrameArrayIndex < priorityFrameArray.size(); ++iProrityFrameArrayIndex)
				{
					QJsonObject priorityFrameObj = priorityFrameArray[iProrityFrameArrayIndex].toObject();
					int iPriorityFrameIndex = priorityFrameObj["frame"].toInt();
					if(iPriorityFrameIndex == iPasteFrameIndex)
					{
						// This frame exists, so make sure it's not overwriting anything in 'priorityFrameArray', but merge the rest
						QJsonObject priorityPropsObj = priorityFrameObj["props"].toObject();
						for(QString sCategory : pastePropsObj.keys())
						{
							QJsonObject pasteCategoryObj = pastePropsObj[sCategory].toObject();
							for(QString sPropName : pasteCategoryObj.keys())
							{
								if(priorityPropsObj.contains(sCategory) == false)
									priorityPropsObj.insert(sCategory, QJsonObject());
								QJsonObject priorityCategoryObj = priorityPropsObj[sCategory].toObject();
								if(priorityCategoryObj.contains(sPropName) == false)
									priorityCategoryObj.insert(sPropName, pasteCategoryObj[sPropName]);
								priorityPropsObj[sCategory] = priorityCategoryObj;
							}
						}
						priorityFrameObj["props"] = priorityPropsObj;
						priorityFrameArray[iProrityFrameArrayIndex] = priorityFrameObj;

						bPastePropertyAccountedFor = true;
						break;
					}
				}

				if(bPastePropertyAccountedFor == false)
					priorityFrameArray.append(pasteFrameObj);
			} // for(frameDataArray)
		} // for(keyFrameMimeObj.keys())

		pasteKeyFramesPairList.push_back(QPair<EntityTreeItemData *, QJsonArray>(pContextTreeItemData, priorityFrameArray));
	}
	else // if 'pContextTreeItemData' is nullptr, then try to divvy the paste 'keyFrameMimeObj' between all items
	{
		HyGuiLog("AuxDopeSheet::PasteFrames() - null destination item Unimplemented", LOGTYPE_Error);
	}

	EntityUndoCmd_PasteKeyFrames *pCmd = new EntityUndoCmd_PasteKeyFrames(*ui->graphicsView->GetScene(), pasteKeyFramesPairList, iStartFrameIndex);
	GetEntityStateModel()->GetModel().GetItem().GetUndoStack()->push(pCmd);
	
}
