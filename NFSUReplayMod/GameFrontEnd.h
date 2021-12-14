#pragma once
#include "stdafx.h"
#include "R_math.h"

// size 0x20?
struct RaceOptions {
	DWORD numberOfRacingCars;
	DWORD unk0x4;				// +0x4 / =1
	DWORD SkipFETrackNumber;	// +0x8
	DWORD isBackwardRaceTrack;	// +0xC
	DWORD lapsNumber;			// +0x10
	DWORD oponentsSkill;		// +0x14
	DWORD Traffic;				// +0x18
	BYTE unk0x1C[3];
	BYTE chatchUp;				// +0x1F
};

// size 0xB0C
struct FERaceEventStage {
	DWORD SkipFETrackNumber;	// +0x0
	DWORD isBackwardRaceTrack;	// +0x4
	DWORD lapsNumber;			// +0x8
	// +0x28 obj_size_0x15C [8]; (total size 0xAE0)
	// unk_size0xB0C* ?
};

// size totally unknown but it's in 0x19000 MemoryBlock, vtbl 0x6B7F7C
struct FERaceEvent {
	virtual void __thiscall Destroy(bool free);
	// void *vtbl;		// 11 entries
	// FERaceEventStage [8]; (total size 0x5860)
	// 0x5860 - continue
	// +0x5864 - ptr to this+4
	// +0x5868 DWORD stages number
	// +0x586C [4], size 0xC, total 0x30, destructor 0x4B39A0, constructor 0x4B39A0
	// +0x589C - continue
	// +0x58A0 - CarierRace
		// +0x58A8 - NumberOfOpponents (must be the same)
		// +0x59C8 - NumberOfOpponents
		// +0x59CC - float
		// +0x59D4 - TrafficLevel
		// +0x59DC - float [3] - OpponentSkill-relaible (=1)
	// +0x59EC - CarierRace end
};

// size 0x4
struct RaceStageInfo {
	WORD Tack;
	BYTE LapsNum;
	BYTE isBackwardRaceTrack;
};

// in memblock, size mb 0x14C
struct CarierRace {
	int number;
	// +0x10 float pledge? =0
	// +0x78 RaceStageInfo stages[8]; (total size 0x20)
	// +0x98 DWORD StagesNumber
};

struct FECareerManager {
	// +0x1374 - current money bank
	// +0x1378 - mb erned money
	// +0x137C - spended money
	// +0x88B0 - FERaceEvent*
};
enum CarierRacesStatus {
	Unavaible = 0,
	Avaible = 1,
	Complited = 2,
	InProgress = 3
};

struct cFEng;
struct FEngine;
struct FESomeObject;
struct FENode;
struct FEunk1;
struct FEObject;
struct FEObjectData;
struct FEList;
struct FEPackage;
struct FEPackageData;
struct FEngFont;
struct MenuScreen;

// size 0x10
struct cFEng {
	void* Vtable;				// 
	BYTE unk0x4;				// +0x4
	BYTE unk0x5;				// +0x5
	BYTE unk0x6;				// +0x6
	BYTE unk0x7;				// +0x7
	FEngine* feng;				// +0x8
	void* unk_obj0xC;			// +0xC / size 0x3420
};

// size 0x5254
// old name "unkFEobj1"
struct FEngine {
	bool flags[5];
	BYTE align0x5[3];

	void* unk_Lobj1;			// +0x8, size 0x110, no vtlb

	// +0x84
	// +0xD4 number (of iterations)
	// +0x4114 FEList with FESomeObject's

};

// size 0x20
struct FESomeObject {
	void* vtlb;					// +0x0 / 0x6C2C68
	FESomeObject* next;
	FESomeObject* prev;
	FEObject* unkFEobj;			// +0xC
	FEObject* unkFEobj2;		// +0x10, may be -5
	FEPackage* pkg;				// +0x14
	BYTE unk0x18[4];
	int action_num;				// +0x1C
};

// size ??
struct FENode {
	void* vtlb;
	FENode* next;				// +0x4
	FENode* prev;				// +0x8
	//
	char* name;					// +0xC, (.fng)
	DWORD hash;					// +0x10
	// mb
	DWORD idk;					// +0x14
	int number;					// +0x18
};

