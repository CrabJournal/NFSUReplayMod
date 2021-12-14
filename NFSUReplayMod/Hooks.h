#pragma once

#include "Replayer.h"

extern Replayer replayer;

extern "C" int _atexit(void(__cdecl *func)(void));

HRESULT _stdcall HookEndScene(IDirect3DDevice9* d3ddevice);
HRESULT BeforeReset();

struct AddrReplaceStruct {
	void *place;
	void *newAddr;
	void *backup;
};

extern const size_t countofToReplace;
extern AddrReplaceStruct ToReplace[];