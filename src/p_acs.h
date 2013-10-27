/*
** p_acs.h
** ACS script stuff
**
**---------------------------------------------------------------------------
** Copyright 1998-2008 Randy Heit
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. The name of the author may not be used to endorse or promote products
**    derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**---------------------------------------------------------------------------
**
*/

#ifndef __P_ACS_H__
#define __P_ACS_H__

#include "dobject.h"
#include "dthinker.h"
#include "doomtype.h"

#define LOCAL_SIZE				20
#define NUM_MAPVARS				128

class FFont;
class FileReader;


enum
{
	NUM_WORLDVARS = 256,
	NUM_GLOBALVARS = 64
};

struct InitIntToZero
{
	void Init(int &v)
	{
		v = 0;
	}
};
typedef TMap<SDWORD, SDWORD, THashTraits<SDWORD>, InitIntToZero> FWorldGlobalArray;

// ACS variables with world scope
extern SDWORD ACS_WorldVars[NUM_WORLDVARS];
extern FWorldGlobalArray ACS_WorldArrays[NUM_WORLDVARS];

// ACS variables with global scope
extern SDWORD ACS_GlobalVars[NUM_GLOBALVARS];
extern FWorldGlobalArray ACS_GlobalArrays[NUM_GLOBALVARS];

void P_ReadACSVars(PNGHandle *);
void P_WriteACSVars(FILE*);
void P_ClearACSVars(bool);

// The in-memory version
struct ScriptPtr
{
	WORD Number;
	BYTE Type;
	BYTE ArgCount;
	WORD VarCount;
	WORD Flags;
	DWORD Address;
};

// The present ZDoom version
struct ScriptPtr3
{
	WORD Number;
	BYTE Type;
	BYTE ArgCount;
	DWORD Address;
};

// The intermediate ZDoom version
struct ScriptPtr1
{
	WORD Number;
	WORD Type;
	DWORD Address;
	DWORD ArgCount;
};

// The old Hexen version
struct ScriptPtr2
{
	DWORD Number;	// Type is Number / 1000
	DWORD Address;
	DWORD ArgCount;
};

struct ScriptFlagsPtr
{
	WORD Number;
	WORD Flags;
};

struct ScriptFunction
{
	BYTE ArgCount;
	BYTE LocalCount;
	BYTE HasReturnValue;
	BYTE ImportNum;
	DWORD Address;
};

// Script types
enum
{
	SCRIPT_Closed		= 0,
	SCRIPT_Open			= 1,
	SCRIPT_Respawn		= 2,
	SCRIPT_Death		= 3,
	SCRIPT_Enter		= 4,
	SCRIPT_Pickup		= 5,
	SCRIPT_BlueReturn	= 6,
	SCRIPT_RedReturn	= 7,
	SCRIPT_WhiteReturn	= 8,
	SCRIPT_Lightning	= 12,
	SCRIPT_Unloading	= 13,
	SCRIPT_Disconnect	= 14,
	SCRIPT_Return		= 15,
};

// Script flags
enum
{
	SCRIPTF_Net = 0x0001,	// Safe to "puke" in multiplayer
	SCRIPTF_ClientSide = 0x0002	// [BB] Is executed on the clients, not on the server.
};

enum ACSFormat { ACS_Old, ACS_Enhanced, ACS_LittleEnhanced, ACS_Unknown };

// [BB] Moved here from p_acs.cpp
enum
{
	APROP_Health		= 0,
	APROP_Speed			= 1,
	APROP_Damage		= 2,
	APROP_Alpha			= 3,
	APROP_RenderStyle	= 4,
	APROP_SeeSound		= 5,	// Sounds can only be set, not gotten
	APROP_AttackSound	= 6,
	APROP_PainSound		= 7,
	APROP_DeathSound	= 8,
	APROP_ActiveSound	= 9,
	APROP_Ambush		= 10,
	APROP_Invulnerable	= 11,
	APROP_JumpZ			= 12,	// [GRB]
	APROP_ChaseGoal		= 13,
	APROP_Frightened	= 14,
	APROP_Gravity		= 15,
	APROP_Friendly		= 16,
	APROP_SpawnHealth   = 17,
	APROP_Dropped		= 18,
	APROP_Notarget		= 19,
	APROP_Species		= 20,
	APROP_NameTag		= 21,
	APROP_Score			= 22,
	APROP_Notrigger		= 23,
	APROP_DamageFactor	= 24,
};	

