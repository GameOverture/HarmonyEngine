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
#include <QClipboard>
#include <QApplication>

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

	ui->btnCopyKeyFrames->setDefaultAction(ui->actionCopyFrames);
	ui->btnPasteKeyFrames->setDefaultAction(ui->actionPasteFrames);
	ui->btnDeleteKeyFrames->setDefaultAction(ui->actionDeleteFrames);

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
		dataObj.insert("type", EVENTACTION_CallbackCreate);
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
			dataObj.insert("type", EVENTACTION_CallbackRename);
			dataObj.insert("frame", iContextFrameIndex);
			dataObj.insert("name", sCallback);
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
			dataObj.insert("type", EVENTACTION_CallbackDelete);
			dataObj.insert("frame", iContextFrameIndex);
			dataObj.insert("name", sCallback);
			pDeleteCallback->setData(QVariant(dataObj));
			pDeleteCallback->setToolTip("Removes the callback function at the currently selected frame");
			pNewMenu->addAction(pDeleteCallback);
		}
		pNewMenu->addSeparator();

		// Pause Timeline
		QList<DopeSheetEvent> eventList = pScene->GetEventList(iContextFrameIndex);

		bool bHasPauseEvent = false;
		for(DopeSheetEvent dopeEvent : eventList)
		{
			if(dopeEvent.m_eType == DOPEEVENT_PauseTimeline)
			{
				QAction *pUnpauseTimeline = new QAction(QIcon(":/icons16x16/media-play.png"),
					"Unpause Timeline On Frame " % QString::number(iContextFrameIndex));
				QJsonObject pauseDataObj;
				pauseDataObj.insert("type", EVENTACTION_UnpauseTimeline);
				pauseDataObj.insert("frame", iContextFrameIndex);
				pUnpauseTimeline->setData(QVariant(pauseDataObj));
				pUnpauseTimeline->setToolTip("Unpause the timeline at the currently selected frame");
				pNewMenu->addAction(pUnpauseTimeline);

				bHasPauseEvent = true;
			}
		}

		if(bHasPauseEvent == false)
		{
			QAction *pPauseTimeline = new QAction(QIcon(":/icons16x16/media-pause.png"),
				"Pause Timeline On Frame " % QString::number(iContextFrameIndex));
			QJsonObject pauseDataObj;
			pauseDataObj.insert("type", EVENTACTION_PauseTimeline);
			pauseDataObj.insert("frame", iContextFrameIndex);
			pPauseTimeline->setData(QVariant(pauseDataObj));
			pPauseTimeline->setToolTip("Pause the timeline at the currently selected frame");
			pNewMenu->addAction(pPauseTimeline);
		}
	}
	else // Context not on timeline
	{
		// Copy/Paste Frames actions
		pNewMenu->addAction(ui->actionCopyFrames);
		pNewMenu->addAction(ui->actionPasteFrames);
		pNewMenu->addSeparator();

		QString sSelectAll;
		if(pContextItem)
			sSelectAll = "Select All '" % pContextItem->GetCodeName() % "' Key Frames";
		else
			sSelectAll = "Select All Key Frames";
		QAction *pSelectAllCallback = new QAction(sSelectAll);
		pSelectAllCallback->setEnabled(pContextItem != nullptr);
		QJsonObject selectDataObj;
		selectDataObj.insert("type", EVENTACTION_SelectAllItemKeyFrames);
		pSelectAllCallback->setData(QVariant(selectDataObj));
		pNewMenu->addAction(pSelectAllCallback);

		QAction *pDeselectAllCallback = new QAction("Deselect Key Frames");
		pDeselectAllCallback->setEnabled(pScene->selectedItems().size() > 0);
		QJsonObject deselectDataObj;
		deselectDataObj.insert("type", EVENTACTION_DeselectAllKeyFrames);
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
	if(ui->graphicsView->scene())
	{
		ui->sbFramesPerSecond->setEnabled(true);
		ui->chkAutoInitialize->setEnabled(true);

		EntityDopeSheetScene &dopeSheetSceneRef = GetEntityStateModel()->GetDopeSheetScene();

		// Determine if a quick-tween button should be shown
		TweenProperty eQuickTweenProp = dopeSheetSceneRef.DetermineIfContextQuickTween(m_pContextTweenTreeItemData, m_iContextTweenStartFrame, m_iContextTweenEndFrame);
		if(eQuickTweenProp != TWEENPROP_None)
		{
			ui->btnTween->setVisible(true);
			switch(eQuickTweenProp)
			{
			case TWEENPROP_Position:	ui->btnTween->setDefaultAction(ui->actionCreatePositionTween); break;
			case TWEENPROP_Rotation:	ui->btnTween->setDefaultAction(ui->actionCreateRotationTween); break;
			case TWEENPROP_Scale:		ui->btnTween->setDefaultAction(ui->actionCreateScaleTween); break;
			case TWEENPROP_Alpha:		ui->btnTween->setDefaultAction(ui->actionCreateAlphaTween); break;
			default:
				HyGuiLog("AuxDopeSheet::UpdateWidgets - Unknown TweenProperty", LOGTYPE_Error);
				break;
			}
		}
		else
		{
			m_pContextTweenTreeItemData = nullptr;
			m_iContextTweenStartFrame = m_iContextTweenEndFrame = -1;
			ui->btnTween->setDefaultAction(nullptr);
			ui->btnTween->setVisible(false);
		}
		
		QList<EntityTreeItemData *> selectedFrameItemList = ui->graphicsView->GetScene()->GetItemsFromSelectedFrames();
		ui->actionCopyFrames->setEnabled(selectedFrameItemList.size() == 1);

		const QMimeData *pMimeData = QApplication::clipboard()->mimeData();
		ui->actionPasteFrames->setEnabled(ui->graphicsView->GetContextClickItem() && pMimeData->hasFormat(HyGlobal::MimeTypeString(MIMETYPE_EntityFrames)));

		ui->actionDeleteFrames->setEnabled(selectedFrameItemList.size() > 0);
	}
	else
	{
		m_pContextTweenTreeItemData = nullptr;
		m_iContextTweenStartFrame = m_iContextTweenEndFrame = -1;
		ui->btnTween->setDefaultAction(nullptr);
		ui->btnTween->setVisible(false);

		ui->sbFramesPerSecond->setEnabled(false);
		ui->chkAutoInitialize->setEnabled(false);

		ui->actionCopyFrames->setEnabled(false);
		ui->actionPasteFrames->setEnabled(false);
		ui->actionDeleteFrames->setEnabled(false);
	}
}

