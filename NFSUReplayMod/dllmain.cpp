#include "stdafx.h"
#include "Replayer.h"
#include <windows.h>

void WriteProtectedMem(void* dst, const void *src, size_t size) {
	DWORD oldp;
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldp);
	memcpy(dst, src, size);
}
void WriteProtectedMem(void* dst, const void *src, size_t size, void* save) {
	memcpy(save, dst, size);
	WriteProtectedMem(dst, src, size);
}
int GetRelativeAddress(int* addr1, void* addr2) {
	return (char*)addr2 - (char*)(addr1 + 1);
}
int WriteProtectedMemEIPRelativeAdderess(int* place, void* new_addr) {
	DWORD oldp;
	VirtualProtect(place, 4, PAGE_EXECUTE_READWRITE, &oldp);
	int old = *place;
	*place = GetRelativeAddress(place, new_addr);
	return old;
}

Replayer replayer;

int recordkey = 51; // 3
int playkey = 52; // 4
int choosekey = 53; // 5

//vector3 rot;
//float Matrix[3][4];

void _cdecl CHook(Object3D* obj) {
	if ((GetAsyncKeyState(recordkey) & 1)) {
		replayer.SwitchRec(obj);
		//AlignedRotationMatrixToRotation(obj->RotationMatrixAligned, &rot);
		//RotationToAlignedRotationMatrix(Matrix, &rot);
	}
	else {
		if ((GetAsyncKeyState(playkey) & 1)) {
			replayer.SwitchPlay(obj);
			// RotationToAlignedRotationMatrix(obj->RotationMatrixAligned, &rot);
		}
		else {
			if ((GetAsyncKeyState(choosekey) & 1)) {
				replayer.ChooseButton();
			}
		}
	}
	replayer.Proc(obj);
}

void __declspec(naked) HookObj()
{
	_asm
	{
		// check ret address
		// didn't found better way yet
		cmp dword ptr[esp + 0x2F0], 0x42036E
		jne skip
		push ecx
		call CHook
		pop ecx
		skip :
		push 0x585810
			ret
	}
}

T_World_DoTimestep World_DoTimestep_backup; // in case of someone already placed hook there (if it was call near hook)

void _stdcall FromMainLoop(void* _World, float f) {
	World_DoTimestep_backup(_World, f);
	node_obj* cur = *(node_obj**)(obj_list);
	do {
		if (cur->unk0x12 && *(int*)((*((BYTE**)cur - 1)) + 0x28) == 0x46A030) {
			Object3D* h = (Object3D*)(*(BYTE**)((*(BYTE**)((BYTE*)(cur->obj_ptr) + 0x34)) + 0x14) - 4);
			CHook(h);
			break;
		}
		cur = cur->next;
	} while (cur != (node_obj*)(obj_list));
}

RECT stockrect = { 0, 0, 0, 0 };
HRESULT _stdcall HookEndScene(IDirect3DDevice9* d3ddevice) {
	D3DDrawText(replayer.msg, &stockrect);
	return d3ddevice->EndScene();
}
HRESULT BeforeReset() {
	D3DReleaseFont();
	HRESULT res = (*ppD3D9Device)->Reset(pPresentationParameters);
	if (res == S_OK) {
		D3DInitFont(*ppD3D9Device);
	}
	return res;
}

void BeforeMainLoop() {}

DWORD backup;
// 0x4479A5 HUD render or something
// 0X447969 physics
void Init() {
	replayer.Init();
	// Set virtual function hook
	//void* ptr = HookObj;
	//WriteProtectedMem(VirtualFunc.ptr, &ptr, 4, &backup);
	World_DoTimestep_backup = (T_World_DoTimestep)
		((int)call_World_DoTimestep + 4 + WriteProtectedMemEIPRelativeAdderess((int*)(call_World_DoTimestep), FromMainLoop));

	//WriteProtectedMemEIPRelativeAdderess((int*)0x447647, BeforeMainLoop);
	//*(BYTE*)0x447646 = 0xE8; // call near
	//*(DWORD*)0x44764B = 0x90909090; // 4 NOPs

	WriteProtectedMemEIPRelativeAdderess((int*)((BYTE*)call_IDirect3DDevice9_EndScene + 1), HookEndScene);
	*(BYTE*)call_IDirect3DDevice9_EndScene = 0xE8; // call near
	*((BYTE*)call_IDirect3DDevice9_EndScene + 5) = 0x90; // NOP

	WriteProtectedMemEIPRelativeAdderess((int*)((BYTE*)before_call_IDirect3DDevice9_Reset + 1), BeforeReset);
	*(BYTE*)before_call_IDirect3DDevice9_Reset = 0xE8; // call near
	*(WORD*)((BYTE*)before_call_IDirect3DDevice9_Reset + 5) = 0x09EB; // jmp short +9
}

void Detach() {

	replayer.Destroy();
	// Restore virtual function pointer
	//WriteProtectedMem(VirtualFunc.ptr, &backup, 4);
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
			MessageBoxA(NULL, "This .exe is not supported.\nPlease use v1.4 English speed.exe (3,03 MB (3.178.496 bytes)).", "NFSU Extra Options", MB_ICONERROR);
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

