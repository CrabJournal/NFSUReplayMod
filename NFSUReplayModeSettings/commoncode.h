#pragma once
#include "Structure.h"
#include "windows.h"

Settings settings = { 51, 52, 53, 55, 54, FALSE, TRUE };

const char settings_file_name[] = "NFSURMSettings.bin";

void LoadSettings() {
	HANDLE hFile = CreateFileA(settings_file_name, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		return;
	}
	DWORD numbytes;
	ReadFile(hFile, &settings, sizeof(settings), &numbytes, NULL);
	CloseHandle(hFile);
}

const char* ButtonNames[] = { "Play", "Save", "Choose", "Increase playing speed", "Decrease playing speed" };
const char* FlagNames[] = { "Show info in race",  "Show info when replaying" };