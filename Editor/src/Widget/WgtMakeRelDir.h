/**************************************************************************
 *	WgtMakeRelDir.h
 *
 *	Harmony Engine - HyEditor
 *	Copyright (c) 2020 Jason Knobler
 *
 *	GPL/LGPL License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/main/Editor/LICENSE
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

	QString					m_sAbsBasePath; // AKA the project location

	bool					m_bMustBeEmptyFolder;
	
public:
	explicit WgtMakeRelDir(QWidget *parent = 0);
	~WgtMakeRelDir();

	void Setup(QString sTitle, QString sAbsBasePath, QString sDefaultRelativePath, bool bMustBeEmptyFolder);

	QString GetRelPath() const;
	QString GetAbsPath() const;

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
