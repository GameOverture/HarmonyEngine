/**************************************************************************
 *	WgtSrcDependency.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2020 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef WgtSrcDependency_H
#define WgtSrcDependency_H

#include <QWidget>

namespace Ui {
class WgtSrcDependency;
}

class SourceSettingsDlg;

class WgtSrcDependency : public QWidget
{
	Q_OBJECT

	SourceSettingsDlg *		m_pParentDlg;
	QString					m_sAbsSrcDepPath;
	
public:
	explicit WgtSrcDependency(SourceSettingsDlg *pParentDlg, QWidget *pParent = nullptr);
	virtual ~WgtSrcDependency();

	QString GetProjectName() const;
	QString GetAbsPath() const;
	QString GetOptions() const;

	void Set(QString sProjectName, QString sDependAbsPath, QString sOptions);

	void Refresh();

	QString GetError();
	
private Q_SLOTS:
	void on_btnRemoveDependency_clicked();
	void on_btnBrowseDir_clicked();
	void on_txtProjectName_textChanged(const QString &arg1);

private:
	Ui::WgtSrcDependency *ui;

Q_SIGNALS:
	void OnDirty();
};

#endif // WgtSrcDependency_H
