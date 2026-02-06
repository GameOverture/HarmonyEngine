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

		CONTEXTACTION_TimeInsert,			// All tweens active on this frame have their durations extended, and all frames beyond this one are offset
		CONTEXTACTION_TimeRemove,			// All tweens active on this frame have their durations shortened, and all frames beyond this one are offset

		CONTEXTACTION_PauseTimeline,
		CONTEXTACTION_UnpauseTimeline,
		CONTEXTACTION_GotoFrame,
		CONTEXTACTION_RemoveGotoFrame,
		CONTEXTACTION_GotoState,
		CONTEXTACTION_RemoveGotoState,

		CONTEXTACTION_Paste,				// Paste frames at their copied frame index - if keyframe items are not 1:1 it will use best guess
		CONTEXTACTION_PasteAtFrame,			// Paste frames starting at the specified frame index - if keyframe items are not 1:1 it will use best guess
		CONTEXTACTION_PasteIntoItem,		// Paste frames at their copied frame index into the specified item - if the copied frames don't have a matching item, it will use best guess
		CONTEXTACTION_PasteIntoItemAtFrame,	// Paste frames starting at the specified frame index into the specified item - if the copied frames don't have a matching item, it will use best guess

		CONTEXTACTION_SelectAll,			// Select every keyframe in the entire dope sheet
		CONTEXTACTION_SelectAllPrior,		// Select every keyframe at the specified index and all prior
		CONTEXTACTION_SelectAllAfter,		// Select every keyframe at the specified index and all after
		CONTEXTACTION_SelectAllItem,		// Select every keyframe for the specified item
		CONTEXTACTION_SelectAllItemPrior,	// Select every keyframe for the specified item at the specified index and all prior
		CONTEXTACTION_SelectAllItemAfter,	// Select every keyframe for the specified item at the specified index and all after
		CONTEXTACTION_DeselectAll,
	};

public:
	explicit AuxDopeSheet(QWidget *pParent = nullptr);
	virtual ~AuxDopeSheet();

	EntityStateData *GetEntityStateModel() const;
	void SetEntityStateModel(EntityStateData *pEntStateData);

	void UpdateWidgets(); // EntityWidget::OnFocusState() will call this

	QMenu *AllocContextMenu(bool bOnTimeline, EntityTreeItemData *pContextItem, int iContextFrameIndex);

	void EnsureSelectedFrameVisible(); // Scrolls the graphics view to ensure the current frame is visible

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
	void on_actionCutFrames_triggered();
	void on_actionDeleteFrames_triggered();

private:
	void CreateContextTween(TweenProperty eTweenProp);

	void PasteFrames(int iStartFrameIndex, EntityTreeItemData *pContextTreeItemData); // -1 means don't offset pasted frames, pContextTreeItemData can be nullptr and it tries to best guess
};

#endif // WIDGETOUTPUTLOG_H
