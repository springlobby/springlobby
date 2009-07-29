#ifndef SPRINGLOBBY_HEADERGUARD_SPRINGUNITSYNCLIB_H
#define SPRINGLOBBY_HEADERGUARD_SPRINGUNITSYNCLIB_H

#include <wx/thread.h>
#include <wx/string.h>
#include <stdexcept>

#include "nonportable.h"
#include "iunitsync.h"

class wxString;
class wxImage;
struct SpringMapInfo;
class wxDynamicLibrary;

class unitsync_assert : public std::runtime_error
{
  public:
   unitsync_assert(std::string msg) : std::runtime_error(msg) {};
};

#define UNITSYNC_EXCEPTION(cond,msg) if(!(cond))\
{wxLogMessage(_T("unitsync runtime assertion ( %s:%d ): %s"), TowxString(__FILE__).c_str(),__LINE__ , wxString(msg).c_str() );throw unitsync_assert(std::string(wxString(msg).mb_str()));}


struct SpringMapInfo
{
  char* description;
  int tidalStrength;
  int gravity;
  float maxMetal;
  int extractorRadius;
  int minWind;
  int maxWind;

  int width;
  int height;
  int posCount;
  StartPos positions[16];

  char* author;
};


/**
 * \defgroup DllPointerTypes Pointer types used with the unitsync library.
 * \TODO move from global namespace
 */
/** @{ */

typedef const char* (USYNC_CALL_CONV *GetSpringVersionPtr)();

typedef int (USYNC_CALL_CONV *InitPtr)(bool, int);
typedef void (USYNC_CALL_CONV *UnInitPtr)();
typedef const char* (USYNC_CALL_CONV *GetNextErrorPtr)();
typedef const char* (USYNC_CALL_CONV *GetWritableDataDirectoryPtr)();

typedef int (USYNC_CALL_CONV *GetMapCountPtr)();
typedef unsigned int (USYNC_CALL_CONV *GetMapChecksumPtr)(int);
typedef const char* (USYNC_CALL_CONV *GetMapNamePtr)(int);
typedef int (USYNC_CALL_CONV *GetMapInfoExPtr)(const char*, SpringMapInfo*, int);
typedef void* (USYNC_CALL_CONV *GetMinimapPtr)(const char*, int);
typedef int (USYNC_CALL_CONV *GetInfoMapSizePtr)(const char*, const char*, int*, int*);
typedef int (USYNC_CALL_CONV *GetInfoMapPtr)(const char*, const char*, void*, int);

typedef unsigned int (USYNC_CALL_CONV *GetPrimaryModChecksumPtr)(int);
typedef int (USYNC_CALL_CONV *GetPrimaryModIndexPtr)(const char*);
typedef const char* (USYNC_CALL_CONV *GetPrimaryModNamePtr)(int);
typedef int (USYNC_CALL_CONV *GetPrimaryModCountPtr)();
typedef const char* (USYNC_CALL_CONV *GetPrimaryModArchivePtr)(int);

typedef int (USYNC_CALL_CONV *GetSideCountPtr)();
typedef const char* (USYNC_CALL_CONV *GetSideNamePtr)(int);

typedef void (USYNC_CALL_CONV *AddAllArchivesPtr)(const char*);
typedef void (USYNC_CALL_CONV *RemoveAllArchivesPtr)();

typedef const char * (USYNC_CALL_CONV *GetFullUnitNamePtr)(int);
typedef const char * (USYNC_CALL_CONV *GetUnitNamePtr)(int);
typedef int (USYNC_CALL_CONV *GetUnitCountPtr)();
typedef int (USYNC_CALL_CONV *ProcessUnitsNoChecksumPtr)();

typedef int (USYNC_CALL_CONV *InitFindVFSPtr)(const char*);
typedef int (USYNC_CALL_CONV *FindFilesVFSPtr)(int, char*, int);
typedef int (USYNC_CALL_CONV *OpenFileVFSPtr)(const char*);
typedef int (USYNC_CALL_CONV *FileSizeVFSPtr)(int);
typedef int (USYNC_CALL_CONV *ReadFileVFSPtr)(int, void*, int);
typedef void (USYNC_CALL_CONV *CloseFileVFSPtr)(int);

