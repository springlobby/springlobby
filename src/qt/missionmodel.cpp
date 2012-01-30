#include "missionmodel.h"
#include "springunitsync.h"
#include "customizations.h"

#include <boost/algorithm/string.hpp>

MissionModel::MissionModel(QObject *parent) :
    QAbstractListModel(parent)
{
    wxString fn = _T("Mission/*.txt");
    wxArrayString res = usync().FindFilesVFS( fn );

    foreach( fn, res ) {
        wxString content = usync().GetTextfileAsString(  SLcustomizations().GetModname(), fn );
        wxString title = content.BeforeFirst( '\n' );
        wxString intro = content.AfterFirst( '\n' );
        wxString image = fn;
        image.Replace( _T(".txt"), _T(".png") );
        wxString script = fn;
        script.Replace( _T(".txt"), _T(".script") );
        Mission m;
        m.intro = ToQString( intro );
        m.title = ToQString( title );
        m.script = ToQString( script );
        m.image = ToQString( image );
        m_missions.append( m );
    }

    QHash<int, QByteArray> roles;
    roles[Intro] = "intro";
    roles[Script] = "script";
    roles[Image] = "image";
    roles[Title] = "title";
    setRoleNames(roles);
}

int MissionModel::rowCount(const QModelIndex &/*parent*/) const
{
    return m_missions.size();
}

QVariant MissionModel::data(const QModelIndex &index, int role) const
{
    int row =  index.row();
    if ( /*!index.isValid() || */row >= m_missions.size()/* || !(m_missions[row])*/ )
        return QVariant();
    const Mission& mission = m_missions[row];
    switch ( role ) {
        case Intro:
            return QVariant::fromValue( mission.intro );
        case Image:
            return QVariant::fromValue( mission.image );
        default:
        case Title:
            return QVariant::fromValue( mission.title );
        case Script:
            return QVariant::fromValue( mission.script );
    }
}

//QString MissionModel::title(int index) const
//{
//}

//QString MissionModel::script(int index) const
//{
//}

//QString MissionModel::intro(int index) const
//{
//}