// old name FEobj_vt_0x6C2C4C
// contains literaly any object
// size 0x10, as part of obj, vtlb 0x6C2C4C
struct FEList {
	void* vtlb;
	int nodes_number;			// +0x4 / =32,4, 3, 5 (elem number?)
	void* first;				// +0x8
	void* last;					// +0xC
};

// mb FEScript
// size 0x34, in block size 0x6A0 (const)
struct FEunk1 {
	void* vtlb;					// +0x0 / 0x6C17F0
	FEunk1 *next;				// +0x4
	FEunk1* prev;				// +0x8
	// +0x20 - vector?
	BYTE unk0x4[0x20];
	char* str;					// +0x2C
	DWORD hash;					// +0x30
};

// not sure
enum mbFEObjectType {
	None = 0x0,
	Image = 0x1,
	String = 0x2,
	Model = 0x3,
	List = 0x4,
	Group = 0x5,
	CodeList = 0x6,
	Movie = 0x7,
	Effect = 0x8,
	ColoredImage = 0x9,
	AnimImage = 0xA,
	SimpleImage = 0xB,
	MultiImage = 0xC,
};

// size 0x58, vtlb 0x6C1810 (inh. 0x6C17E8)
struct FEObject {
	struct FEObjectFlags {
		unsigned isInvisible : 1;	// 0
		unsigned unk1 : 2;	// 1 - 2
		unsigned PCOnly : 1;	// 3
		unsigned unk4 : 2;	// 4 - 5
		unsigned ConsoleOnly : 1;	// 6
		unsigned unk7 : 10;	// 7 - 16
		unsigned MouseObject : 1;	// 17
		unsigned SaveStaticTracks : 1;	// 18
		unsigned DontNavigate : 1;	// 19
		unsigned UsesLibraryObject : 1;	// 20
		unsigned CodeSuppliedResource : 1;	// 21
		unsigned DirtyCode : 1;	// 22
		unsigned DirtyColor : 1;	// 23
		unsigned DirtyTransform : 1;	// 24
		unsigned Dirty : 1;	// 25
		unsigned IgnoreButton : 1;	// 26
		unsigned ObjectLocked : 1;	// 27
		unsigned IsButton : 1;	// 28
		unsigned PerspectiveProjection : 1;	// 29
		unsigned AffectAllScripts : 1;	// 30
		unsigned HideInEdit : 1;	// 31
	};
	void* vtlb;					// +0x0 / ///0x6C17D8 (2 entr. mb, destructor and ??), 0x6C1834 size 0x68 (inhr.)
	FEObject* next;				// +0x4
	FEObject* prev;				// +0x8
	int ID;						// +0xC
	DWORD hash;					// +0x10
	char* fe_name;				// +0x14 
	mbFEObjectType type;		// +0x18 / enum, 2 - FEString
	FEObjectFlags flags;		// +0x1C bitset of flags (visible including mb | 0x400000)
	BYTE unk[0xC];				// +0x20
	FEObjectData* data;			// +0x2C / in block size 0x880 (not a const), inh. incuding
	DWORD ObjectDataSize;		// +0x30 / =0x44, 0x54
	FEList l1;					// +0x34 / [mb FEList Scripts] FEObjects, 
	FEList l2;					// +0x44 / [mb FEList MessageResponses] FEunk1, 
	FEunk1* feunk1;				// +0x54 /	
};

struct color4x32 {
	unsigned int r, g, b, a;
};

// raw, check
// size 0x44
struct FEObjectData {
	color4x32 color;
	vector3 pivot;			// +0x10
	vector3 Positon;		// +0x1C
	Quanternion rotation;	// +0x28
	vector3 size;			// +0x38
};

// size 0x54
struct FEImageData : FEObjectData {
	vector2 UpperLeft;		// +0x44
	vector2 LowerRight;		// +0x4C
};

enum TextFormat {
	JustifyHorizontalCenter = 0x0, // left
	JustifyHorizontalRight = 0x1, // hor_cent
	JustifyVerticalCenter = 0x2, // right
	JustifyVerticalBottom = 0x3, // hor_cent
	WordWrap = 0x4, // left_bott
	// 5 = 101- hor_cent_bott
	// 6 = 110 - right bott
};