typedef void (USYNC_CALL_CONV *SetSpringConfigFilePtr)(const char*);
typedef const char * (USYNC_CALL_CONV *GetSpringConfigFilePtr)();

typedef int (USYNC_CALL_CONV *GetSpringConfigIntPtr)(const char*, int );
typedef const char* (USYNC_CALL_CONV *GetSpringConfigStringPtr)(const char*, const char* );
typedef float (USYNC_CALL_CONV *GetSpringConfigFloatPtr)(const char*, float );

typedef void (USYNC_CALL_CONV *SetSpringConfigStringPtr)(const char*, const char* );
typedef void (USYNC_CALL_CONV *SetSpringConfigIntPtr)(const char*, int );
typedef void (USYNC_CALL_CONV *SetSpringConfigFloatPtr)(const char*, float );

typedef int (USYNC_CALL_CONV *ProcessUnitsPtr)(void);
typedef void (USYNC_CALL_CONV *AddArchivePtr)(const char* name);
typedef unsigned int (USYNC_CALL_CONV *GetArchiveChecksumPtr)(const char* arname);
typedef const char* (USYNC_CALL_CONV *GetArchivePathPtr)(const char* arname);
typedef int (USYNC_CALL_CONV *GetMapArchiveCountPtr)(const char* mapName);
typedef const char* (USYNC_CALL_CONV *GetMapArchiveNamePtr)(int index);
typedef unsigned int (USYNC_CALL_CONV *GetMapChecksumPtr)(int index);
typedef int (USYNC_CALL_CONV *GetMapChecksumFromNamePtr)(const char* mapName);

typedef const char* (USYNC_CALL_CONV *GetPrimaryModShortNamePtr)(int index);
typedef const char* (USYNC_CALL_CONV *GetPrimaryModVersionPtr)(int index);
typedef const char* (USYNC_CALL_CONV *GetPrimaryModMutatorPtr)(int index);
typedef const char* (USYNC_CALL_CONV *GetPrimaryModGamePtr)(int index);
typedef const char* (USYNC_CALL_CONV *GetPrimaryModShortGamePtr)(int index);
typedef const char* (USYNC_CALL_CONV *GetPrimaryModDescriptionPtr)(int index);
typedef const char* (USYNC_CALL_CONV *GetPrimaryModArchivePtr)(int index);
typedef int (USYNC_CALL_CONV *GetPrimaryModArchiveCountPtr)(int index);
typedef const char* (USYNC_CALL_CONV *GetPrimaryModArchiveListPtr)(int arnr);
typedef unsigned int (USYNC_CALL_CONV *GetPrimaryModChecksumFromNamePtr)(const char* name);
typedef unsigned int (USYNC_CALL_CONV *GetModValidMapCountPtr)();
typedef const char* (USYNC_CALL_CONV *GetModValidMapPtr)(int index);

typedef int (USYNC_CALL_CONV *GetLuaAICountPtr)();
typedef const char* (USYNC_CALL_CONV *GetLuaAINamePtr)(int aiIndex);
typedef const char* (USYNC_CALL_CONV *GetLuaAIDescPtr)(int aiIndex);

typedef int (USYNC_CALL_CONV *GetMapOptionCountPtr)(const char* name);
typedef int (USYNC_CALL_CONV *GetCustomOptionCountPtr)(const char* name);
typedef int (USYNC_CALL_CONV *GetModOptionCountPtr)();
typedef int (USYNC_CALL_CONV *GetSkirmishAIOptionCountPtr)(int index);
typedef const char* (USYNC_CALL_CONV *GetOptionKeyPtr)(int optIndex);
typedef const char* (USYNC_CALL_CONV *GetOptionNamePtr)(int optIndex);
typedef const char* (USYNC_CALL_CONV *GetOptionDescPtr)(int optIndex);
typedef const char* (USYNC_CALL_CONV *GetOptionSectionPtr)(int optIndex);
typedef const char* (USYNC_CALL_CONV *GetOptionStylePtr)(int optIndex);
typedef int (USYNC_CALL_CONV *GetOptionTypePtr)(int optIndex);
typedef int (USYNC_CALL_CONV *GetOptionBoolDefPtr)(int optIndex);
typedef float (USYNC_CALL_CONV *GetOptionNumberDefPtr)(int optIndex);
typedef float (USYNC_CALL_CONV *GetOptionNumberMinPtr)(int optIndex);
typedef float (USYNC_CALL_CONV *GetOptionNumberMaxPtr)(int optIndex);
typedef float (USYNC_CALL_CONV *GetOptionNumberStepPtr)(int optIndex);
typedef const char* (USYNC_CALL_CONV *GetOptionStringDefPtr)(int optIndex);
typedef int (USYNC_CALL_CONV *GetOptionStringMaxLenPtr)(int optIndex);
typedef int (USYNC_CALL_CONV *GetOptionListCountPtr)(int optIndex);
typedef const char* (USYNC_CALL_CONV *GetOptionListDefPtr)(int optIndex);
typedef const char* (USYNC_CALL_CONV *GetOptionListItemKeyPtr)(int optIndex, int itemIndex);
typedef const char* (USYNC_CALL_CONV *GetOptionListItemNamePtr)(int optIndex, int itemIndex);
typedef const char* (USYNC_CALL_CONV *GetOptionListItemDescPtr)(int optIndex, int itemIndex);

