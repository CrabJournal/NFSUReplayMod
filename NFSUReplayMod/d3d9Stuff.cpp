#include "stdafx.h"
#include "d3d9Stuff.h"
#include "FromGame.h"

LPD3DXFONT dx_Font = NULL;

HRESULT (WINAPI* __D3DXCreateFontA)(
	LPDIRECT3DDEVICE9       pDevice,
	INT                     Height,
	UINT                    Width,
	UINT                    Weight,
	UINT                    MipLevels,
	BOOL                    Italic,
	DWORD                   CharSet,
	DWORD                   OutputPrecision,
	DWORD                   Quality,
	DWORD                   PitchAndFamily,
	LPCSTR                  pFaceName,
	LPD3DXFONT*             ppFont);

HRESULT WINAPI __D3DXCreateFontA_bypass(
	LPDIRECT3DDEVICE9       pDevice,
	INT                     Height,
	UINT                    Width,
	UINT                    Weight,
	UINT                    MipLevels,
	BOOL                    Italic,
	DWORD                   CharSet,
	DWORD                   OutputPrecision,
	DWORD                   Quality,
	DWORD                   PitchAndFamily,
	LPCSTR                  pFaceName,
	LPD3DXFONT*             ppFont) {
	ppFont = NULL;
	return 1;
}

int(_cdecl* __vsprintf_s)(__int64 options, char* buf, size_t buf_size, const char* format, _locale_t locale, va_list arglist);
int _cdecl __vsprintf_s_bypass(__int64, char* buf, size_t buf_size, const char* format, _locale_t, va_list va) {
	char *tbuf = (char*)NFS_malloc(buf_size); // do not overflows stack or merged code and data segment at least
	int _Result = NFS_vsprintf(tbuf, format, va);
	tbuf[buf_size - 1] = 0;
	NFS_strcpy(buf, tbuf);
	NFS_free(tbuf);
	return _Result;
}
int __sprintf_s(char* buf, size_t buf_size, const char* format, ...) {
	int _Result;
	va_list _ArgList;
	va_start(_ArgList, format);
	_Result = __vsprintf_s(4, buf, buf_size, format, NULL, _ArgList);
	va_end(_ArgList);
	return _Result;
}

extern "C" int __fltused;

void D3DDrawText(const char* str, LPRECT pos) {
	if (str[0] && dx_Font) {
		if (dx_Font->DrawTextA(NULL, str, -1, pos, DT_NOCLIP, D3DCOLOR_XRGB(200, 200, 255)) == 0) {
			// error
		}
	}
}

void D3DReleaseFont() {
	if (dx_Font) {
		dx_Font->Release();
		dx_Font = NULL;
	}
}

void D3DInitFont(IDirect3DDevice9 *pDevice) {
	if (dx_Font == 0)
		if (__D3DXCreateFontA(pDevice, 30, 15, FW_BOLD, 0, 0, DEFAULT_CHARSET,
			OUT_TT_ONLY_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial", &dx_Font) != S_OK) {
			// __asm int 3
			// error
			char buf_t[256];
			_sprintf_s(buf_t, 256, "D3DXCreateFontA fails, pDevice=%p", pDevice);
			MessageBoxA(0, buf_t, "NFSUReplayMod", MB_OK);
			dx_Font = NULL;
		}
}

void D3DReinitFont(IDirect3DDevice9 *pDevice) {
	D3DReleaseFont();
	D3DInitFont(pDevice);
}

HMODULE hCRTdll;
HMODULE hd3d;
void D3DXInit() {
	hCRTdll = LoadLibraryA("api-ms-win-crt-stdio-l1-1-0.dll");
	__vsprintf_s = (int(_cdecl*)(__int64, char*, size_t, const char*, _locale_t, va_list))GetProcAddress(hCRTdll, "__stdio_common_vsprintf_s");
	if (__vsprintf_s == NULL) {
		//MessageBoxA(NULL, "Couldn't load vsprintf_s", "NFSU Replay Mod", MB_OK);
		__vsprintf_s = __vsprintf_s_bypass;
	}

	for (int i = 43; i >= 24; i--) {
		const int buf_size = 16;
		char buf[buf_size];
		_sprintf_s(buf, buf_size, "d3dx9_%d.dll", i);
		hd3d = LoadLibraryA(buf);
		if (hd3d != NULL) {
			__D3DXCreateFontA = (HRESULT(WINAPI*)(LPDIRECT3DDEVICE9,INT,UINT,UINT,UINT,BOOL,DWORD,DWORD,DWORD,DWORD,LPCSTR,LPD3DXFONT*))
				GetProcAddress(hd3d, "D3DXCreateFontA");
			if (__D3DXCreateFontA != NULL) {
				return;
			}
		}
	}

	__D3DXCreateFontA = __D3DXCreateFontA_bypass;
}

void D3DXDeInit() {
	FreeLibrary(hCRTdll);
	FreeLibrary(hd3d);
}