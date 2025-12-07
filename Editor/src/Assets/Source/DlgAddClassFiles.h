/**************************************************************************
 *	DlgAddClassFiles.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2021 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef DlgAddClassFiles_H
#define DlgAddClassFiles_H

#include "Global.h"
#include "Project.h"

#include <QDialog>

namespace Ui {
class DlgAddClassFiles;
}

class DlgAddClassFiles : public QDialog
{
	Q_OBJECT

	Ui::DlgAddClassFiles *		ui;

public:
	DlgAddClassFiles(QStringList sEditorEntityList, QWidget *pParent = nullptr);
	virtual ~DlgAddClassFiles();

	QString GetCodeClassName() const;
	QString GetHeaderFileName() const;
	QString GetCppFileName() const;
	QString GetBaseClassName() const;
	bool IsEntityBaseClass() const;

protected Q_SLOTS:
	void on_txtClassName_textChanged(const QString &arg1);
	void on_txtHFile_textChanged(const QString &arg1);
	void on_txtCppFile_textChanged(const QString &arg1);
	void on_txtBaseClass_textChanged(const QString &arg1);

	void on_radBaseSpecify_toggled(bool bChecked);
	void on_radBaseEditor_toggled(bool bChecked);

	void on_editorEntitiesList_itemSelectionChanged();

private:
	void ErrorCheck();
};

#endif // DlgAddClassFiles_H