typedef int (USYNC_CALL_CONV *OpenArchivePtr)(const char* name);
typedef void (USYNC_CALL_CONV *CloseArchivePtr)(int archive);
typedef int (USYNC_CALL_CONV *FindFilesArchivePtr)(int archive, int cur, char* nameBuf, int* size);
typedef int (USYNC_CALL_CONV *OpenArchiveFilePtr)(int archive, const char* name);
typedef int (USYNC_CALL_CONV *ReadArchiveFilePtr)(int archive, int handle, void* buffer, int numBytes);
typedef void (USYNC_CALL_CONV *CloseArchiveFilePtr)(int archive, int handle);
typedef int (USYNC_CALL_CONV *SizeArchiveFilePtr)(int archive, int handle);

typedef int (USYNC_CALL_CONV *GetSkirmishAICountPtr)();
typedef int (USYNC_CALL_CONV *GetSkirmishAIInfoCountPtr)(int index);
typedef const char* (USYNC_CALL_CONV *GetInfoKeyPtr)(int index);
typedef const char* (USYNC_CALL_CONV *GetInfoValuePtr)(int index);
typedef const char* (USYNC_CALL_CONV *GetInfoDescriptionPtr)(int index);

/// Unitsync functions wrapping lua parser
typedef void (USYNC_CALL_CONV *lpClosePtr)();
typedef int (USYNC_CALL_CONV *lpOpenFilePtr)(const char* filename, const char* fileModes,  const char* accessModes);
typedef int (USYNC_CALL_CONV *lpOpenSourcePtr)(const char* source, const char* accessModes);
typedef int (USYNC_CALL_CONV *lpExecutePtr)();
typedef const char* (USYNC_CALL_CONV *lpErrorLogPtr)();

typedef void (USYNC_CALL_CONV *lpAddTableIntPtr)(int key, int override);
typedef void (USYNC_CALL_CONV *lpAddTableStrPtr)(const char* key, int override);
typedef void (USYNC_CALL_CONV *lpEndTablePtr)();
typedef void (USYNC_CALL_CONV *lpAddIntKeyIntValPtr)(int key, int val);
typedef void (USYNC_CALL_CONV *lpAddStrKeyIntValPtr)(const char* key, int val);
typedef void (USYNC_CALL_CONV *lpAddIntKeyBoolValPtr)(int key, int val);
typedef void (USYNC_CALL_CONV *lpAddStrKeyBoolValPtr)(const char* key, int val);
typedef void (USYNC_CALL_CONV *lpAddIntKeyFloatValPtr)(int key, float val);
typedef void (USYNC_CALL_CONV *lpAddStrKeyFloatValPtr)(const char* key, float val);
typedef void (USYNC_CALL_CONV *lpAddIntKeyStrValPtr)(int key, const char* val);
typedef void (USYNC_CALL_CONV *lpAddStrKeyStrValPtr)(const char* key, const char* val);

typedef int (USYNC_CALL_CONV *lpRootTablePtr)();
typedef int (USYNC_CALL_CONV *lpRootTableExprPtr)(const char* expr);
typedef int (USYNC_CALL_CONV *lpSubTableIntPtr)(int key);
typedef int (USYNC_CALL_CONV *lpSubTableStrPtr)(const char* key);
typedef int (USYNC_CALL_CONV *lpSubTableExprPtr)(const char* expr);
typedef void (USYNC_CALL_CONV *lpPopTablePtr)();

