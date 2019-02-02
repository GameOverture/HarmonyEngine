/**************************************************************************
 *	DlgSetEngineLocation.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef DLGOPENPROJECT_H
#define DLGOPENPROJECT_H

#include <QDialog>
#include <QFileSystemModel>
#include <QShowEvent>
#include <QUndoStack>

namespace Ui {
class DlgSetEngineLocation;
}

class DlgSetEngineLocationUndoCmd : public QUndoCommand
{
	QString	&	m_sDirPathRef;
	QString		m_sOld;
	QString		m_sNew;

public:
	DlgSetEngineLocationUndoCmd(QString &sDirPathRef, QString sNewDirPath) :
		QUndoCommand(sNewDirPath),
		m_sDirPathRef(sDirPathRef),
		m_sOld(sDirPathRef),
		m_sNew(sNewDirPath)
	{ }

	virtual void undo() override
	{ m_sDirPathRef = m_sOld; }
	virtual void redo() override
	{ m_sDirPathRef = m_sNew; }
};

class DlgSetEngineLocation : public QDialog
{
	Q_OBJECT

	QString				m_sDirPath;
	
	QFileSystemModel *	m_pFileModel;
	QUndoStack			m_DirPathUndoStack;

public:
	explicit DlgSetEngineLocation(QWidget *parent = 0);
	~DlgSetEngineLocation();
	
	QString SelectedDir();
	
private Q_SLOTS:
	void on_listView_doubleClicked(const QModelIndex &index);
	
	void on_txtCurDirectory_editingFinished();
	
	//void on_listView_clicked(const QModelIndex &index);

	void on_btnBack_clicked();

	void on_btnForward_clicked();

	void on_btnUp_clicked();
	
private:
	Ui::DlgSetEngineLocation *ui;

	void ErrorCheck();
};

#endif // DLGOPENPROJECT_H
