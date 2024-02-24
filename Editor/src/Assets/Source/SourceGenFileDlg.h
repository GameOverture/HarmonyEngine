/**************************************************************************
 *	SourceGenFileDlg.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2021 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef SourceGenFileDlg_H
#define SourceGenFileDlg_H

#include "Global.h"
#include "Project.h"

#include <QDialog>

namespace Ui {
class SourceGenFileDlg;
}

class SourceGenFileDlg : public QDialog
{
	Q_OBJECT

	Ui::SourceGenFileDlg *		ui;

public:
	SourceGenFileDlg(QStringList sEditorEntityList, QWidget *pParent = nullptr);
	virtual ~SourceGenFileDlg();

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

#endif // SourceGenFileDlg_H