typedef int (USYNC_CALL_CONV *lpGetKeyExistsIntPtr)(int key);
typedef int (USYNC_CALL_CONV *lpGetKeyExistsStrPtr)(const char* key);

typedef int (USYNC_CALL_CONV *lpGetIntKeyTypePtr)(int key);
typedef int (USYNC_CALL_CONV *lpGetStrKeyTypePtr)(const char* key);

typedef int (USYNC_CALL_CONV *lpGetIntKeyListCountPtr)();
typedef int (USYNC_CALL_CONV *lpGetIntKeyListEntryPtr)(int index);
typedef int (USYNC_CALL_CONV *lpGetStrKeyListCountPtr)();
typedef const char* (USYNC_CALL_CONV *lpGetStrKeyListEntryPtr)(int index);

typedef int (USYNC_CALL_CONV *lpGetIntKeyIntValPtr)(int key, int defVal);
typedef int (USYNC_CALL_CONV *lpGetStrKeyIntValPtr)(const char* key, int defVal);
typedef int (USYNC_CALL_CONV *lpGetIntKeyBoolValPtr)(int key, int defVal);
typedef int (USYNC_CALL_CONV *lpGetStrKeyBoolValPtr)(const char* key, int defVal);
typedef float (USYNC_CALL_CONV *lpGetIntKeyFloatValPtr)(int key, float defVal);
typedef float (USYNC_CALL_CONV *lpGetStrKeyFloatValPtr)(const char* key, float defVal);
typedef const char* (USYNC_CALL_CONV *lpGetIntKeyStrValPtr)(int key, const char* defVal);
typedef const char* (USYNC_CALL_CONV *lpGetStrKeyStrValPtr)(const char* key, const char* defVal);


/** @} */

/**
 * Primitive class handeling the unitsync library.
 *
 * This class is - in a limited way - thread safe but may block execution
 * in case two threads use it at the same time.  The thread safety ensures
 * there can never be multiple threads executing unitsync functions at the
 * same time.  However, many unitsync functions use (hidden) global state,
 * so often there is a need for running multiple unitsync methods while
 * holding a single lock continuously.
 */
class SpringUnitSyncLib
{
  public:

    /**
     * Constructor.
     */
    SpringUnitSyncLib();

    /**
     * Destructor, unloads unitsync if loaded.
     */
    ~SpringUnitSyncLib();

    /**
     * Loads the unitsync library from path.
     * @param path path to the unitsync lib.
     * @param ForceConfigFilePath if set forces unitsync to use pointed config file, if empty leaves to spring's default
     * @see Unload().
     * @note Throws runtime_error if load failed.
     */
    void Load( const wxString& path, const wxString& ForceConfigFilePath );

    /**
     * Unload the unitsync library. Does nothing if not loaded.
     * @see Load().
     */
    void Unload();

    /**
     * Returns true if the library is loaded.
     */
    bool IsLoaded();

    /**
     * Gets last error from unitsync library
     * @note throws unitsync_assert in case of error
     * @note this method should only be used after using directly an unitsync call to catch it's errors
     */
    void AssertUnitsyncOk();

    /**
     * Get list of errors from unitsync library in an array
     */
    wxArrayString GetUnitsyncErrors();

    bool VersionSupports( IUnitSync::GameFeature feature );


    int GetModIndex( const wxString& name );

    wxString GetSpringVersion();

    /**
     * Loads unitsync from any number of paths in succession,
     * queries the Spring versions supported by these unitsyncs,
     * and returns those.
     *
     * This is done by a single function because this "transaction"
     * needs to hold the unitsync lock the entire time.
     */
    std::map<wxString, wxString> GetSpringVersionList(const std::map<wxString, wxString>& usync_paths);

    wxString GetSpringDataDir();
    wxString GetConfigFilePath();

    int GetMapCount();
    wxString GetMapChecksum( int index );
    wxString GetMapName( int index );
    int GetMapArchiveCount( int index );
    wxString GetMapArchiveName( int arnr );
    wxArrayString GetMapDeps( int index );