QAction *AuxDopeSheet::GetTweenAction()
{
	if(ui->btnTween->isVisible())
		return ui->btnTween->defaultAction();

	return nullptr;
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
	{
		HyGuiLog("AuxDopeSheet::OnEventActionTriggered() - dataObj.isEmpty()", LOGTYPE_Error);
		return;
	}

	ContextActionType eActionType = static_cast<ContextActionType>(dataObj["type"].toInt());
	switch(eActionType)
	{
	case EVENTACTION_CallbackCreate:
	case EVENTACTION_CallbackRename: {
		int iFrameIndex = dataObj["frame"].toInt();

		QString sDefaultName;
		QString sTitle;
		if(eActionType == EVENTACTION_CallbackRename)
		{
			sDefaultName = dataObj["name"].toString();
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
			if(eActionType == EVENTACTION_CallbackCreate)
			{
				EntityUndoCmd_AddEvent *pNewCmd = new EntityUndoCmd_AddEvent(GetEntityStateModel()->GetDopeSheetScene(), iFrameIndex, dlg.GetName());
				GetEntityStateModel()->GetModel().GetItem().GetUndoStack()->push(pNewCmd);
			}
			else // EVENTACTION_CallbackRename
			{
				EntityUndoCmd_RenameCallback *pNewCmd = new EntityUndoCmd_RenameCallback(GetEntityStateModel()->GetDopeSheetScene(), iFrameIndex, sDefaultName, dlg.GetName());
				GetEntityStateModel()->GetModel().GetItem().GetUndoStack()->push(pNewCmd);
			}
		}
		break; }
	
	case EVENTACTION_CallbackDelete: {
		int iFrameIndex = dataObj["frame"].toInt();
		QString sCallbackName = dataObj["name"].toString();
		EntityUndoCmd_RemoveEvent *pNewCmd = new EntityUndoCmd_RemoveEvent(GetEntityStateModel()->GetDopeSheetScene(), iFrameIndex, sCallbackName);
		GetEntityStateModel()->GetModel().GetItem().GetUndoStack()->push(pNewCmd);
		break; }

	case EVENTACTION_SelectAllItemKeyFrames:
		GetEntityStateModel()->GetDopeSheetScene().SelectAllItemKeyFrames(ui->graphicsView->GetContextClickItem());
		break;

	case EVENTACTION_DeselectAllKeyFrames:
		GetEntityStateModel()->GetDopeSheetScene().clearSelection();
		break;

	case EVENTACTION_PauseTimeline: {
		EntityUndoCmd_AddEvent *pNewCmd = new EntityUndoCmd_AddEvent(GetEntityStateModel()->GetDopeSheetScene(), dataObj["frame"].toInt(), "_PauseTimeline");
		GetEntityStateModel()->GetModel().GetItem().GetUndoStack()->push(pNewCmd);
		break; }

	case EVENTACTION_UnpauseTimeline: {
		EntityUndoCmd_RemoveEvent *pNewCmd = new EntityUndoCmd_RemoveEvent(GetEntityStateModel()->GetDopeSheetScene(), dataObj["frame"].toInt(), "_PauseTimeline");
		GetEntityStateModel()->GetModel().GetItem().GetUndoStack()->push(pNewCmd);
		break; }

	default:
		HyGuiLog("AuxDopeSheet::OnEventActionTriggered() - Unknown ContextActionType", LOGTYPE_Error);
		break;
	}
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
																			  serializedKeyFrameObj);
		GetEntityStateModel()->GetModel().GetItem().GetUndoStack()->push(pCmd);
	}
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
	EntityUndoCmd_ConvertToTween *pNewCmd = new EntityUndoCmd_ConvertToTween(GetEntityStateModel()->GetDopeSheetScene(),
																			 m_pContextTweenTreeItemData,
																			 eTweenProp,
																			 m_iContextTweenStartFrame,
																			 m_iContextTweenEndFrame);
	GetEntityStateModel()->GetModel().GetItem().GetUndoStack()->push(pNewCmd);
}
