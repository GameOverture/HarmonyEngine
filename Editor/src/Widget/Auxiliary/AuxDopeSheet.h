/**************************************************************************
 *	AuxDopeSheet.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2023 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef AuxDopeSheet_H
#define AuxDopeSheet_H

#include <QWidget>
#include <QDataWidgetMapper>

class EntityTreeItemData;
class EntityStateData;

namespace Ui {
class AuxDopeSheet;
}

class AuxDopeSheet : public QWidget
{
	Q_OBJECT

	QDataWidgetMapper		m_WidgetMapper;

	// These are actions the user can take to manipulate the timeline within the editor.
	// These are not used to serialize actions to be used during runtime
	enum ContextActionType
	{
		CONTEXTACTION_CallbackCreate = 0,
		CONTEXTACTION_CallbackRename,
		CONTEXTACTION_CallbackDelete,
		CONTEXTACTION_PauseTimeline,
		CONTEXTACTION_UnpauseTimeline,
		CONTEXTACTION_GotoFrame,
		CONTEXTACTION_RemoveGotoFrame,
		CONTEXTACTION_GotoState,
		CONTEXTACTION_RemoveGotoState,

		CONTEXTACTION_SelectAllItemKeyFrames,
		CONTEXTACTION_DeselectAllKeyFrames,
	};

public:
	explicit AuxDopeSheet(QWidget *pParent = nullptr);
	virtual ~AuxDopeSheet();

	EntityStateData *GetEntityStateModel() const;
	void SetEntityStateModel(EntityStateData *pEntStateData);

	void UpdateWidgets();

	QMenu *AllocContextMenu(bool bOnTimeline, EntityTreeItemData *pContextItem, int iContextFrameIndex);

private:
	Ui::AuxDopeSheet *ui;

private Q_SLOTS:
	void on_actionRewind_triggered();
	void on_actionPreviousKeyFrame_triggered();
	void on_actionPlayAnimations_triggered();
	void on_actionNextKeyFrame_triggered();
	void on_actionLastKeyFrame_triggered();
	
	void OnEventActionTriggered(QAction *pEventAction);

	void on_actionBreakTween_triggered();
	void on_actionCreatePositionTween_triggered();
	void on_actionCreateRotationTween_triggered();
	void on_actionCreateScaleTween_triggered();
	void on_actionCreateAlphaTween_triggered();

	void on_actionCopyFrames_triggered();
	void on_actionPasteFrames_triggered();
	void on_actionPasteOnFrame_triggered();
	void on_actionDeleteFrames_triggered();

private:
	void CreateContextTween(TweenProperty eTweenProp);

	void PasteFrames(int iStartFrameIndex); // -1 means don't offset pasted frames
};

#endif // WIDGETOUTPUTLOG_H