    /**
     * @brief Get information about a map.
     * @param version will get author if >=1.
     * @note Throws assert_exception if unsuccessful.
     */
    MapInfo GetMapInfoEx( const wxString& mapName, int version );

    /**
     * @brief Get minimap.
     * @note Throws assert_exception if unsuccessful.
     */
    wxImage GetMinimap( const wxString& mapFileName );

    /**
     * @brief Get metalmap.
     * @note Throws assert_exception if unsuccessful.
     */
    wxImage GetMetalmap( const wxString& mapFileName );

    /**
     * @brief Get heightmap.
     * @note Throws assert_exception if unsuccesful.
     */
    wxImage GetHeightmap( const wxString& mapFileName );

    wxString GetPrimaryModChecksum( int index );
    int GetPrimaryModIndex( const wxString& modName );
    wxString GetPrimaryModName( int index );
    int GetPrimaryModCount();
    wxString GetPrimaryModArchive( int index );
    wxString GetPrimaryModShortName( int index );
    wxString GetPrimaryModVersion( int index );
    wxString GetPrimaryModMutator( int index );
    wxString GetPrimaryModGame( int index );
    wxString GetPrimaryModShortGame( int index );
    wxString GetPrimaryModDescription( int index );
    int GetPrimaryModArchiveCount( int index );
    wxString GetPrimaryModArchiveList( int arnr );
    wxString GetPrimaryModChecksumFromName( const wxString& name );
    wxArrayString GetModDeps( int index );

    wxArrayString GetSides( const wxString& modName );

    /**
     * Add all achives.
     * @note Not sure what this does, but adding the mod archive path to this when setting new mod seems to work :)
     */
    void AddAllArchives( const wxString& root );

    void SetCurrentMod( const wxString& modname );
    void UnSetCurrentMod( );

    wxString GetFullUnitName( int index );
    wxString GetUnitName( int index );
    int GetUnitCount();
    int ProcessUnitsNoChecksum();

    /**
     * Search for a file pattern.
     * @param the search patterns
     * @return wxarraystring of results
     */
    wxArrayString FindFilesVFS( const wxString& name );
    int OpenFileVFS( const wxString& name );
    int FileSizeVFS( int handle );
    int ReadFileVFS( int handle, void* buffer, int bufferLength );
    void CloseFileVFS( int handle );

    int GetLuaAICount( const wxString& modname );
    wxString GetLuaAIName( int aiIndex );
    wxString GetLuaAIDesc( int aiIndex );

    unsigned int GetValidMapCount( const wxString& modname );
    wxString GetValidMapName( unsigned int MapIndex );

    int GetMapOptionCount( const wxString& name );
    int GetCustomOptionCount( const wxString& modname, const wxString& filename );
    int GetModOptionCount( const wxString& name );
    int GetAIOptionCount( const wxString& modname, int index );
    wxString GetOptionKey( int optIndex );
    wxString GetOptionName( int optIndex );
    wxString GetOptionDesc( int optIndex );
    wxString GetOptionSection( int optIndex );
    wxString GetOptionStyle( int optIndex );
    int GetOptionType( int optIndex );
    int GetOptionBoolDef( int optIndex );
    float GetOptionNumberDef( int optIndex );
    float GetOptionNumberMin( int optIndex );
    float GetOptionNumberMax( int optIndex );
    float GetOptionNumberStep( int optIndex );
    wxString GetOptionStringDef( int optIndex );
    int GetOptionStringMaxLen( int optIndex );
    int GetOptionListCount( int optIndex );
    wxString GetOptionListDef( int optIndex );
    wxString GetOptionListItemKey( int optIndex, int itemIndex );
    wxString GetOptionListItemName( int optIndex, int itemIndex );
    wxString GetOptionListItemDesc( int optIndex, int itemIndex );

    int OpenArchive( const wxString& name );
    void CloseArchive( int archive );
    int FindFilesArchive( int archive, int cur, wxString& nameBuf );
    int OpenArchiveFile( int archive, const wxString& name );
    int ReadArchiveFile( int archive, int handle, void* buffer, int numBytes) ;
    void CloseArchiveFile( int archive, int handle );
    int SizeArchiveFile( int archive, int handle );
    wxString GetArchivePath( const wxString& name );

