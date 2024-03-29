#include "stdafx.h"
#include "Hooks.h"
#include "../NFSUReplayModeSettings/commoncode.h"
#include "MemWriter.h"

// CRT bypass
extern "C" int _fltused;
int _fltused;

void ReplaceAddrs(AddrReplaceStruct* ars, int num) {
	for (int i = 0; i < num; i++) {
		ars[i].backup = WriteProtectedMemEIPRelativeAdderess_ReAddr((int*)ars[i].place, ars[i].newAddr);
	}
}

struct BytesToReplace {
	void* place;
	WORD val_table_offset;
	WORD size;
};

void ByteReplace(const BytesToReplace* btr, size_t count, const BYTE* Table) {
	for (size_t i = 0; i < count; i++) {
		WriteProtectedMem(btr[i].place, Table + btr[i].val_table_offset, btr[i].size);
	}
}
#define MakeTableOffset(Addr) ((BYTE*)(Addr) - (BYTE*)&Table ) // causing dynamic Initialazing, fuck
#define FillOffsetSize(Field) MakeTableOffset(Field), sizeof(Field)

#pragma pack(push, 1)
struct BytesTable {
	BYTE call_near_opcode[1];
	int HookEndSceneRA;
	BYTE nop_opcode[1];
	
	BYTE call_near_opcode2[1];
	int HookResetRA;
	BYTE jmp_short11[2];

	BYTE pref_jmp_near[2];
	BYTE nop5bytes[5];
};
#pragma pack(pop)

BytesTable Table = { 
	{ 0xE8 },
	  0,
	{ 0x90 },

	{ 0xE8 },
	  0,
	{ 0xEB, 0x09 },

	{ 0x2E /* cs prefix to make instruction 6 bytes long, better than nop on most CPUs */, 0xE9 /* jmp near */ },
	{ 0x0F, 0x1F, 0x44, 0x00, 0x00 }
};
const BytesToReplace BTR[] = {
	{ call_IDirect3DDevice9_EndScene,		MakeTableOffset(Table.call_near_opcode), 6 },
	{ before_call_IDirect3DDevice9_Reset,	MakeTableOffset(Table.call_near_opcode2), 7 },

	{ (void*)0x44BA0C,						FillOffsetSize(Table.pref_jmp_near) },
	{ (void*)0x45C3C7,						FillOffsetSize(Table.nop5bytes) } // call bypass (causing crash)
};
void Init() {
	replayer.Init();
	LoadSettings();

	ReplaceAddrs(ToReplace, countofToReplace);

	Table.HookEndSceneRA = GetRelativeAddress((int*)((BYTE*)call_IDirect3DDevice9_EndScene + 1), HookEndScene);
	Table.HookResetRA = GetRelativeAddress((int*)((BYTE*)before_call_IDirect3DDevice9_Reset + 1), BeforeReset);

	ByteReplace(BTR, _countof(BTR), (BYTE*)&Table);
}

void Detach() {
	replayer.Release();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
	if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
		uintptr_t base = (uintptr_t)GetModuleHandleA(NULL);
		IMAGE_DOS_HEADER* dos = (IMAGE_DOS_HEADER*)(base);
		IMAGE_NT_HEADERS* nt = (IMAGE_NT_HEADERS*)(base + dos->e_lfanew);
		if ((base + nt->OptionalHeader.AddressOfEntryPoint + (0x400000 - base)) == 0x670CB5) { // Check if .exe file is compatible - Thanks to thelink2012 and MWisBest
			Init();
		}
		else {
			MessageBoxA(NULL, "This .exe is not supported.\n" "Please use v1.4 English speed.exe (3,03 MB (3.178.496 bytes)).", 
				"NFSU Replay Mod", MB_ICONERROR);
			return FALSE;
		}
	}
	else {
		if (ul_reason_for_call == DLL_PROCESS_DETACH) {
			Detach(); // just in case
		}
	}
	return TRUE;
}