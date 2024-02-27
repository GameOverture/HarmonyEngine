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
	EntityTreeItemData *	m_pContextTweenTreeItemData;
	int						m_iContextTweenStartFrame;
	int						m_iContextTweenEndFrame;

	enum ContextActionType
	{
		EVENTACTION_CallbackCreate = 0,
		EVENTACTION_CallbackRename,
		EVENTACTION_CallbackDelete,
		EVENTACTION_SelectAllItemKeyFrames,
		EVENTACTION_DeselectAllKeyFrames,
		EVENTACTION_PauseTimeline,
		EVENTACTION_UnpauseTimeline,
	};

public:
	explicit AuxDopeSheet(QWidget *pParent = nullptr);
	virtual ~AuxDopeSheet();

	EntityStateData *GetEntityStateModel() const;
	void SetEntityStateModel(EntityStateData *pEntStateData);

	void UpdateWidgets();

	QMenu *AllocContextMenu(bool bOnTimeline, EntityTreeItemData *pContextItem, int iContextFrameIndex);
	QAction *GetTweenAction();

private:
	Ui::AuxDopeSheet *ui;

private Q_SLOTS:
	void on_actionRewind_triggered();
	void on_actionPreviousKeyFrame_triggered();
	void on_actionPlayAnimations_triggered();
	void on_actionNextKeyFrame_triggered();
	void on_actionLastKeyFrame_triggered();
	
	void OnEventActionTriggered(QAction *pEventAction);

	void on_actionCreatePositionTween_triggered();
	void on_actionCreateRotationTween_triggered();
	void on_actionCreateScaleTween_triggered();
	void on_actionCreateAlphaTween_triggered();

	void on_actionCopyFrames_triggered();
	void on_actionPasteFrames_triggered();
	void on_actionDeleteFrames_triggered();

private:
	void CreateContextTween(TweenProperty eTweenProp);
};

#endif // WIDGETOUTPUTLOG_H
