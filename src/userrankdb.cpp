#include "userrankdb.h"
#include "globalsmanager.h"
#include <wx/fileconf.h>
#include <wx/filename.h>
#include <wx/wfstream.h>

UserRankDB& CustomRankDB()
{
    static GlobalObjectHolder<UserRankDB> m_rankdb;
    return m_rankdb;
}

UserRankDB::UserRankDB()
{
	m_database = new wxFileConfig();
}

UserRankDB::~UserRankDB()
{
	delete m_database;
}

void UserRankDB::SetOwner( const wxString& playeridentifier )
{
	m_owner = playeridentifier;
	m_database->Write( _T("DatabaseOwner"), playeridentifier );
}

UserStatus::UserRankContainer UserRankDB::GetPlayerRank( const wxString& playeridentifier, const wxString& modshortname )
{
	return (UserStatus::UserRankContainer)m_database->Read( _T("MergedTable/") + playeridentifier + _T("/") + modshortname + _T("/Rank"), (long)UserStatus::USER_RANK_UNKNOWN );
}

UserStatus::UserTrustContainer UserRankDB::GetPlayerTrust( const wxString& playeridentifier )
{
	return (UserStatus::UserTrustContainer)m_database->Read( _T("MergedTable/") + playeridentifier + _T("/Trust"), (long)UserStatus::USER_TRUST_UNKNOWN );
}

void UserRankDB::SetPlayerRank( const wxString& playeridentifier, const wxString& modshortname, const UserStatus::UserRankContainer& value )
{
	m_database->Write( _T("OriginalTables/") + m_owner + _T("/") + playeridentifier + _T("/") + modshortname + _T("/Rank"), (int)value );
	m_database->Write( _T("MergedTable/") + playeridentifier + _T("/Rank"), (int)value );
	m_database->Flush();
}

void UserRankDB::SetPlayerTrust( const wxString& playeridentifier, const UserStatus::UserTrustContainer& value )
{
	m_database->Write( _T("OriginalTables/") + m_owner + _T("/") + playeridentifier + _T("/Trust"), (int)value );
	m_database->Write( _T("MergedTable/") + playeridentifier + _T("/Trust"), (int)value );
	m_database->Flush();
}

bool UserRankDB::ImportExternalPlayerDatabaseFromFile( const wxString& filepath )
{
	if ( !wxFileName::IsFileReadable( filepath ) ) return false;
	wxFileInputStream streamdb( filepath );
	if ( !streamdb.IsOk() ) return false;
	return ImportExternalPlayerDatabase( streamdb );
}

bool UserRankDB::ImportExternalPlayerDatabase( wxInputStream& input )
{
	wxFileConfig db_to_import( input, wxConvUTF8 );
	wxString importingowner = db_to_import.Read( _T("DatabaseOwner"), _T("") );
	if ( importingowner.IsEmpty() ) return false;
	UserStatus::UserTrustContainer importingtrust = GetPlayerTrust( importingowner );
	if ( importingtrust == UserStatus::USER_TRUST_UNKNOWN ) return false; // don't know how to weight stuff
  wxString playeridentifier;
  long dummy;
  db_to_import.SetPath( _T("OriginalTables/") + importingowner );
  bool groupexist = db_to_import.GetFirstGroup(playeridentifier, dummy);
  while ( groupexist )
  {
		// import trust if we don't have already a personal entry
  	if ( (UserStatus::UserTrustContainer)m_database->Read( _T("OriginalTables/") + m_owner + _T("/") + playeridentifier + _T("/Trust"), (long)UserStatus::USER_TRUST_UNKNOWN ) != UserStatus::USER_TRUST_UNKNOWN )
  	{
  		// insert awesome merge function here
  	}

  	db_to_import.SetPath( _T("OriginalTables/") + importingowner + _T("/") + playeridentifier );
  	wxString modshortname;
  	long moddummy;
  	bool modexist = db_to_import.GetFirstGroup(modshortname, moddummy);
  	while ( modexist ) // merge singular mods
  	{
  		// import rank if we don't have already a personal entry
			if ( (UserStatus::UserRankContainer)m_database->Read( _T("OriginalTables/") + m_owner + _T("/") + modshortname + _T("/") + playeridentifier + _T("/Rank"), (long)UserStatus::USER_RANK_UNKNOWN ) != UserStatus::USER_RANK_UNKNOWN )
			{
				// insert awesome merge function here
			}
			modexist = db_to_import.GetNextGroup(modshortname, moddummy);
  	}
  	db_to_import.SetPath( _T("OriginalTables/") + importingowner );

    groupexist = db_to_import.GetNextGroup(playeridentifier, dummy);
  }

  return true;
}
