#include "stdafx.h"
#include "Hooks.h"
#include "R_math.h"

#ifndef __countof
#define __countof(array) (sizeof(array) / sizeof(array[0]))
#endif // !__countof

extern "C" int __fltused;

Replayer replayer;

void _stdcall WorldDoTimestepHook(World* _World, float f);
void  RaceStartHook();
void _stdcall FromRCChangeState(RaceCoordinator* ESI_this, RCMessage mess, RCState state);
void BeforeFEUpdate();
void* _cdecl _nh_malloc_hook(size_t size, int flags);
void  __usercall InitFERaceEventHook();
//void  __stdcall SomeSteeringHook(float, float, DWORD);

AddrReplaceStruct ToReplace[] = {
	{call_World_DoTimestep, WorldDoTimestepHook, World_DoTimestep},
	{call_RaceStarter_StartRace, RaceStartHook, RaceStarter_StartRace},
	{call_RCSendMessage_InRCChangeState, FromRCChangeState, RaceCoordinator_RCSendMessage},
	{call_FEngUpdate, BeforeFEUpdate, FEngUpdate},
	{call_nh_malloc, _nh_malloc_hook, 0},
	{call_InitFERaceEvent, InitFERaceEventHook, FECarierManager_InitFERaceEvent},
	{call_InitFERaceEvent2, InitFERaceEventHook, FECarierManager_InitFERaceEvent},
	//{(void*)0x4616A1, SomeSteeringHook, 0}
};
const size_t countofToReplace = __countof(ToReplace);

T_World_DoTimestep* const B_World_DoTimestep = (T_World_DoTimestep*)&ToReplace[0].backup;
void_void* const B_RaceStarter_StartRace = (void_void*)&ToReplace[1].backup;
void** const B_RCChangeState = (void**)&ToReplace[2].backup;
void_void* const B_FEngUpdate = (void_void*)&ToReplace[3].backup;
T_nh_malloc* const B_nh_malloc = (T_nh_malloc*)&ToReplace[4].backup;
void** const B_InitFERaceEvent = (void**)&ToReplace[5].backup;
//void** const B_Steering = (void**)&ToReplace[7].backup;

//void __declspec(naked) __stdcall SomeSteeringHook(/*esi - this, */ float val, float time /*also eax*/, DWORD) {
//	__asm {
//		shl eax, 1
//		test eax, eax
//		je loc_ret
//		mov eax, B_Steering
//		jmp [eax]
//
//		loc_ret:
//		mov ax, [esi + 0x70]
//		ret 0xC
//	}
//}

void _stdcall WorldDoTimestepHook(World* _World, float time_elapsed) {
	if (GetAsyncKeyState(settings.buttons[Buttons::button_increasespeed]) & 1) {
		replayer.IncreasePlayingSpeed();
	}
	else if (GetAsyncKeyState(settings.buttons[Buttons::button_decreasespeed]) & 1) {
		replayer.DecreasePlayingSpeed();
	}
	if (time_elapsed == 0.0f)
		return;
	(*B_World_DoTimestep)(_World, time_elapsed);
	for (int i = racing_car_num; i < total_car_num; i++) {
		Car* tcar = (*pCurrentWorld)->Cars[i];
		//if (tcar->MovmentMode == CarMovmentMode::Traffic) {
			Car_SetMovementMode_C(CarMovmentMode::Mellow, tcar);
		//}
	}
	replayer.Proc();
}

void RaceStartHook() {
	replayer.StartRec(R_RaceType::QuickRace, 0);
	replayer.PauseCurrentMode();
	(*B_RaceStarter_StartRace)();
}

void _fastcall InitFERaceEventHookC(FECareerManager*, int);
void __declspec(naked) __usercall InitFERaceEventHook(/*esi - FECarierManager*, edi - CarierRaceNumber*/) {
	_asm {
		mov eax, B_InitFERaceEvent
		call [eax]
		mov ecx, esi
		mov edx, edi
		jmp InitFERaceEventHookC
	}
}
void _fastcall InitFERaceEventHookC(FECareerManager*, int CarierRaceNumber) {
	replayer.StartRec(R_RaceType::Underground, CarierRaceNumber);
	replayer.PauseCurrentMode();
}

void __stdcall FromRCChangeStateC(RCMessage mess, RCState state);
void __declspec(naked) __stdcall  FromRCChangeState(RaceCoordinator* ESI_this, RCMessage mess, RCState state) {
	__asm {
		mov eax, B_RCChangeState
		push dword ptr[esp + 8]
		push dword ptr[esp + 8]

		call [eax]
		jmp FromRCChangeStateC
	}
}
void __stdcall FromRCChangeStateC(RCMessage mess, RCState state) {
	if (state == RCState::RCS_End || state == RCState::RCS_Finish) {
		replayer.ResetCurrentMode(); 
		replayer.PauseCurrentMode();
	}
	else if (state == RCState::RCS_Race) {
		replayer.ContinueCurrentMode();
	}
}

void* __cdecl _nh_malloc_hook(size_t size, int flags) {
	void* ptr;
	while ((ptr = (*B_nh_malloc)(size, flags)) == 0) {}
	return ptr;
}

void BeforeFEUpdate() {
	if (GetAsyncKeyState(settings.buttons[Buttons::button_save])) {
		replayer.Save();
	}
	if (*_GameFlowState == GameFlowState::GAMEFLOW_STATE_IN_FRONTEND) {
		if (GetAsyncKeyState(settings.buttons[Buttons::button_choose]) & 1) {
			replayer.ChooseButton();
		}
		else if (GetAsyncKeyState(settings.buttons[Buttons::button_play]) & 1) {
			replayer.StartPlay();
			replayer.PauseCurrentMode();
		}
	}
	(*B_FEngUpdate)();
}

RECT stockrect = { 0, 0, 0, 0 };
HRESULT _stdcall HookEndScene(IDirect3DDevice9* d3ddevice) {
	D3DDrawText(msgbuf, &stockrect);
	return d3ddevice->EndScene();
}

HRESULT BeforeReset() {
	LPD3DXFONT old = dx_Font;
	D3DReleaseFont();
	HRESULT res = (*ppD3D9Device)->Reset(pPresentationParameters);
	if (old && res == S_OK) {
		D3DReinitFont(*ppD3D9Device);
	}
	return res;
}