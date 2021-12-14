#pragma once

enum Buttons {
	button_play = 0,
	button_save,
	button_choose,
	button_increasespeed,
	button_decreasespeed,

	buttons_number
};
enum Flags {
	Flag_ShowInfoInRace,
	Flag_ShowInfoOnReplaying,

	Flags_number
};

#pragma pack(push, 1)
struct Settings {
	int buttons[Buttons::buttons_number];
	char flags[Flags::Flags_number];
};
#pragma pack(pop)

const int fields_number = Buttons::buttons_number + Flags::Flags_number;