#ifndef SPRINGLOBBY_HEADERGUARD_IBATTLE_H
#define SPRINGLOBBY_HEADERGUARD_IBATTLE_H


#include <wx/string.h>

#include "iunitsync.h"
#include "user.h"
#include "mmoptionswrapper.h"
#include "userlist.h"


const unsigned int DEFAULT_SERVER_PORT = 8452;
const unsigned int DEFAULT_EXTERNAL_UDP_SOURCE_PORT = 16941;

class IBattle;

struct BattleStartRect
{
    BattleStartRect()
    {
        toadd = false;
        todelete = false;
        exist = false;
        toresize = false;
    }
    bool toadd;
    bool todelete;
    bool toresize;
    bool exist;

    bool IsOk()
    {
        return exist && !todelete;
    }

    int ally;
    int top;
    int left;
    int right;
    int bottom;
};


enum NatType
{
		NAT_None = 0,
		NAT_Hole_punching,
		NAT_Fixed_source_ports
};

enum RankLimitType
{
		rank_limit_none = 0,
		rank_limit_autospec,
		rank_limit_autokick
};

struct BattleOptions
{
	BattleOptions() :
		battleid(-1),islocked(false),isreplay(false),ispassworded(false),rankneeded(0),isproxy(false),lockexternalbalancechanges(false),ranklimittype(rank_limit_autospec),
		nattype(NAT_None),port(DEFAULT_SERVER_PORT),externaludpsourceport(DEFAULT_EXTERNAL_UDP_SOURCE_PORT),internaludpsourceport(DEFAULT_EXTERNAL_UDP_SOURCE_PORT),maxplayers(0),spectators(0),
		guilistactiv(false) {}

	int battleid;
	bool islocked;
	bool isreplay;
	bool ispassworded;
	int rankneeded;
	bool isproxy;
	bool lockexternalbalancechanges;
	bool ranklimittype;

	wxString founder;

	NatType nattype;
	unsigned int port;
	wxString ip;
	unsigned int externaludpsourceport;
	unsigned int internaludpsourceport;

	unsigned int maxplayers;
	unsigned int spectators;

	wxString maphash;
	wxString modhash;

	wxString description;
	wxString mapname;
	wxString modname;

	bool guilistactiv;
};

class IBattle: public UserList
{
public:

    IBattle();
    virtual ~IBattle();

    /** @name Constants
     * @{
     */
    static const unsigned int HI_None           = 0;
    static const unsigned int HI_Map            = 1;
    static const unsigned int HI_Locked         = 2;
    static const unsigned int HI_Spectators     = 4;
    static const unsigned int HI_StartResources = 8;
    static const unsigned int HI_MaxUnits = 16;
    static const unsigned int HI_StartType = 32;
    static const unsigned int HI_GameType = 64;
    static const unsigned int HI_Options = 128;
    static const unsigned int HI_StartRects = 256;
    static const unsigned int HI_Restrictions = 512;
    static const unsigned int HI_Map_Changed = 1024;
    static const unsigned int HI_Mod_Changed = 2048;
    static const unsigned int HI_Send_All_opts  = 4096;
    /**@}*/

    /** @name Enums
     * @{
     */


    enum BalanceType
    {
        balance_divide,
        balance_random
    };

    typedef int HostInfo;

    enum StartType
    {
        ST_Fixed = 0,
        ST_Random = 1,
        ST_Choose = 2,
        ST_Pick = 3
    };

    enum GameType
    {
        GT_ComContinue = 0,
        GT_ComEnds = 1,
        GT_Lineage = 2
    };


    enum BattleType
    {
        BT_Unknown = 0,
        BT_Multi = 1,
        BT_Single = 2
    };


    /**@}*/

    virtual void SetHostMap( const wxString& mapname, const wxString& hash );
    virtual void SetLocalMap( const UnitSyncMap& map );
    virtual const UnitSyncMap& LoadMap();
    virtual wxString GetHostMapName() const;
    virtual wxString GetHostMapHash() const;

    virtual bool IsSynced();

    virtual bool IsFounderMe() = 0;

		virtual int GetPlayerNum( const User& user );

    virtual void SetHostMod( const wxString& modname, const wxString& hash );
    virtual void SetLocalMod( const UnitSyncMod& mod );
    virtual const UnitSyncMod& LoadMod();
    virtual wxString GetHostModName() const;
    virtual wxString GetHostModHash() const;

    virtual bool MapExists();
    virtual bool ModExists();

    virtual BattleStartRect GetStartRect( unsigned int allyno );
    User& OnUserAdded( User& user );
    void OnUserBattleStatusUpdated( User &user, UserBattleStatus status );
    void OnUserRemoved( User& user );

		bool IsEveryoneReady();

		void ForceSide( User& user, int side );
		void ForceAlly( User& user, int ally );
		void ForceTeam( User& user, int team );
		void ForceColour( User& user, const wxColour& col );
		void ForceSpectator( User& user, bool spectator );
		void SetHandicap( User& user, int handicap);
		void KickPlayer( User& user );


    virtual void AddStartRect( unsigned int allyno, unsigned int left, unsigned int top, unsigned int right, unsigned int bottom );
    virtual void RemoveStartRect( unsigned int allyno );
    virtual void ResizeStartRect( unsigned int allyno );
    virtual void StartRectRemoved( unsigned int allyno );
    virtual void StartRectResized( unsigned int allyno );
    virtual void StartRectAdded( unsigned int allyno );
    virtual void ClearStartRects();
    virtual unsigned int GetNumRects();

