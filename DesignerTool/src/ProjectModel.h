#ifndef PROJECTMODEL_H
#define PROJECTMODEL_H

#include <QObject>
#include <QAbstractListModel>

class ProjectModel : public QAbstractListModel
{
    Q_OBJECT

public:
    ProjectModel();
};

#endif // PROJECTMODEL_H
