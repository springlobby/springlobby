#include "missionmodel.h"
#include "springunitsynclib.h"
#include "customizations.h"

#include <QDebug>

MissionModel::MissionModel(QObject *parent) :
    QAbstractListModel(parent)
{
    wxString mission_dir;
    wxArrayString mission_dirs = susynclib().SubDirsVFS( _T("missions"), _T("*"), _T("M") );//only mod vfs part

    foreach( mission_dir, mission_dirs ) {
        try {
            const wxArrayString mission_pics = susynclib().DirListVFS( mission_dir, _T("*.png") );
            if ( mission_pics.size() < 1 )
                continue;
            const wxString image = mission_pics[0];
            const wxArrayString mission_scripts = susynclib().DirListVFS( mission_dir, _T("*.script") );
            if ( mission_scripts.size() < 1 )
                continue;
            const wxString script = mission_scripts[0];
            const wxArrayString mission_txt = susynclib().DirListVFS( mission_dir, _T("*.txt") );
            if ( mission_txt.size() < 1 )
                continue;
            const wxString content = usync().GetTextfileAsString(  SLcustomizations().GetModname(), mission_txt[0] );
            const wxString title = content.BeforeFirst( '\n' );
            const wxString intro = content.AfterFirst( '\n' );

            Mission m;
            m.intro = ToQString( intro );
            m.title = ToQString( title );
            m.script = ToQString( script );
            m.image = ToQString( image );
            qDebug() << m.title << " " << m.script << " " << ToQString( mission_dir );
            m_missions.append( m );
        } catch ( std::exception& e ) { continue; }
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

