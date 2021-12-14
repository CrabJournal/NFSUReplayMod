#include "windows.h"
#include "windowsx.h"
#include "commoncode.h"


const int left_padding = 20;
const int top_padding = 20;
const int field_height = 18;
const int field_name_width = 190;
const int fileld_param1_width = 80;
const int fileld_param2_width = field_name_width;
const int fields_height = fields_number * field_height;
const int buttom_margin = fields_height + top_padding;
const int right_marin = left_padding + field_name_width + fileld_param1_width + fileld_param2_width;

const int wndWidth = right_marin;
const int wndHeight = buttom_margin + 100;


const int invalid_field_value = -1;

int(_cdecl* __sprintf_s)(char* buf, size_t buf_count, const char* format, ...);
#define _sprintf_s __sprintf_s


int GetFieldPressed(int x, int y) {
	y -= top_padding;
	if (y < 0 || y > fields_height) {
		return invalid_field_value;
	}
	return y / field_height;
}

void SaveSettings() {
	HANDLE HFile = CreateFileA(settings_file_name, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD numbytes;
	WriteFile(HFile, &settings, sizeof(settings), &numbytes, NULL);
	CloseHandle(HFile);
}

size_t __strlen(const char* str) {
	const char* p = str;
	while (*p++);
	return p - str;
}

void DrawFields(HDC hdc) {
	int y = top_padding;
	for (int i = 0; i < Buttons::buttons_number; i++, y += field_height) {
		TextOutA(hdc, left_padding, y, ButtonNames[i], __strlen(ButtonNames[i]));
		static const int buf_size = 32;
		char buf[buf_size];
		UINT vsc = (MapVirtualKeyA(settings.buttons[i], MAPVK_VK_TO_VSC) << 16);
		GetKeyNameTextA(vsc, buf, buf_size);
		TextOutA(hdc, left_padding + field_name_width, y, buf, __strlen(buf));
		_sprintf_s(buf, buf_size, "%d", settings.buttons[i]);
		TextOutA(hdc, left_padding + field_name_width + fileld_param1_width, y, buf, __strlen(buf));
	}
	for (int i = 0; i < Flags::Flags_number; i++, y += field_height) {
		TextOutA(hdc, left_padding, y, FlagNames[i], __strlen(FlagNames[i]));
		settings.flags[i] &= TRUE;
		static const char* yes_no[] = { "No", "Yes" };
		const char *text = yes_no[settings.flags[i]];
		TextOutA(hdc, left_padding + field_name_width, y, text, __strlen(text));
	}
}
inline void DrawPressButton(HDC hdc) {
	static const char text[] = "Press the Button";
	TextOutA(hdc, 0, 0, text, sizeof(text));
}

int curr_button;

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK SecifyingButton(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT(CALLBACK* CurrWndProc)(HWND, UINT, WPARAM, LPARAM) = MainWndProc;

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		COLORREF color = RGB(100, 0, 0);
		DrawFields(hdc);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_LBUTTONDOWN: {
		int x = GET_X_LPARAM(lParam), y = GET_Y_LPARAM(lParam);
		int field = GetFieldPressed(x, y);
		if (field == invalid_field_value) {
			break;
		}
		if (field < Buttons::buttons_number) {
			curr_button = field;
			CurrWndProc = SecifyingButton;
		}
		else {
			field -= Buttons::buttons_number;
			settings.flags[field] ^= 1;
		}

		InvalidateRect(hWnd, 0, true);
		break;
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK SecifyingButton(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		DrawPressButton(hdc);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN: {
		settings.buttons[curr_button] = wParam;
		CurrWndProc = MainWndProc;
		InvalidateRect(hWnd, 0, true);
		break;
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	if (message == WM_DESTROY) {
		SaveSettings();
		ExitProcess(0);
	}
	return CurrWndProc(hWnd, message, wParam, lParam);
}

const char clName[] = "Settings";
#define Title clName

ATOM MyRegisterClass(HINSTANCE hInstance) {
	WNDCLASSA wcex;
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = 0;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)6;
	wcex.lpszMenuName = 0;
	wcex.lpszClassName = clName;
	return RegisterClassA(&wcex);
}

void InitInstance(HINSTANCE hInstance) {
	HWND hWnd = CreateWindowA(clName, Title, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, wndWidth,  wndHeight, nullptr, nullptr, hInstance, nullptr);
	ShowWindow(hWnd, SW_SHOWNORMAL);
	UpdateWindow(hWnd);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
	__sprintf_s = (int(_cdecl*)(char*, size_t, const char*, ...))GetProcAddress(hNtdll, "sprintf_s");
	LoadSettings();
	MyRegisterClass(hInstance);
	InitInstance(hInstance);
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}