// old name FEObject2, FEButton (mistaken)
// size 0x74, vtlb 0x6C17C8, from 0x501630
struct FEString : FEObject {
	char *alt_name;				// +0x58 / or something
	DWORD alt_name_hash;		// +0x5C
	char* mbTranslatedString;	// +0x60 / mb UTF-8 or UTF-16 size 0x20 (not const), from 0x50410A
	int mbMaxLenght;			// +0x64 / 
	TextFormat format;			// +0x68
	BYTE unk0x6C[0x8];			// +0x6C / =0
};

// old name FEObject1
// size 0x5C vtlb 0x6C17D8, from 0x5016B5
struct FEImage : FEObject {
	// FEImageData
	DWORD mbImageFlags;			// +0x58 / =0
};

// old name FEObject3
// size 0x68, vtlb 0x6C1834, from 0x501699
struct FEGroup :FEObject {
	FEList FEObjects;			// +0x58
};

// size 0x24
struct InputNodes {
	InputNodes* next;
	InputNodes* prev;						// +0x4 / 0x73B260 - end
	DWORD code;								// +0x8
	void* unk0xC;							// +0xC / .data
	void* Handler;							// +0x10
	cFEng* cFEng;							// +0x14
	DWORD unk0x0x18;						// +0x18
	DWORD unk0x1C[2];						// +0x1C / =0
};

// check
namespace ResourceType {
	enum ResourceType {
		None = 0x0,
		Image = 0x1,
		Font = 0x2,
		Model = 0x3,
		Movie = 0x4,
		Effect = 0x5,
		AnimatedImage = 0x6,
		MultiImage = 0x7,
	};
}

// size 0x18, no vtlb, in vector
struct ResourceRequest {
	DWORD mbHash;
	char *res_path;				// +0x4
	ResourceType::ResourceType type;			// +0x8
	DWORD unk0xC;				// +0xC / =0
	DWORD mbHash2;				// different
	DWORD unk0x14;				// +0x14 / =0
};

// size 0x10
struct FEunk2 {
	DWORD mbHash;				// +0x0 / for ex - 0xABC08912
	int num_of_arr;				// +0x4 / =0x12
	int num_of_arr2;			// +0x8 / =0x12, same (mb allocated / reserved?)
	FEObject* feobjs_arr;		// +0xC / 
};

// inhirited from FENode
// size 0xA0, vtlb 0x6C17F4
struct FEPackage {
	void* vtlb;
	FEPackage* mbNext;			// +0x4
	FEPackage* mbPrev;			// +0x8
	char *fng;					// +0xC / heap
	DWORD hash;					// +0x10
	int type;					// +0x14 / =1 (mb Resourse Request)
	BYTE unk0x10[0xC];
	FEngine* feng;				// +0x24 / Parent?
	DWORD unk0x28;				// +0x28 / =14, 18, 0
	char* fng_fullpath;			// +0x2C
	BYTE unk0x30[0x8];			// +0x30 / =0
	FEList Objects;				// +0x38
	FEList l2;					// +0x48 / obj_vt_0x6C181C
	int number_of_RRqs;			// +0x58 / =9
	ResourceRequest* RRq;		// +0x5C / array of objs size 0x18, no vtlb
	int number_of_feunk;		// +0x60
	FEunk2* feunk;				// +0x64 / prt to arr, [feunk - 4] = number_of_feunk

	void *vtlb0x6C2C48;			// +0x68 / some object starts here, size unknown, vtlb near FEList's vtlb, FEList inhirited
	DWORD unk0x6C[6];			// +0x6C
	// +0x80 FEObject, inhirited including
	FEString* mbMenuOpts;		// +0x84 / array
	int NumOf_mbMenuOpts;		// +0x88
	char* idk;					// +0x8C / path to White16x16.bmp
	FEList l3;					// +0x90 / empty
};

// size 0x18, vtlb 0x6C181C, in block size 0x620, from 0x4FE5AB
struct obj_vt_0x6C181C {
	void* vtlb;
	obj_vt_0x6C181C *next;		// +0x4
	obj_vt_0x6C181C *prev;		// +0x8
	DWORD mbHash;				// +0xC
	int unk0x10;				// +0x10 / =1
	void* unk0x14;				// +0x14 / ptr (+4) in block size 0x10 (not const), from 0x4FE43F
};

