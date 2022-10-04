/**************************************************************************
 *	WgtAudioGroup.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2022 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef WGTAUDIOGROUP_H
#define WGTAUDIOGROUP_H

#include <QWidget>

class AudioGroupsModel;

namespace Ui {
class WgtAudioGroup;
}

class WgtAudioGroup : public QWidget
{
	Q_OBJECT
	
public:
	explicit WgtAudioGroup(QWidget *parent = 0);
	~WgtAudioGroup();

	void Init(AudioGroupsModel *pModel, quint32 uiId, bool bIncludeCheckBox);

	bool IsValid() const;
	quint32 GetCurrentId() const;
	
private Q_SLOTS:
	void on_cmbAudioGroup_currentIndexChanged(int index);
	void on_btnAddGroup_clicked();
	void on_btnRemoveGroup_clicked();

private:
	Ui::WgtAudioGroup *ui;
};

#endif // WIDGETCOLOR_H
