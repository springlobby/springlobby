#include "missionmodel.h"
#include <lslunitsync/c_api.h>
#include "customizations.h"
#include "exceptions.h"
#include <utils/conversion.h>
#include <QDebug>


MissionModel::MissionModel(QObject *parent) :
    QAbstractListModel(parent)
{
    wxString mission_dir;
    std::string md = SLcustomizations().DataBasePath().append( "/missions/" ).toStdString();
    wxArrayString mission_dirs = LSL::susynclib().SubDirsVFS( TowxString(md), _T("*"), _T("r") );//only raw vfs part

    foreach( mission_dir, mission_dirs ) {
//        try
        {
            const wxArrayString mission_pics = LSL::susynclib().DirListVFS( mission_dir, _T("*.png"), _T("r") );
            if ( mission_pics.size() < 1 )
                throw content_exception( QString( "mission pic in ").append(ToQString(mission_dir)).toStdString() );
            const wxString image = mission_pics[0];
            const wxArrayString mission_scripts = LSL::susynclib().DirListVFS( mission_dir, _T("*.script"), _T("r") );
            if ( mission_scripts.size() < 1 )
                throw content_exception( QString( "mission script in ").append(ToQString(mission_dir)).toStdString() );
            const wxString script = mission_scripts[0];
            const wxArrayString mission_txt = LSL::susynclib().DirListVFS( mission_dir, _T("*.txt"), _T("r") );
            if ( mission_txt.size() < 1 )
                throw content_exception( QString( "mission description in ").append(ToQString(mission_dir)).toStdString() );
            const wxString content = LSL::usync().GetTextfileAsString(  SLcustomizations().GetModname(), mission_txt[0] );
            const wxString title = content.BeforeFirst( '\n' );
            const wxString intro = content.AfterFirst( '\n' );

            Mission m;
            m.intro = ToQString( intro );
            m.title = ToQString( title );
            m.script = ToQString( script );
            m.image = ToQString( image );
            qDebug() << m.title << " " << m.script << " " << ToQString( mission_dir );
            m_missions.append( m );
        }
//        catch ( std::exception& e ) { continue; }
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