// size 0x2C, vtlb 0x6C1EAC
struct FEPackageData {
	void* vtlb;					// 0x6C1EAC (1 entry)
	void* mbNext_plus4;			// +0x4, 0x746108 mb end
	void* mbprev_plus4;			// +0x8, 
	BYTE unk0xC[0x8];
	MenuScreen* screen;			// +0x14, =GarageMainScreen, SplashScreen, 0
	FEPackage* FEpkg;			// +0x18
	BYTE unk0x1C[0x10];			// +0x1C
};

// size 0x30, no vtlb
struct FEngFont {
	FEngFont* mbnext;			// 0x7460FC, mb end
	FEngFont* mbprev;			// +0x4
	void* unk_obj0x8;			// +0x8, from pool size 0x19000, from 0x5656C8, no vtlb
	void* unk_obj0xC;			// +0xC, same
	// ...
};

// no vtlb, size ?? (mb 0x10)
struct ScreenConstructorData {
	char *fng;
	FEPackage* FEpkg;
	void* idk; // .data
	DWORD unk;			// +0xC, =0
};

// size 0x44 (nope), vtlb 0x6C1874 (original), abstract i guess
struct MenuScreen {
	virtual void __thiscall Destroy(bool free);
	virtual DWORD __thiscall unk(DWORD);
	virtual void __thiscall NotificationMessage(DWORD hash, FEObject*, DWORD, DWORD);
	virtual void __thiscall NotifySoundMessage(DWORD, FEObject*, DWORD, DWORD, void* FrontendSoundEvent);
	//void* vtlb;					//  
	DWORD unk0x4;				// +0x4 / =BAADF000 / mb byte
	DWORD hash;					// +0x8 
	char *fng;					// +0xC

	// ScreenConstructorData??
	char *fng2;					// +0x10
	FEPackage* FEpkg;			// +0x14
	void *unk_0x18;				// +0x18  .data
	DWORD unk0x1C;				// +0x1C
	DWORD unk0x20;				// +0x20 / =BAADF000 / mb byte
	// +0x24 (+0x80 from FEPackage)
	// +0x28 FEObject* (=0)

	// +0x40 (not in MenuScreen) FEString* (description), =2 (vtlb 0x6C3124)
};

// vtbl 0x6C52E0, size 0x58, from 0x4C641D
struct UndergroundBriefScreen : MenuScreen {
	// +0x40 - TumbnailScroller(Node*?) {*next, *prev, void* (in mem block size 0x17DA1C)} size 0xC, from 0x4C4B37
};

// size 0x164, vtlb 0x6C3124 (4 entries)
struct mbMenuBar : MenuScreen {
	// +0x40 =2 number of FEImage
	FEImage* image[2/*?*/];				// +0x44 / Quit_button, Back_button

	// +0x144 bool []
};

// size 0x74, vtlb 0x6C3BE0
struct ConfirmDialog : MenuScreen {
	// +0x4C cha* parent fng
	// +0x50 parent hash
	// +0x58 parent hash
};

// vtlb 0x6C614C
struct GameMainMenuScreen : MenuScreen {
	DWORD Time;					// +0x44

};

// size 0x128
struct GameMainScreen {
	void* vtlb;
	// +0x40 obj* size 0x9B0, no vtlb
	// +0x44 same


	// +0x90 DWORD HandleShowPackage
	// +0x114 GarageCarLoader*
};

// size 0x74, vtlb 0x6C41C8
struct PauseMenu : MenuScreen {
	void* FuncsProcButtons[4];			// +0x44 / void _thiscall PauseMenu::ProcButton() {Resume, Resart, Options, LeaveRace}
	// +0x6D bool Restaart?
	// +0x70 bool is ScriptRunning or something
};


enum StringCodes :DWORD {
	StylePoints = 0x4D88440C,		// Style Points
	Steering_right = 0xDFE58E2F,	// Steering (right)
	Brake_Reverse = 0x87AF4E51,		// Brake/Reverse
	Nitrous_Boost = 0x6446DF0B,		// Nitrous Boost
	// check: res = -1; for (i = 0; str[i] != 0; i++) res = res * 0x21 + str[i];
};


FERaceEvent** const RaceEventMemory = (FERaceEvent**)0x735EC4;
FERaceEvent** const pFERaceEvent = (FERaceEvent**)0x7677A8;
FECareerManager* const CareerManager = (FECareerManager*)0x75EEF8;