// [Dusk] Enumeration for GetTeamProperty
enum
{
	TPROP_Name = 0,
	TPROP_Score,
	TPROP_IsValid,
	TPROP_NumPlayers,
	TPROP_NumLivePlayers,
	TPROP_TextColor,
	TPROP_PlayerStartNum,
	TPROP_Spread,
	TPROP_Carrier,
	TPROP_Assister,
	TPROP_FragCount,
	TPROP_DeathCount,
	TPROP_WinCount,
	TPROP_PointCount,
	TPROP_ReturnTics,
	TPROP_TeamItem,
	TPROP_WinnerTheme,
	TPROP_LoserTheme,
};

class FBehavior
{
public:
	FBehavior (int lumpnum, FileReader * fr=NULL, int len=0);
	~FBehavior ();

	bool IsGood ();
	BYTE *FindChunk (DWORD id) const;
	BYTE *NextChunk (BYTE *chunk) const;
	const ScriptPtr *FindScript (int number) const;
	void StartTypedScripts (WORD type, AActor *activator, bool always, int arg1, bool runNow, bool onlyClientSideScripts=false);
	int CountTypedScripts( WORD type );
	DWORD PC2Ofs (int *pc) const { return (DWORD)((BYTE *)pc - Data); }
	int *Ofs2PC (DWORD ofs) const {	return (int *)(Data + ofs); }
	ACSFormat GetFormat() const { return Format; }
	ScriptFunction *GetFunction (int funcnum, FBehavior *&module) const;
	int GetArrayVal (int arraynum, int index) const;
	void SetArrayVal (int arraynum, int index, int value);
	int FindFunctionName (const char *funcname) const;
	int FindMapVarName (const char *varname) const;
	int FindMapArray (const char *arrayname) const;
	int GetLibraryID () const { return LibraryID; }
	int *GetScriptAddress (const ScriptPtr *ptr) const { return (int *)(ptr->Address + Data); }

	SDWORD *MapVars[NUM_MAPVARS];

	static FBehavior *StaticLoadModule (int lumpnum, FileReader * fr=NULL, int len=0);
	static void StaticLoadDefaultModules ();
	static void StaticUnloadModules ();
	static bool StaticCheckAllGood ();
	static FBehavior *StaticGetModule (int lib);
	static void StaticSerializeModuleStates (FArchive &arc);

	static const ScriptPtr *StaticFindScript (int script, FBehavior *&module);
	static const char *StaticLookupString (DWORD index);
	static void StaticStartTypedScripts (WORD type, AActor *activator, bool always, int arg1=0, bool runNow=false, bool onlyClientSideScripts=false);
	static void StaticStopMyScripts (AActor *actor);
	static int StaticCountTypedScripts( WORD type );

private:
	struct ArrayInfo;

	ACSFormat Format;

	int LumpNum;
	BYTE *Data;
	int DataSize;
	BYTE *Chunks;
	ScriptPtr *Scripts;
	int NumScripts;
	BYTE *Functions;
	int NumFunctions;
	ArrayInfo *ArrayStore;
	int NumArrays;
	ArrayInfo **Arrays;
	int NumTotalArrays;
	DWORD StringTable;
	SDWORD MapVarStore[NUM_MAPVARS];
	TArray<FBehavior *> Imports;
	DWORD LibraryID;
	char ModuleName[9];

	static TArray<FBehavior *> StaticModules;

	void LoadScriptsDirectory ();

