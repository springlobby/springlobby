#ifndef SPRINGLOBBY_HEADERGUARD_SPRINGUNITSYNCLIB_H
#define SPRINGLOBBY_HEADERGUARD_SPRINGUNITSYNCLIB_H

#include <wx/thread.h>
#include <wx/string.h>

#include "nonportable.h"
#include "iunitsync.h"

class wxString;
class wxImage;
struct SpringMapInfo;
class wxDynamicLibrary;


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
 */
/** @{ */

typedef const char* (USYNC_CALL_CONV *GetSpringVersionPtr)();

typedef int (USYNC_CALL_CONV *InitPtr)(bool, int);
typedef void (USYNC_CALL_CONV *UnInitPtr)();

typedef int (USYNC_CALL_CONV *GetMapCountPtr)();
typedef unsigned int (USYNC_CALL_CONV *GetMapChecksumPtr)(int);
typedef const char* (USYNC_CALL_CONV *GetMapNamePtr)(int);
typedef int (USYNC_CALL_CONV *GetMapInfoExPtr)(const char*, SpringMapInfo*, int);
typedef void* (USYNC_CALL_CONV *GetMinimapPtr)(const char*, int);

typedef unsigned int (USYNC_CALL_CONV *GetPrimaryModChecksumPtr)(int);
typedef int (USYNC_CALL_CONV *GetPrimaryModIndexPtr)(const char*);
typedef const char* (USYNC_CALL_CONV *GetPrimaryModNamePtr)(int);
typedef int (USYNC_CALL_CONV *GetPrimaryModCountPtr)();
typedef const char* (USYNC_CALL_CONV *GetPrimaryModArchivePtr)(int);

typedef int (USYNC_CALL_CONV *GetSideCountPtr)();
typedef const char* (USYNC_CALL_CONV *GetSideNamePtr)(int);

typedef void (USYNC_CALL_CONV *AddAllArchivesPtr)(const char*);

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
typedef int (USYNC_CALL_CONV *GetLuaAICountPtr)();
typedef const char* (USYNC_CALL_CONV *GetLuaAINamePtr)(int aiIndex);
typedef const char* (USYNC_CALL_CONV *GetLuaAIDescPtr)(int aiIndex);
typedef int (USYNC_CALL_CONV *GetMapOptionCountPtr)(const char* name);
typedef int (USYNC_CALL_CONV *GetModOptionCountPtr)();
typedef const char* (USYNC_CALL_CONV *GetOptionKeyPtr)(int optIndex);
typedef const char* (USYNC_CALL_CONV *GetOptionNamePtr)(int optIndex);
typedef const char* (USYNC_CALL_CONV *GetOptionDescPtr)(int optIndex);
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


/** @} */

/**
 * Primitive class handeling the unitsync library.
 * This class is thread safe but may block execution in case two threads use it at the same time.
 */
class SpringUnitSyncLib
{
  public:

    /**
     * Constructor.
     * @param path path to the unitsync lib, if specified the lib will be loaded when created.
     * @see Load().
    */
    SpringUnitSyncLib( const wxString& path = wxEmptyString );

    /**
     * Destructor, unloads unitsync if loaded.
     */
    ~SpringUnitSyncLib();

    /**
     * Loads the unitsync library from path.
     * @param path ath to the unitsync lib.
     * @see Unload().
     * @note Throws runtime_error if load failed.
     */
    void Load( const wxString& path );

    /**
     * Unload the unitsync library. Does nothing if not loaded.
     * @see Load().
     */
    void Unload();

    /**
     * Reloads the unitsync library.
     * @note Throws logic_error if no path has been set in constructor or in Load() call. Throws runtime_error if reloading fails.
     * @see Load().
     */
    void Reload();

    /**
     * Returns true if the library is loaded.
     */
    bool IsLoaded();



    wxString GetSpringVersion();

    int GetMapCount();
    unsigned int GetMapChecksum( int index );
    wxString GetMapName( int index );

    /**
     * Get information about a map.
     * @param version will get author if >=1.
     * @note Throws runtime_error if unsuccessful.
     */
    SpringMapInfo GetMapInfoEx( const wxString& mapName, int version );

    /**
     * Get minimap.
     * @param miplevel should be 0-10 not sure what it does, 10 seems to work fine.
     * @note Throws runtime_error if unsuccessful.
     */
    wxImage GetMinimap( const wxString& mapFileName, int miplevel );

