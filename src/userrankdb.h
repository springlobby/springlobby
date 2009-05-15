#ifndef USERRANKDB_H_INCLUDED
#define USERRANKDB_H_INCLUDED

#include "user.h"

class wxInputStream;
class wxFileConfig;

class UserRankDB
{
	struct UserBalanceInfos
	{
		UserStatus::UserTrustContainer trust;
		UserStatus::UserRankContainer rank;
		UserBalanceInfos(): rank( UserStatus::USER_RANK_UNKNOWN ), trust( UserStatus::USER_TRUST_UNKNOWN ) {}
	};

	public:
		UserRankDB();
		~UserRankDB();

		void SetOwner( const wxString& playeridentifier );

		UserStatus::UserRankContainer GetPlayerRank( const wxString& playeridentifier );
		UserStatus::UserTrustContainer GetPlayerTrust( const wxString& playeridentifier );

		void SetPlayerRank( const wxString& playeridentifier, const UserStatus::UserRankContainer& value );
		void SetPlayerTrust( const wxString& playeridentifier, const UserStatus::UserTrustContainer& value );

		bool ImportExternalPlayerDatabaseFromFile( const wxString& path );
		bool ImportExternalPlayerDatabase( const wxInputStream& input );

	private:
		wxFileConfig* m_database;
		wxString m_owner;
};

#endif // USERRANKDB_H_INCLUDED