	static int STACK_ARGS SortScripts (const void *a, const void *b);
	void UnencryptStrings ();
	int FindStringInChunk (DWORD *chunk, const char *varname) const;
	const char *LookupString (DWORD index) const;

	void SerializeVars (FArchive &arc);
	void SerializeVarSet (FArchive &arc, SDWORD *vars, int max);
};

class DLevelScript : public DObject
{
	DECLARE_CLASS (DLevelScript, DObject)
	HAS_OBJECT_POINTERS
public:

	// P-codes for ACS scripts
	enum
	{
/*  0*/	PCD_NOP,
		PCD_TERMINATE,
		PCD_SUSPEND,
		PCD_PUSHNUMBER,
		PCD_LSPEC1,
		PCD_LSPEC2,
		PCD_LSPEC3,
		PCD_LSPEC4,
		PCD_LSPEC5,
		PCD_LSPEC1DIRECT,
/* 10*/	PCD_LSPEC2DIRECT,
		PCD_LSPEC3DIRECT,
		PCD_LSPEC4DIRECT,
		PCD_LSPEC5DIRECT,
		PCD_ADD,
		PCD_SUBTRACT,
		PCD_MULTIPLY,
		PCD_DIVIDE,
		PCD_MODULUS,
		PCD_EQ,
/* 20*/ PCD_NE,
		PCD_LT,
		PCD_GT,
		PCD_LE,
		PCD_GE,
		PCD_ASSIGNSCRIPTVAR,
		PCD_ASSIGNMAPVAR,
		PCD_ASSIGNWORLDVAR,
		PCD_PUSHSCRIPTVAR,
		PCD_PUSHMAPVAR,
/* 30*/	PCD_PUSHWORLDVAR,
		PCD_ADDSCRIPTVAR,
		PCD_ADDMAPVAR,
		PCD_ADDWORLDVAR,
		PCD_SUBSCRIPTVAR,
		PCD_SUBMAPVAR,
		PCD_SUBWORLDVAR,
		PCD_MULSCRIPTVAR,
		PCD_MULMAPVAR,
		PCD_MULWORLDVAR,
/* 40*/	PCD_DIVSCRIPTVAR,
		PCD_DIVMAPVAR,
		PCD_DIVWORLDVAR,
		PCD_MODSCRIPTVAR,
		PCD_MODMAPVAR,
		PCD_MODWORLDVAR,
		PCD_INCSCRIPTVAR,
		PCD_INCMAPVAR,
		PCD_INCWORLDVAR,
		PCD_DECSCRIPTVAR,
/* 50*/	PCD_DECMAPVAR,
		PCD_DECWORLDVAR,
		PCD_GOTO,
		PCD_IFGOTO,
		PCD_DROP,
		PCD_DELAY,
		PCD_DELAYDIRECT,
		PCD_RANDOM,
		PCD_RANDOMDIRECT,
		PCD_THINGCOUNT,
/* 60*/	PCD_THINGCOUNTDIRECT,
		PCD_TAGWAIT,
		PCD_TAGWAITDIRECT,
		PCD_POLYWAIT,
		PCD_POLYWAITDIRECT,
		PCD_CHANGEFLOOR,
		PCD_CHANGEFLOORDIRECT,
		PCD_CHANGECEILING,
		PCD_CHANGECEILINGDIRECT,
		PCD_RESTART,
/* 70*/	PCD_ANDLOGICAL,
		PCD_ORLOGICAL,
		PCD_ANDBITWISE,
		PCD_ORBITWISE,
		PCD_EORBITWISE,
		PCD_NEGATELOGICAL,
		PCD_LSHIFT,
		PCD_RSHIFT,
		PCD_UNARYMINUS,
		PCD_IFNOTGOTO,
/* 80*/	PCD_LINESIDE,
		PCD_SCRIPTWAIT,
		PCD_SCRIPTWAITDIRECT,
		PCD_CLEARLINESPECIAL,
		PCD_CASEGOTO,
		PCD_BEGINPRINT,
		PCD_ENDPRINT,
		PCD_PRINTSTRING,
		PCD_PRINTNUMBER,
		PCD_PRINTCHARACTER,
/* 90*/	PCD_PLAYERCOUNT,
		PCD_GAMETYPE,
		PCD_GAMESKILL,
		PCD_TIMER,
		PCD_SECTORSOUND,
		PCD_AMBIENTSOUND,
		PCD_SOUNDSEQUENCE,
		PCD_SETLINETEXTURE,
		PCD_SETLINEBLOCKING,
		PCD_SETLINESPECIAL,
/*100*/	PCD_THINGSOUND,
		PCD_ENDPRINTBOLD,		// [RH] End of Hexen p-codes
		PCD_ACTIVATORSOUND,
		PCD_LOCALAMBIENTSOUND,
		PCD_SETLINEMONSTERBLOCKING,
		PCD_PLAYERBLUESKULL,	// [BC] Start of new [Skull Tag] pcodes
		PCD_PLAYERREDSKULL,
		PCD_PLAYERYELLOWSKULL,
		PCD_PLAYERMASTERSKULL,
		PCD_PLAYERBLUECARD,
/*110*/	PCD_PLAYERREDCARD,
		PCD_PLAYERYELLOWCARD,
		PCD_PLAYERMASTERCARD,
		PCD_PLAYERBLACKSKULL,
		PCD_PLAYERSILVERSKULL,
		PCD_PLAYERGOLDSKULL,
		PCD_PLAYERBLACKCARD,
		PCD_PLAYERSILVERCARD,
		PCD_ISMULTIPLAYER,
		PCD_PLAYERTEAM,
/*120*/	PCD_PLAYERHEALTH,
		PCD_PLAYERARMORPOINTS,
		PCD_PLAYERFRAGS,
		PCD_PLAYEREXPERT,
		PCD_BLUETEAMCOUNT,
		PCD_REDTEAMCOUNT,
		PCD_BLUETEAMSCORE,
		PCD_REDTEAMSCORE,
		PCD_ISONEFLAGCTF,
		PCD_GETINVASIONWAVE,
/*130*/	PCD_GETINVASIONSTATE,
		PCD_PRINTNAME,
		PCD_MUSICCHANGE,
		PCD_CONSOLECOMMANDDIRECT,
		PCD_CONSOLECOMMAND,
		PCD_SINGLEPLAYER,		// [RH] End of Skull Tag p-codes
		PCD_FIXEDMUL,
		PCD_FIXEDDIV,
		PCD_SETGRAVITY,
		PCD_SETGRAVITYDIRECT,
/*140*/	PCD_SETAIRCONTROL,
		PCD_SETAIRCONTROLDIRECT,
		PCD_CLEARINVENTORY,
		PCD_GIVEINVENTORY,
		PCD_GIVEINVENTORYDIRECT,
		PCD_TAKEINVENTORY,
		PCD_TAKEINVENTORYDIRECT,
		PCD_CHECKINVENTORY,
		PCD_CHECKINVENTORYDIRECT,
		PCD_SPAWN,
/*150*/	PCD_SPAWNDIRECT,
		PCD_SPAWNSPOT,
		PCD_SPAWNSPOTDIRECT,
		PCD_SETMUSIC,
		PCD_SETMUSICDIRECT,
		PCD_LOCALSETMUSIC,
		PCD_LOCALSETMUSICDIRECT,
		PCD_PRINTFIXED,
		PCD_PRINTLOCALIZED,
		PCD_MOREHUDMESSAGE,
/*160*/	PCD_OPTHUDMESSAGE,
		PCD_ENDHUDMESSAGE,
		PCD_ENDHUDMESSAGEBOLD,
		PCD_SETSTYLE,
		PCD_SETSTYLEDIRECT,
		PCD_SETFONT,
		PCD_SETFONTDIRECT,
		PCD_PUSHBYTE,
		PCD_LSPEC1DIRECTB,
		PCD_LSPEC2DIRECTB,
/*170*/	PCD_LSPEC3DIRECTB,
		PCD_LSPEC4DIRECTB,
		PCD_LSPEC5DIRECTB,
		PCD_DELAYDIRECTB,
		PCD_RANDOMDIRECTB,
		PCD_PUSHBYTES,
		PCD_PUSH2BYTES,
		PCD_PUSH3BYTES,
		PCD_PUSH4BYTES,
		PCD_PUSH5BYTES,
/*180*/	PCD_SETTHINGSPECIAL,
		PCD_ASSIGNGLOBALVAR,
		PCD_PUSHGLOBALVAR,
		PCD_ADDGLOBALVAR,
		PCD_SUBGLOBALVAR,
		PCD_MULGLOBALVAR,
		PCD_DIVGLOBALVAR,
		PCD_MODGLOBALVAR,
		PCD_INCGLOBALVAR,
		PCD_DECGLOBALVAR,
/*190*/	PCD_FADETO,
		PCD_FADERANGE,
		PCD_CANCELFADE,
		PCD_PLAYMOVIE,
		PCD_SETFLOORTRIGGER,
		PCD_SETCEILINGTRIGGER,
		PCD_GETACTORX,
		PCD_GETACTORY,
		PCD_GETACTORZ,
		PCD_STARTTRANSLATION,
/*200*/	PCD_TRANSLATIONRANGE1,
		PCD_TRANSLATIONRANGE2,
		PCD_ENDTRANSLATION,
		PCD_CALL,
		PCD_CALLDISCARD,
		PCD_RETURNVOID,
		PCD_RETURNVAL,
		PCD_PUSHMAPARRAY,
		PCD_ASSIGNMAPARRAY,
		PCD_ADDMAPARRAY,
/*210*/	PCD_SUBMAPARRAY,
		PCD_MULMAPARRAY,
		PCD_DIVMAPARRAY,
		PCD_MODMAPARRAY,
		PCD_INCMAPARRAY,
		PCD_DECMAPARRAY,
		PCD_DUP,
		PCD_SWAP,
		PCD_WRITETOINI,
		PCD_GETFROMINI,
/*220*/ PCD_SIN,
		PCD_COS,
		PCD_VECTORANGLE,
		PCD_CHECKWEAPON,
		PCD_SETWEAPON,
		PCD_TAGSTRING,
		PCD_PUSHWORLDARRAY,
		PCD_ASSIGNWORLDARRAY,
		PCD_ADDWORLDARRAY,
		PCD_SUBWORLDARRAY,
/*230*/	PCD_MULWORLDARRAY,
		PCD_DIVWORLDARRAY,
		PCD_MODWORLDARRAY,
		PCD_INCWORLDARRAY,
		PCD_DECWORLDARRAY,
		PCD_PUSHGLOBALARRAY,
		PCD_ASSIGNGLOBALARRAY,
		PCD_ADDGLOBALARRAY,
		PCD_SUBGLOBALARRAY,
		PCD_MULGLOBALARRAY,
/*240*/	PCD_DIVGLOBALARRAY,
		PCD_MODGLOBALARRAY,
		PCD_INCGLOBALARRAY,
		PCD_DECGLOBALARRAY,
		PCD_SETMARINEWEAPON,
		PCD_SETACTORPROPERTY,
		PCD_GETACTORPROPERTY,
		PCD_PLAYERNUMBER,
		PCD_ACTIVATORTID,
		PCD_SETMARINESPRITE,
/*250*/	PCD_GETSCREENWIDTH,
		PCD_GETSCREENHEIGHT,
		PCD_THING_PROJECTILE2,
		PCD_STRLEN,
		PCD_SETHUDSIZE,
		PCD_GETCVAR,
		PCD_CASEGOTOSORTED,
		PCD_SETRESULTVALUE,
		PCD_GETLINEROWOFFSET,
		PCD_GETACTORFLOORZ,
/*260*/	PCD_GETACTORANGLE,
		PCD_GETSECTORFLOORZ,
		PCD_GETSECTORCEILINGZ,
		PCD_LSPEC5RESULT,
		PCD_GETSIGILPIECES,
		PCD_GETLEVELINFO,
		PCD_CHANGESKY,
		PCD_PLAYERINGAME,
		PCD_PLAYERISBOT,
		PCD_SETCAMERATOTEXTURE,
/*270*/	PCD_ENDLOG,
		PCD_GETAMMOCAPACITY,
		PCD_SETAMMOCAPACITY,
		PCD_PRINTMAPCHARARRAY,		// [JB] start of new p-codes
		PCD_PRINTWORLDCHARARRAY,
		PCD_PRINTGLOBALCHARARRAY,	// [JB] end of new p-codes
		PCD_SETACTORANGLE,			// [GRB]
		PCD_GRABINPUT,				// Unused but acc defines them
		PCD_SETMOUSEPOINTER,		// "
		PCD_MOVEMOUSEPOINTER,		// "
/*280*/	PCD_SPAWNPROJECTILE,
		PCD_GETSECTORLIGHTLEVEL,
		PCD_GETACTORCEILINGZ,
		PCD_SETACTORPOSITION,
		PCD_CLEARACTORINVENTORY,
		PCD_GIVEACTORINVENTORY,
		PCD_TAKEACTORINVENTORY,
		PCD_CHECKACTORINVENTORY,
		PCD_THINGCOUNTNAME,
		PCD_SPAWNSPOTFACING,
/*290*/	PCD_PLAYERCLASS,			// [GRB]
		//[MW] start my p-codes
		PCD_ANDSCRIPTVAR,
		PCD_ANDMAPVAR, 
		PCD_ANDWORLDVAR, 
		PCD_ANDGLOBALVAR, 
		PCD_ANDMAPARRAY, 
		PCD_ANDWORLDARRAY, 
		PCD_ANDGLOBALARRAY,
		PCD_EORSCRIPTVAR, 
		PCD_EORMAPVAR, 
/*300*/	PCD_EORWORLDVAR, 
		PCD_EORGLOBALVAR, 
		PCD_EORMAPARRAY, 
		PCD_EORWORLDARRAY, 
		PCD_EORGLOBALARRAY,
		PCD_ORSCRIPTVAR, 
		PCD_ORMAPVAR, 
		PCD_ORWORLDVAR, 
		PCD_ORGLOBALVAR, 
		PCD_ORMAPARRAY, 
/*310*/	PCD_ORWORLDARRAY, 
		PCD_ORGLOBALARRAY,
		PCD_LSSCRIPTVAR, 
		PCD_LSMAPVAR, 
		PCD_LSWORLDVAR, 
		PCD_LSGLOBALVAR, 
		PCD_LSMAPARRAY, 
		PCD_LSWORLDARRAY, 
		PCD_LSGLOBALARRAY,
		PCD_RSSCRIPTVAR, 
/*320*/	PCD_RSMAPVAR, 
		PCD_RSWORLDVAR, 
		PCD_RSGLOBALVAR, 
		PCD_RSMAPARRAY, 
		PCD_RSWORLDARRAY, 
		PCD_RSGLOBALARRAY, 
		//[MW] end my p-codes
		PCD_GETPLAYERINFO,			// [GRB]
		PCD_CHANGELEVEL,
		PCD_SECTORDAMAGE,
		PCD_REPLACETEXTURES,
/*330*/	PCD_NEGATEBINARY,
		PCD_GETACTORPITCH,
		PCD_SETACTORPITCH,
		PCD_PRINTBIND,
		PCD_SETACTORSTATE,
		PCD_THINGDAMAGE2,
		PCD_USEINVENTORY,
		PCD_USEACTORINVENTORY,
		PCD_CHECKACTORCEILINGTEXTURE,
		PCD_CHECKACTORFLOORTEXTURE,
/*340*/	PCD_GETACTORLIGHTLEVEL,
		PCD_SETMUGSHOTSTATE,
		PCD_THINGCOUNTSECTOR,
		PCD_THINGCOUNTNAMESECTOR,
		PCD_CHECKPLAYERCAMERA,		// [TN]
		PCD_MORPHACTOR,				// [MH]
		PCD_UNMORPHACTOR,			// [MH]
		PCD_GETPLAYERINPUT,
		PCD_CLASSIFYACTOR,
		PCD_PRINTBINARY,
/*350*/	PCD_PRINTHEX,
		PCD_CALLFUNC,
		PCD_SAVESTRING,			// [FDARI]
		// [BB] We need to fix the number for the new commands!
		// [CW] Begin team additions.
		PCD_GETTEAMPLAYERCOUNT,
		// [CW] End team additions.

/*353*/	PCODE_COMMAND_COUNT
	};

