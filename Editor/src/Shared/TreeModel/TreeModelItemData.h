/**************************************************************************
*	TreeModelItemData.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2020 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef TREEMODELITEMDATA_H
#define TREEMODELITEMDATA_H

#include "Global.h"

#include <QObject>
#include <QTreeWidget>

class TreeModelItemData : public QObject
{
	Q_OBJECT

protected:
	const HyGuiItemType		m_eTYPE;
	QString					m_sName;
	bool					m_bIsProjectItem;

public:
	TreeModelItemData();
	TreeModelItemData(HyGuiItemType eType, const QString sText);
	virtual ~TreeModelItemData();

	HyGuiItemType GetType() const;
	QString GetText() const;
	void SetText(QString sText);
	QIcon GetIcon(SubIcon eSubIcon) const;

	bool IsProjectItem() const;
};
Q_DECLARE_METATYPE(TreeModelItemData *)
Q_DECLARE_METATYPE(const TreeModelItemData *)

QDataStream &operator<<(QDataStream &out, TreeModelItemData *const &rhs);
QDataStream &operator>>(QDataStream &in, TreeModelItemData *rhs);

#endif // TREEMODELITEMDATA_H
