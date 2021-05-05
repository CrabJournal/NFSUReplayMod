#include "stdafx.h"
#include "d3d9Stuff.h"

LPD3DXFONT dx_Font = NULL;

void D3DDrawText(const char* str, LPRECT pos) {
	if (str[0] && dx_Font) {
		if (dx_Font->DrawTextA(NULL, str, -1, pos, DT_NOCLIP, D3DCOLOR_XRGB(200, 200, 255)) == 0) {
			__asm {
				int 3
			}
		}
	}
}

void D3DInitFont(IDirect3DDevice9 *pDevice) {
	if (dx_Font) {
		dx_Font->Release();
		dx_Font = NULL;
	}
	if (D3DXCreateFontA(pDevice, 30, 15, FW_BOLD, 0, 0, DEFAULT_CHARSET,
		OUT_TT_ONLY_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial", &dx_Font) != S_OK) {
		__asm {
			int 3
		}
	}
}

void D3DReleaseFont() {
	if (dx_Font) {
		dx_Font->Release();
		dx_Font = NULL;
	}
}