	// Some constants used by ACS scripts
	enum {
		LINE_FRONT =			0,
		LINE_BACK =				1
	};
	enum {
		SIDE_FRONT =			0,
		SIDE_BACK =				1
	};
	enum {
		TEXTURE_TOP =			0,
		TEXTURE_MIDDLE =		1,
		TEXTURE_BOTTOM =		2
	};
	enum {
		GAME_SINGLE_PLAYER =	0,
		GAME_NET_COOPERATIVE =	1,
		GAME_NET_DEATHMATCH =	2,
		GAME_TITLE_MAP =		3,
		GAME_NET_TEAMGAME =		4,
	};
	enum {
		CLASS_FIGHTER =			0,
		CLASS_CLERIC =			1,
		CLASS_MAGE =			2
	};
	enum {
		SKILL_VERY_EASY =		0,
		SKILL_EASY =			1,
		SKILL_NORMAL =			2,
		SKILL_HARD =			3,
		SKILL_VERY_HARD =		4
	};
	enum {
		BLOCK_NOTHING =			0,
		BLOCK_CREATURES =		1,
		BLOCK_EVERYTHING =		2,
		BLOCK_RAILING =			3,
		BLOCK_PLAYERS =			4
	};
	enum {
		LEVELINFO_PAR_TIME,
		LEVELINFO_CLUSTERNUM,
		LEVELINFO_LEVELNUM,
		LEVELINFO_TOTAL_SECRETS,
		LEVELINFO_FOUND_SECRETS,
		LEVELINFO_TOTAL_ITEMS,
		LEVELINFO_FOUND_ITEMS,
		LEVELINFO_TOTAL_MONSTERS,
		LEVELINFO_KILLED_MONSTERS,
		LEVELINFO_SUCK_TIME
	};
	enum {
		PLAYERINFO_TEAM,
		PLAYERINFO_AIMDIST,
		PLAYERINFO_COLOR,
		PLAYERINFO_GENDER,
		PLAYERINFO_NEVERSWITCH,
		PLAYERINFO_MOVEBOB,
		PLAYERINFO_STILLBOB,
		PLAYERINFO_PLAYERCLASS
	};

