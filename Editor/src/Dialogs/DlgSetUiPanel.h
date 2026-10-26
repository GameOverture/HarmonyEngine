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

#include "Global.h"
#include "HyEngine.h"

#include <QDialog>

class QButton;

namespace Ui {
class DlgSetUiPanel;
}

class DlgSetUiPanel : public QDialog
{
	Q_OBJECT
	
	Project	&			m_ProjectRef;
	HyUiPanelInit		m_Init;
	QUuid				m_NodeUuid; // The UUID of the referenced node, when set as 'NodeItem' panel type

	enum StackedPage
	{
		PAGE_BoundingVolume = 0,
		PAGE_Primitive,
		PAGE_Node
	};

public:
	explicit DlgSetUiPanel(Project &projectRef, QString sTitle, HyUiPanelInit &init, QUuid nodeUuid, QWidget *parent = 0);
	~DlgSetUiPanel();

	HyUiPanelInit GetPanelInit();
	QUuid GetNodeUuid();
	
	static QVariant SerializePanelInit(const HyUiPanelInit &init, QUuid nodeUuid);
	static HyUiPanelInit DeserializePanelInit(const QJsonObject &serializedObj, QUuid &nodeUuidOut);

protected:
	void SyncNodeComboBox();
	void SyncWidgets();

private Q_SLOTS:
	void on_radBoundingVolume_toggled(bool bChecked);
	void on_radPrimitive_toggled(bool bChecked);
	void on_radNode_toggled(bool bChecked);

	void OnSizeChanged(QVariant newSize);

	void on_sbPrimFrame_valueChanged(int arg1);
	void on_btnPanelColor_clicked();
	void on_btnFrameColor_clicked();
	void on_btnTertiaryColor_clicked();

	void on_cmbNode_currentIndexChanged(int index);

private:
	Ui::DlgSetUiPanel *ui;

	HyColor SetPrimColor(HyColor startingColor, QPushButton *pButton, bool bShowDialog);
};

#endif // DLGSETUIPANEL_H
