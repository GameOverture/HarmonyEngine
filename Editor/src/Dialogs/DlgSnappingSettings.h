/**************************************************************************
 *	DlgSnappingSettings.h
 *
 *	Harmony Engine - HyEditor
 *	Copyright (c) 2023 Jason Knobler
 *
 *	GPL/LGPL License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/main/Editor/LICENSE
 *************************************************************************/
#ifndef DLGSNAPPINGSETTINGS_H
#define DLGSNAPPINGSETTINGS_H

#include <QDialog>

class IManagerModel;

namespace Ui {
class DlgSnappingSettings;
}

class DlgSnappingSettings : public QDialog
{
	Q_OBJECT

public:
	explicit DlgSnappingSettings(uint32 uiSnappingSettings, QWidget *pParent = nullptr);
	~DlgSnappingSettings();

	uint32 GetSnappingSettings() const;

private Q_SLOTS:
	virtual void done(int r);
	void on_chkEnableSnapping_clicked();

private:
	Ui::DlgSnappingSettings *ui;
};

#endif // DLGSNAPPINGSETTINGS_H