    unsigned int GetPrimaryModChecksum( int index );
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
    unsigned int GetPrimaryModChecksumFromName( const wxString& name );

    int GetSideCount( const wxString& modName );
    wxString GetSideName( const wxString& modName, int index );

    /**
     * Add all achives.
     * @note Not sure what this does, but adding the mod archive path to this when setting new mod seems to work :)
     */
    void AddAllArchives( const wxString& root );

    wxString GetFullUnitName( int index );
    wxString GetUnitName( int index );
    int GetUnitCount();
    int ProcessUnitsNoChecksum();

    int InitFindVFS( const wxString& pattern );

    /**
     * Get next search result.
     * @param handle the handle returned by InitFindVFS().
     * @param name the returned name.
     * @return true if more results.
     */
    bool FindFilesVFS( int handle, wxString& name );
    int OpenFileVFS( const wxString& name );
    int FileSizeVFS( int handle );
    int ReadFileVFS( int handle, void* buffer, int bufferLength );
    void CloseFileVFS( int handle );

    int GetLuaAICount();
    wxString GetLuaAIName( int aiIndex );
    wxString GetLuaAIDesc( int aiIndex );

    int GetMapOptionCount( const wxString& name );
    int GetModOptionCount();
    wxString GetOptionKey( int optIndex );
    wxString GetOptionName( int optIndex );
    wxString GetOptionDesc( int optIndex );
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

    int GetSpringConfigInt( const wxString& key, int defValue );
    wxString GetSpringConfigString( const wxString& key, const wxString& defValue );
    float GetSpringConfigFloat( const wxString& key, const float defValue );
    void SetSpringConfigString( const wxString& key, const wxString& value );
    void SetSpringConfigInt( const wxString& key, int value );
    void SetSpringConfigFloat( const wxString& key, const float value );


  protected:

    //! Keeps track if unitsync is loaded or not.
    bool m_loaded;

    //! Handle to the unitsync library.
    wxDynamicLibrary* m_libhandle;

    //! Critical section controlling access to unitsync functions.
    wxCriticalSection m_lock;

    //! Path to unitsync.
    wxString m_path;


    /**
     * Loads a function pointer from unitsync.
     */
    void* _GetLibFuncPtr( const wxString& name );

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

    /**
     * \defgroup DllFuncPointers Pointers to the functions in unitsync.
     */
    /*@{*/

    InitPtr m_init;
    UnInitPtr m_uninit;

    GetMapCountPtr m_get_map_count;
    GetMapChecksumPtr m_get_map_checksum;
    GetMapNamePtr m_get_map_name;
    GetMapInfoExPtr m_get_map_info_ex;
    GetMinimapPtr m_get_minimap;

    GetPrimaryModChecksumPtr m_get_mod_checksum;
    GetPrimaryModIndexPtr m_get_mod_index;
    GetPrimaryModNamePtr m_get_mod_name;
    GetPrimaryModCountPtr m_get_mod_count;
    GetPrimaryModArchivePtr m_get_mod_archive;

    GetSideCountPtr m_get_side_count;
    GetSideNamePtr m_get_side_name;

    AddAllArchivesPtr m_add_all_archives;

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
    GetLuaAICountPtr m_get_luaai_count;
    GetLuaAINamePtr m_get_luaai_name;
    GetLuaAIDescPtr m_get_luaai_desc;
    GetMapOptionCountPtr m_get_map_option_count;
    GetModOptionCountPtr m_get_Mod_option_count;
    GetOptionKeyPtr m_get_option_key;
    GetOptionNamePtr m_get_option_name;
    GetOptionDescPtr m_get_option_desc;
    GetOptionTypePtr m_get_option_type;
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
    SetSpringConfigFloatPtr m_set_spring_config_float;
    GetSpringConfigFloatPtr m_get_spring_config_float;
    GetSpringConfigIntPtr m_get_spring_config_int;
    GetSpringConfigStringPtr m_get_spring_config_string;
    SetSpringConfigStringPtr m_set_spring_config_string;
    SetSpringConfigIntPtr m_set_spring_config_int;

    /*@}*/
};

SpringUnitSyncLib* susynclib();

#endif //SPRINGLOBBY_HEADERGUARD_SPRINGUNITSYNCLIB_H