	enum EScriptState
	{
		SCRIPT_Running,
		SCRIPT_Suspended,
		SCRIPT_Delayed,
		SCRIPT_TagWait,
		SCRIPT_PolyWait,
		SCRIPT_ScriptWaitPre,
		SCRIPT_ScriptWait,
		SCRIPT_PleaseRemove,
		SCRIPT_DivideBy0,
		SCRIPT_ModulusBy0,
	};

	DLevelScript (AActor *who, line_t *where, int num, const ScriptPtr *code, FBehavior *module,
		bool backSide, int arg0, int arg1, int arg2, int always);
	~DLevelScript ();

	void Serialize (FArchive &arc);
	int RunScript ();

	inline void SetState (EScriptState newstate) { state = newstate; }
	inline EScriptState GetState () { return state; }

protected:
	DLevelScript	*next, *prev;
	int				script;
	SDWORD			*localvars;
	int				numlocalvars;
	int				*pc;
	EScriptState	state;
	int				statedata;
	TObjPtr<AActor>	activator;
	line_t			*activationline;
	bool			backSide;
	FFont			*activefont;
	int				hudwidth, hudheight;
	FBehavior	    *activeBehavior;

	void Link ();
	void Unlink ();
	void PutLast ();
	void PutFirst ();
	static int Random (int min, int max);
	static int ThingCount (int type, int stringid, int tid, int tag);
	static void ChangeFlat (int tag, int name, bool floorOrCeiling);
	static int CountPlayers ();
	static void SetLineTexture (int lineid, int side, int position, int name);
	static void ReplaceTextures (int fromname, int toname, int flags);
	static int DoSpawn (int type, fixed_t x, fixed_t y, fixed_t z, int tid, int angle, bool force);
	static bool DoCheckActorTexture(int tid, AActor *activator, int string, bool floor);
	int DoSpawnSpot (int type, int spot, int tid, int angle, bool forced);
	int DoSpawnSpotFacing (int type, int spot, int tid, bool forced);
	int DoClassifyActor (int tid);
	int CallFunction(int argCount, int funcIndex, SDWORD *args);

