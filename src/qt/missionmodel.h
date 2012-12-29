#ifndef MISSIONMODEL_H
#define MISSIONMODEL_H

#ifdef SL_QT_MODE

#include <QAbstractListModel>
#include <QList>

struct Mission //: public QObject
{

    QString title;
    QString script;
    QString intro;
    QString image;
};

class MissionModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit MissionModel(QObject *parent = 0);
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    enum Roles {
                     Intro = Qt::UserRole + 1,
                     Script,Image,Title
             };
signals:
    
public slots:
//    QString title(int index) const;
//    QString script(int index) const;
//    QString intro(int index) const;

private:
    typedef QList<Mission>
        ContainerType;
    ContainerType m_missions;
};

#endif //#ifdef SL_QT_MODE
#endif // MISSIONMODEL_H
