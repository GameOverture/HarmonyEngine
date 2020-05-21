/**************************************************************************
 *	WgtMakeRelDir.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2020 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef WgtMakeProjectDir_H
#define WgtMakeProjectDir_H

#include <QWidget>

namespace Ui {
class WgtMakeRelDir;
}

class WgtMakeRelDir : public QWidget
{
	Q_OBJECT

	QString					m_sTitle;
	QString					m_sAbsProjPath;

	QString					m_sAbsParentDirPath;	// The absolute path to the parent directory of the new dir will reside, but not the directory itself
	
public:
	explicit WgtMakeRelDir(QWidget *parent = 0);
	~WgtMakeRelDir();

	void Setup(QString sTitle, QString sDefaultName, QString sAbsProjectPath);

	QString GetRelPath() const;

	void Refresh();
	QString GetError();
	
private Q_SLOTS:
	void on_btnBrowseDir_clicked();
	void on_txtDirName_textChanged(const QString &arg1);

private:
	Ui::WgtMakeRelDir *ui;

Q_SIGNALS:
	void OnDirty();
};

#endif // WgtMakeProjectDir_H
