#pragma once
#include "stdafx.h"
#include <d3d9.h>

// sizeof may be 0x3E0
struct Object3D {
	void *Vtable;
	Object3D *next_plus4;					// + 0x4
	Object3D *prev_plus4;					// + 0x8
	DWORD magic1_0x69676952;				// + 0xC
	DWORD magic2;							// + 0x10, oftenly = 0x64 or 0x100
	BYTE unk0x14[0xC];						// + 0x14
	vector3 position;						// + 0x20
	DWORD align0x2C;
	float RotationMatrixAligned[3][4];		// + 0x30
	BYTE unk0x60[0x10];						// + 0x60
	vector3a speed;							// + 0x70
	BYTE unk0x80[0x20];						// + 0x80
	float unk0x90;							// + 0x90
	BYTE align0x94[0xC];
	vector3a speed_multiplied;				// + 0xA0
	vector3a unk0xB0;						// + 0xB0				
	float unk0xC0[0x10];					// + 0xC0
	vector3a unk0x100;						// + 0x100
	vector3a unk0x110;						// + 0x110
	BYTE unk0x120[0x20];					// + 0x120
	DWORD flag_mb0x130;						// + 0x130
	DWORD flag_mb0x134;						// + 0x134
	DWORD flag_mb0x138;						// + 0x138
	BYTE unk0x13C[0x8];						// + 0x13C
	float magic3;							// + 0x144
	float magic3_inversed;					// + 0x148
	DWORD align0x14C;
	float unk0x150[0x10];					// + 0x150
	// and more
};

struct node_obj {
	node_obj* next;
	node_obj* prev;
	void* unk0x8;
	BYTE unk0xC[6];
	bool unk0x12;
	BYTE unk0x13[0x129];
	void* obj_ptr;			// + 0x13C
};

HWND* const hWnd = (HWND*)0x736380;
bool* const pIsLostFocus = (bool*)0x7363B6;
IDirect3DDevice9** const ppD3D9Device = (IDirect3DDevice9**)0x73636C;
D3DPRESENT_PARAMETERS* const pPresentationParameters = (D3DPRESENT_PARAMETERS*)0x71A96C;
void* const obj_list = (void*)0x779C70;
DWORD* const pPlayersByIndex = (DWORD*)0x7361BC;
DWORD* const pVirtualPhysicsFunc = (DWORD*)0x6B9670;
void* const call_World_DoTimestep = (void*)0x44796A; // after call opcode
void* const call_IDirect3DDevice9_EndScene = (void*)0x40A6CF;
void* const before_call_IDirect3DDevice9_Reset = (void*)0x40A52F;

enum StringCodes:uint32_t {
	StylePoints = 0x4D88440C,		// Style Points
	Steering_right = 0xDFE58E2F,	// Steering (right)
	Brake_Reverse = 0x87AF4E51,		// Brake/Reverse
	Nitrous_Boost = 0x6446DF0B,		// Nitrous Boost
};

typedef void(__stdcall* T_World_DoTimestep)(void* this_World, float stack_EAX);
const T_World_DoTimestep World_DoTimestep = (T_World_DoTimestep)0x420B40;
typedef char*(_fastcall *T_SearchForString)(DWORD ECX_crap, unsigned int code);
const T_SearchForString SearchForString = (T_SearchForString)0x59FB80;

typedef int(*T_FEngPrintf)(void* EAX_FEString, const char* format, ...);
const T_FEngPrintf FEngPrintf = (T_FEngPrintf)0x4F68A0;