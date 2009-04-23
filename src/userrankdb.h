#ifndef USERRANKDB_H_INCLUDED
#define USERRANKDB_H_INCLUDED

#include "user.h"

class wxInputStream;
class wxFileConfig;

class UserRankDB
{
	struct UserBalanceInfos
	{
		UserStatus::UserRankContainer rank;
		UserStatus::UserTrustContainer trust;
		UserBalanceInfos(): rank( UserStatus::USER_RANK_UNKNOWN ), trust( UserStatus::USER_TRUST_UNKNOWN ) {}
	};

	public:
		UserRankDB();
		~UserRankDB();

		void SetOwner( const wxString& playeridentifier );

		UserStatus::UserRankContainer GetPlayerRank( const wxString& playeridentifier, const wxString& modshortname );
		UserStatus::UserTrustContainer GetPlayerTrust( const wxString& playeridentifier );

		void SetPlayerRank( const wxString& playeridentifier, const wxString& modshortname, const UserStatus::UserRankContainer& value );
		void SetPlayerTrust( const wxString& playeridentifier, const UserStatus::UserTrustContainer& value );

		bool ImportExternalPlayerDatabaseFromFile( const wxString& path );
		bool ImportExternalPlayerDatabase( wxInputStream& input );

	private:
		wxFileConfig* m_database;
		wxString m_owner;
};

UserRankDB& CustomRankDB();

#endif // USERRANKDB_H_INCLUDED