    int GetSpringConfigInt( const wxString& key, int defValue );
    wxString GetSpringConfigString( const wxString& key, const wxString& defValue );
    float GetSpringConfigFloat( const wxString& key, const float defValue );
    void SetSpringConfigString( const wxString& key, const wxString& value );
    void SetSpringConfigInt( const wxString& key, int value );
    void SetSpringConfigFloat( const wxString& key, const float value );

    /// AI info
    int GetSkirmishAICount( const wxString& modname );
    /**
     * Get next search result.
     * @param the AI index within range of GetSkirmishAIInfoCount
     * @return an array made of blocks with this layout { key, value, description }
     */
    wxArrayString GetAIInfo( int index );

    wxString GetArchiveChecksum( const wxString& VFSPath );

    /// lua parser

    void CloseParser();
    bool OpenParserFile( const wxString& filename, const wxString& filemodes, const wxString& accessModes );
    bool OpenParserSource( const wxString& source, const wxString& accessModes );
    bool ParserExecute();
    wxString ParserErrorLog();

    void ParserAddTable( int key, bool override );
    void ParserAddTable( const wxString& key, bool override );
    void ParserEndTable();
    void ParserAddTableValue( int key, int val );
    void ParserAddTableValue( const wxString& key, int val );
    void ParserAddTableValue( int key, bool val );
    void ParserAddTableValue( const wxString& key, bool val );
    void ParserAddTableValue( int key, const wxString& val );
    void ParserAddTableValue( const wxString& key, const wxString& val );
    void ParserAddTableValue( int key, float val );
    void ParserAddTableValue( const wxString& key, float val );

    bool ParserGetRootTable();
    bool ParserGetRootTableExpression( const wxString& exp );
    bool ParserGetSubTableInt( int key );
    bool ParserGetSubTableString( const wxString& key );
    bool ParserGetSubTableInt( const wxString& exp );
    void ParserPopTable();

    bool ParserKeyExists( int key );
    bool ParserKeyExists( const wxString& key );

    int ParserGetKeyType( int key );
    int ParserGetKeyType( const wxString& key );

    int ParserGetIntKeyListCount();
    int ParserGetIntKeyListEntry( int index );
    int ParserGetStringKeyListCount();
    int ParserGetStringKeyListEntry( int index );

    int GetKeyValue( int key, int defval );
    bool GetKeyValue( int key, bool defval );
    wxString GetKeyValue( int key, const wxString& defval );
    float GetKeyValue( int key, float defval );
    int GetKeyValue( const wxString& key, int defval );
    bool GetKeyValue( const wxString& key, bool defval );
    wxString GetKeyValue( const wxString& key, const wxString& defval );
    float GetKeyValue( const wxString& key, float defval );


  protected:
    SpringUnitSyncLib( const SpringUnitSyncLib& );
    //! Keeps track if unitsync is loaded or not.
    bool m_loaded;

    //! Handle to the unitsync library.
    wxDynamicLibrary* m_libhandle;

    //! Critical section controlling access to unitsync functions.
    mutable wxCriticalSection m_lock;

    //! Path to unitsync.
    wxString m_path;

    //! the current loaded mod.
    wxString m_current_mod;

    /**
     * Loads a function pointer from unitsync.
     */
    void* _GetLibFuncPtr( const wxString& name );

    /**
     * Loads the unitsync library from path.
     * @note this function is not threadsafe if called from code not locked.
     * @see Load()
     */
    void _Load( const wxString& path );

    /**
     * Initializes unitsync.
     */
    void _Init();

	/**
	 * Calls RemoveAllArchives if available, _Init() otherwise.
	 */
	void _RemoveAllArchives();

    /**
     * Internal Unload() function.
     * @note this function is not threadsafe if called from code not locked.
     * @see Unload()
     */
    void _Unload();

    /**
     * Returns true if the library is loaded. Internal.
     */
    bool _IsLoaded();

    void _ConvertSpringMapInfo( const SpringMapInfo& in, MapInfo& out );

    void _SetCurrentMod( const wxString& modname );

    /**
     * \defgroup DllFuncPointers Pointers to the functions in unitsync.
     */
    /*@{*/

    InitPtr m_init;
    UnInitPtr m_uninit;
    GetNextErrorPtr m_get_next_error;
    GetWritableDataDirectoryPtr m_get_writeable_data_dir;

