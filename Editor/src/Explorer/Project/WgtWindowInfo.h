/**************************************************************************
 *	WgtWindowInfo.h
 *
 *	Harmony Engine - HyEditor
 *	Copyright (c) 2025 Jason Knobler
 *
 *	GPL/LGPL License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/main/Editor/LICENSE
 *************************************************************************/
#ifndef WgtWindowInfo_H
#define WgtWindowInfo_H

#include <QWidget>

namespace Ui {
class WgtWindowInfo;
}

class DlgProjectSettings;

class WgtWindowInfo : public QWidget
{
	Q_OBJECT

	DlgProjectSettings *		m_pParentDlg;
	
public:
	explicit WgtWindowInfo(DlgProjectSettings *pParentDlg, QJsonObject windowInfoObj, QWidget *pParent = nullptr);
	virtual ~WgtWindowInfo();

	QJsonObject SerializeWidgets();
	
private Q_SLOTS:
	void on_btnRemoveWindow_clicked();

private:
	Ui::WgtWindowInfo *ui;
};

#endif // WgtWindowInfo_H
