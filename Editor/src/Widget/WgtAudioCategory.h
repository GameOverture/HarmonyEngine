/**************************************************************************
 *	WgtAudioCategory.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2022 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef WGTAUDIOCATEGORY_H
#define WGTAUDIOCATEGORY_H

#include <QWidget>

class AudioCategoriesModel;

namespace Ui {
class WgtAudioCategory;
}

class WgtAudioCategory : public QWidget
{
	Q_OBJECT
	
public:
	explicit WgtAudioCategory(QWidget *parent = 0);
	~WgtAudioCategory();

	void Init(AudioCategoriesModel *pModel, quint32 uiId, bool bIncludeCheckBox);

	bool IsValid() const;
	quint32 GetCurrentId() const;
	
private Q_SLOTS:
	void on_cmbAudioGroup_currentIndexChanged(int index);
	void on_btnAddGroup_clicked();
	void on_btnRemoveGroup_clicked();

private:
	Ui::WgtAudioCategory *ui;
};

#endif // WGTAUDIOCATEGORY_H