    GetMapCountPtr m_get_map_count;
    GetMapChecksumPtr m_get_map_checksum;
    GetMapNamePtr m_get_map_name;
    GetMapInfoExPtr m_get_map_info_ex;
    GetMinimapPtr m_get_minimap;
    GetInfoMapSizePtr m_get_infomap_size;
    GetInfoMapPtr m_get_infomap;

    GetPrimaryModChecksumPtr m_get_mod_checksum;
    GetPrimaryModIndexPtr m_get_mod_index;
    GetPrimaryModNamePtr m_get_mod_name;
    GetPrimaryModCountPtr m_get_mod_count;
    GetPrimaryModArchivePtr m_get_mod_archive;

    GetSideCountPtr m_get_side_count;
    GetSideNamePtr m_get_side_name;

    AddAllArchivesPtr m_add_all_archives;
    RemoveAllArchivesPtr m_remove_all_archives;

    GetUnitCountPtr m_get_unit_count;
    GetUnitNamePtr m_get_unit_name;
    GetFullUnitNamePtr m_get_unit_full_name;
    ProcessUnitsNoChecksumPtr m_proc_units_nocheck;

    InitFindVFSPtr m_init_find_vfs;
    FindFilesVFSPtr m_find_files_vfs;
    OpenFileVFSPtr m_open_file_vfs;
    FileSizeVFSPtr m_file_size_vfs;
    ReadFileVFSPtr m_read_file_vfs;
    CloseFileVFSPtr m_close_file_vfs;

    GetSpringVersionPtr m_get_spring_version;

    ProcessUnitsPtr m_process_units;
    AddArchivePtr m_add_archive;
    GetArchiveChecksumPtr m_get_archive_checksum;
    GetArchivePathPtr m_get_archive_path;
    GetMapArchiveCountPtr m_get_map_archive_count;
    GetMapArchiveNamePtr m_get_map_archive_name;
    GetMapChecksumFromNamePtr m_get_map_checksum_from_name;

    GetPrimaryModShortNamePtr m_get_primary_mod_short_name;
    GetPrimaryModVersionPtr m_get_primary_mod_version;
    GetPrimaryModMutatorPtr m_get_primary_mod_mutator;
    GetPrimaryModGamePtr m_get_primary_mod_game;
    GetPrimaryModShortGamePtr m_get_primary_mod_short_game;
    GetPrimaryModDescriptionPtr m_get_primary_mod_description;
    GetPrimaryModArchivePtr m_get_primary_mod_archive;
    GetPrimaryModArchiveCountPtr m_get_primary_mod_archive_count;
    GetPrimaryModArchiveListPtr m_get_primary_mod_archive_list;
    GetPrimaryModChecksumFromNamePtr m_get_primary_mod_checksum_from_name;
    GetModValidMapCountPtr m_get_mod_valid_map_count;
    GetModValidMapPtr m_get_valid_map;

    GetLuaAICountPtr m_get_luaai_count;
    GetLuaAINamePtr m_get_luaai_name;
    GetLuaAIDescPtr m_get_luaai_desc;

    GetMapOptionCountPtr m_get_map_option_count;
    GetCustomOptionCountPtr m_get_custom_option_count;
    GetModOptionCountPtr m_get_mod_option_count;
		GetSkirmishAIOptionCountPtr m_get_skirmish_ai_option_count;
    GetOptionKeyPtr m_get_option_key;
    GetOptionNamePtr m_get_option_name;
    GetOptionDescPtr m_get_option_desc;
    GetOptionTypePtr m_get_option_type;
    GetOptionSectionPtr m_get_option_section;
    GetOptionStylePtr m_get_option_style;
    GetOptionBoolDefPtr m_get_option_bool_def;
    GetOptionNumberDefPtr m_get_option_number_def;
    GetOptionNumberMinPtr m_get_option_number_min;
    GetOptionNumberMaxPtr m_get_option_number_max;
    GetOptionNumberStepPtr m_get_option_number_step;
    GetOptionStringDefPtr m_get_option_string_def;
    GetOptionStringMaxLenPtr m_get_option_string_max_len;
    GetOptionListCountPtr m_get_option_list_count;
    GetOptionListDefPtr m_get_option_list_def;
    GetOptionListItemKeyPtr m_get_option_list_item_key;
    GetOptionListItemNamePtr m_get_option_list_item_name;
    GetOptionListItemDescPtr m_get_option_list_item_desc;