	void DoFadeTo (int r, int g, int b, int a, fixed_t time);
	void DoFadeRange (int r1, int g1, int b1, int a1,
		int r2, int g2, int b2, int a2, fixed_t time);
	void DoSetFont (int fontnum);
	void SetActorProperty (int tid, int property, int value);
	void DoSetActorProperty (AActor *actor, int property, int value);
	int GetActorProperty (int tid, int property);
	int CheckActorProperty (int tid, int property, int value);
	int GetPlayerInput (int playernum, int inputnum);

	int LineFromID(int id);
	int SideFromID(int id, int side);

private:
	DLevelScript ();

	friend class DACSThinker;

	// [BB] The clients need to call some of the static functions from DLevelScript.
	friend class STClient;
};

inline FArchive &operator<< (FArchive &arc, DLevelScript::EScriptState &state)
{
	BYTE val = (BYTE)state;
	arc << val;
	state = (DLevelScript::EScriptState)val;
	return arc;
}

class DACSThinker : public DThinker
{
	DECLARE_CLASS (DACSThinker, DThinker)
	HAS_OBJECT_POINTERS
public:
	DACSThinker ();
	~DACSThinker ();

	void Serialize (FArchive &arc);
	void Tick ();

	DLevelScript *RunningScripts[1000];	// Array of all synchronous scripts
	static TObjPtr<DACSThinker> ActiveThinker;