    virtual int GetFreeTeamNum( bool excludeme );

    virtual User& GetMe() = 0;

    virtual void SendHostInfo( HostInfo update );
    virtual void SendHostInfo( const wxString& Tag );
		virtual void Update ( const wxString& Tag );

    virtual unsigned int GetNumBots() const;
    virtual bool HaveMultipleBotsInSameTeam() const;
    virtual User& OnBotAdded( const wxString& nick, const UserBattleStatus& bs );

    virtual void GetFreePosition( int& x, int& y );
    virtual int GetFreeAlly();

    virtual void DisableUnit( const wxString& unitname );
    virtual void EnableUnit( const wxString& unitname );
    virtual void EnableAllUnits();
    virtual wxArrayString DisabledUnits();

    virtual void OnUnitSyncReloaded();

    virtual OptionsWrapper& CustomBattleOptions()
    {
        return m_opt_wrap;
    }

    virtual bool LoadOptionsPreset( const wxString& name );
    virtual void SaveOptionsPreset( const wxString& name );
    virtual wxString GetCurrentPreset();
    virtual void DeletePreset( const wxString& name );
    virtual wxArrayString GetPresetList();

    virtual std::vector<wxColour> &GetFixColoursPalette();
    virtual int GetClosestFixColour(const wxColour &col, const std::vector<int> &excludes, int &difference);
    virtual wxColour GetFixColour(int i);
    virtual wxColour GetFreeColour( User &for_whom ) const;
    virtual wxColour GetFreeColour() const;

    virtual int ColourDifference(const wxColour &a, const wxColour &b);

		User& GetFounder() const { return GetUser( m_opts.founder ); }

		bool IsFull() const { return GetMaxPlayers() == ( GetNumUsers() - GetSpectators() ); }

		virtual unsigned int GetNumPlayers() const;

		virtual int GetBattleId() const { return m_opts.battleid; }

		virtual bool GetGUIListActiv() const { return m_opts.guilistactiv; }
		virtual void SetGUIListActiv(bool Activ) { m_opts.guilistactiv = Activ; }

		virtual void SetInGame( bool ingame ) { m_ingame = ingame; }
		virtual bool GetInGame() const { return m_ingame; }

		virtual void SetIsReplay( const bool isreplay ) { m_opts.isreplay = isreplay; }
		virtual void SetIsLocked( const bool islocked ) { m_opts.islocked = islocked; }
		virtual bool IsLocked() const { return m_opts.islocked; }
		virtual void SetIsPassworded( const bool ispassworded ) { m_opts.ispassworded = ispassworded; }
		virtual bool IsPassworded() const { return m_opts.ispassworded; }

		virtual void SetNatType( const NatType nattype ) { m_opts.nattype = nattype; }
		virtual NatType GetNatType() const { return m_opts.nattype; }
		virtual void SetHostPort( unsigned int port) { m_opts.port = port; }

		virtual void SetMyExternalUdpSourcePort(unsigned int port){m_opts.externaludpsourceport=port;}
		virtual unsigned int GetMyExternalUdpSourcePort(){return m_opts.externaludpsourceport;}

		virtual void SetMyInternalUdpSourcePort(unsigned int port){m_opts.internaludpsourceport=port;}
		virtual unsigned int GetMyInternalUdpSourcePort(){return m_opts.internaludpsourceport;}

		virtual int GetHostPort() const { return m_opts.port; }
		virtual void SetFounder( const wxString& nick ) { m_opts.founder = nick; }
		virtual void SetHostIp( const wxString& ip ) { m_opts.ip = ip; }
		virtual wxString GetHostIp() const { return m_opts.ip; }

		virtual void SetMaxPlayers( const int& maxplayers ) { m_opts.maxplayers = maxplayers; }
		virtual unsigned int GetMaxPlayers() const { return m_opts.maxplayers; }
		virtual void SetSpectators( const int& spectators ) { m_opts.spectators = spectators; }
		virtual int GetSpectators() const { return m_opts.spectators; }

		virtual void SetRankNeeded( const int& rankneeded ) { m_opts.rankneeded = rankneeded; }
		virtual int GetRankNeeded() const { return m_opts.rankneeded; }

		// virtual void SetMapHash( const wxString& maphash ) { m_opts.maphash = maphash; }
		// virtual void SetMapname( const wxString& map ) { m_opts.mapname = map; }
		virtual void SetDescription( const wxString& desc ) { m_opts.description = desc; }
		virtual wxString GetDescription() const { return m_opts.description; }
		// virtual void SetModname( const wxString& mod ) { m_opts.modname = mod; }

		typedef std::vector<User> UserVec;
		typedef UserVec::const_iterator UserVecCIter;
		typedef UserVec::iterator UserVecIter;

		void SetBattleOptions( const BattleOptions& options ) { m_opts = options; }

        virtual void OnSelfLeftBattle();


protected:

    bool m_map_loaded;
    bool m_mod_loaded;
    bool m_map_exists;
    bool m_mod_exists;
    UnitSyncMap m_local_map;
    UnitSyncMod m_local_mod;
    UnitSyncMap m_host_map;
    UnitSyncMod m_host_mod;

    wxArrayString m_units;

    OptionsWrapper m_opt_wrap;

    BattleOptions m_opts;

		bool m_ingame;

		bool m_generating_script;

		std::map<unsigned int,BattleStartRect> m_rects;

    wxString m_preset;

    bool m_is_self_in;
    UserVec m_internal_bot_list;

};

#endif // SPRINGLOBBY_HEADERGUARD_IBATTLE_H