    OpenArchivePtr m_open_archive;
    CloseArchivePtr m_close_archive;
    FindFilesArchivePtr m_find_Files_archive;
    OpenArchiveFilePtr m_open_archive_file;
    ReadArchiveFilePtr m_read_archive_file;
    CloseArchiveFilePtr m_close_archive_file;
    SizeArchiveFilePtr m_size_archive_file;

    SetSpringConfigFilePtr m_set_spring_config_file_path;
    GetSpringConfigFilePtr m_get_spring_config_file_path;
    SetSpringConfigFloatPtr m_set_spring_config_float;
    GetSpringConfigFloatPtr m_get_spring_config_float;
    GetSpringConfigIntPtr m_get_spring_config_int;
    GetSpringConfigStringPtr m_get_spring_config_string;
    SetSpringConfigStringPtr m_set_spring_config_string;
    SetSpringConfigIntPtr m_set_spring_config_int;

		GetSkirmishAICountPtr m_get_skirmish_ai_count;
		GetSkirmishAIInfoCountPtr m_get_skirmish_ai_info_count;
		GetInfoKeyPtr m_get_skirmish_ai_info_key;
		GetInfoValuePtr m_get_skirmish_ai_info_value;
		GetInfoDescriptionPtr m_get_skirmish_ai_info_description;

    // lua parser section

    lpClosePtr m_parser_close;
    lpOpenFilePtr m_parser_open_file;
    lpOpenSourcePtr m_parser_open_source;
    lpExecutePtr m_parser_execute;
    lpErrorLogPtr m_parser_error_log;

    lpAddTableIntPtr m_parser_add_table_int;
    lpAddTableStrPtr m_parser_add_table_string;
    lpEndTablePtr m_parser_end_table;
    lpAddIntKeyIntValPtr m_parser_add_int_key_int_value;
    lpAddStrKeyIntValPtr m_parser_add_string_key_int_value;
		lpAddIntKeyBoolValPtr m_parser_add_int_key_bool_value;
		lpAddStrKeyBoolValPtr m_parser_add_string_key_bool_value;
		lpAddIntKeyFloatValPtr m_parser_add_int_key_float_value;
		lpAddStrKeyFloatValPtr m_parser_add_string_key_float_value;
		lpAddIntKeyStrValPtr m_parser_add_int_key_string_value;
		lpAddStrKeyStrValPtr m_parser_add_string_key_string_value;

		lpRootTablePtr m_parser_root_table;
		lpRootTableExprPtr m_parser_root_table_expression;
		lpSubTableIntPtr m_parser_sub_table_int;
		lpSubTableStrPtr m_parser_sub_table_string;
		lpSubTableExprPtr m_parser_sub_table_expression;
		lpPopTablePtr m_parser_pop_table;

		lpGetKeyExistsIntPtr m_parser_key_int_exists;
		lpGetKeyExistsStrPtr m_parser_key_string_exists;

		lpGetIntKeyTypePtr m_parser_int_key_get_type;
		lpGetStrKeyTypePtr m_parser_string_key_get_type;

		lpGetIntKeyListCountPtr m_parser_int_key_get_list_count;
		lpGetIntKeyListEntryPtr m_parser_int_key_get_list_entry;
		lpGetStrKeyListCountPtr m_parser_string_key_get_list_count;
		lpGetStrKeyListEntryPtr m_parser_string_key_get_list_entry;

		lpGetIntKeyIntValPtr m_parser_int_key_get_int_value;
		lpGetStrKeyIntValPtr m_parser_string_key_get_int_value;
		lpGetIntKeyBoolValPtr m_parser_int_key_get_bool_value;
		lpGetStrKeyBoolValPtr m_parser_string_key_get_bool_value;
		lpGetIntKeyFloatValPtr m_parser_int_key_get_float_value;
		lpGetStrKeyFloatValPtr m_parser_string_key_get_float_value;
		lpGetIntKeyStrValPtr m_parser_int_key_get_string_value;
		lpGetStrKeyStrValPtr m_parser_string_key_get_string_value;

    /*@}*/
};

SpringUnitSyncLib& susynclib();

#endif //SPRINGLOBBY_HEADERGUARD_SPRINGUNITSYNCLIB_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-09

    springsettings is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