	void DumpScriptStatus();
	void StopScriptsFor (AActor *actor);
	// [BB] Added StopAndDestroyAllScripts, which is needed in GAME_ResetMap.
	void StopAndDestroyAllScripts ();

private:
	DLevelScript *LastScript;
	DLevelScript *Scripts;				// List of all running scripts

	friend class DLevelScript;
};

// The structure used to control scripts between maps
struct acsdefered_t
{
	struct acsdefered_t *next;

	enum EType
	{
		defexecute,
		defexealways,
		defsuspend,
		defterminate
	} type;
	int script;
	int arg0, arg1, arg2;
	int playernum;
};

FArchive &operator<< (FArchive &arc, acsdefered_t *&defer);

//*****************************************************************************
//	PROTOTYPES

bool	ACS_IsCalledFromConsoleCommand( void );
bool	ACS_IsScriptClientSide( ULONG ulScript );
bool	ACS_IsScriptClientSide( const ScriptPtr *pScriptData );
bool	ACS_IsScriptPukeable( ULONG ulScript );
int		ACS_GetTranslationIndex( FRemapTable *pTranslation );

// [BL] Export DoClearInv
void	DoClearInv(AActor *actor);
// [BB] Export DoGiveInv
bool	DoGiveInv(AActor *actor, const PClass *info, int amount);

#endif //__P_ACS_H__
