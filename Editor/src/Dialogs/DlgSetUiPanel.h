/**************************************************************************
 *	DlgSetUiPanel.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2025 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef DLGSETUIPANEL_H
#define DLGSETUIPANEL_H

#include <QDialog>

namespace Ui {
class DlgSetUiPanel;
}

class DlgSetUiPanel : public QDialog
{
	Q_OBJECT
	
	Project	&			m_ProjectRef;
	HyUiPanelInit		m_Init;
	QUuid				m_SelectedNodeUuid;

public:
	explicit DlgSetUiPanel(Project &projectRef, QString sTitle, HyUiPanelInit &init, QUuid selectedNodeUuid, QWidget *parent = 0);
	~DlgSetUiPanel();
	
	QVariant GetSerializedPanelInit() const;

protected:
	void SyncNodeComboBox();
	void SyncWidgets();

private Q_SLOTS:
	void on_sbPrimFrame_valueChanged(int arg1);
	void on_primPanelColor_clicked();

private:
	Ui::DlgSetUiPanel *ui;
};

#endif // DLGSETUIPANEL_H
