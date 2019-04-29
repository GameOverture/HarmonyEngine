/**************************************************************************
 *	ExplorerItem.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef EXPLORERITEM_H
#define EXPLORERITEM_H

#include <QObject>
#include <QTreeWidget>

#include "Global.h"
#include "Shared/TreeModel/ITreeModelItem.h"

class ExplorerItem : public ITreeModelItem
{
	Q_OBJECT

protected:
	const HyGuiItemType		m_eTYPE;
	QString					m_sPath;

	Project *				m_pProject;
	bool					m_bIsProjectItem;
	
public:
	ExplorerItem();
	ExplorerItem(Project &projectRef, HyGuiItemType eType, const QString sPath);
	virtual ~ExplorerItem();

	HyGuiItemType GetType() const;
	Project &GetProject() const;

	bool IsProjectItem() const;
	
	QString GetName(bool bWithPrefix) const;
	QString GetPrefix() const;				// Ends with a '/'
	QIcon GetIcon(SubIcon eSubIcon) const;

	virtual void Rename(QString sNewName);

	virtual QString GetToolTip() const override;
};
//Q_DECLARE_METATYPE(ExplorerItem *)
//
//QDataStream &operator<<(QDataStream &out, ExplorerItem *const &rhs);
//QDataStream &operator>>(QDataStream &in, ExplorerItem *rhs);

#endif // EXPLORERITEM_H
