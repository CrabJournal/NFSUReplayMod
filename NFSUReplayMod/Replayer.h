#pragma once

#include "HVector.h"
#include <windows.h>
#include "R_math.h"
#include "FromGame.h"
#include "d3d9Stuff.h"
#include "../NFSUReplayModeSettings/Structure.h"

extern Settings settings;

#define COMPRESS_MATRIX 
#define CALC_VELOCITY

#define total_car_num ((*pCurrentWorld)->CarsNumber)
#define racing_car_num (*NumberOfOpponents + 1)

void _fastcall Car_SetMovementMode_C(CarMovmentMode MovmentMode, Car* _this);

#define CarNum NosOn
#define CarMovmMode GearCode

const int msgbufsize = 256;
extern char msgbuf[msgbufsize];
#define clear_mgsbuf() (*msgbuf = '\0')

enum R_RaceType {
	QuickRace = 0,
	Underground
};

#pragma pack(push, 1)
struct ReplayMellowCarState {
	vector3 position;
	vector3 rotation;
};
struct ReplayPhysicsCarState : ReplayMellowCarState {
	unsigned NosOn : 1;
	unsigned GearCode : 7;

	BYTE Gas;
	char Steering;
	BYTE Break;
	BYTE E_Break;
};
#pragma pack(pop)

struct ReplayTraffcCarState {
	float pos[2];
	WORD angle;
};

struct GameState {
	float TimeElapsed;
	BYTE traffcCarsInFrame;
};


const float max_abs_playing_speed = 16;
const float min_abs_playing_speed = 1.0 / 8;
struct Frame {
	ReplayPhysicsCarState player_car;
	ReplayMellowCarState cars[15];
};
typedef double framecounttype;
#define game_speed playing_speed

class Replayer {
	framecounttype current_frame;
	float playing_speed;
	float average_frametime;
	HVector data;
	bool SaveCurrentReplay();
	bool LoadCurrentReplay();
	void PrepareToSave();

	static float ProcPlayingSpeed(float plsp);
	Frame* GenerateFrame(Frame*);
	void SetPlayingSpeed(float plsp);

	void Ret();

	void FirstFrameRec();
	void Rec();
	void PushMellow(int);

	void FirstFramePlay();
	void ResetReplayState();
	void Play();
	ReplayMellowCarState* GetPrevReplayCarState(ReplayMellowCarState*);
	void RestoreMellowCar(int, Frame*, Frame*);

	void (Replayer::*_Proc)();
	void (Replayer::*_Backup)();

public:
	void StartPlayNoReload();
	void Proc();
	void Init();
	void Release();
	void StartRec(R_RaceType mode, int ugracenumber);
	void SwitchRec();
	void StartPlay();
	void SwitchPlay();
	void Stop();
	void Save();

	void IncreasePlayingSpeed();
	void DecreasePlayingSpeed();

	void ResetCurrentMode();
	void PauseCurrentMode();
	void ContinueCurrentMode();

	void ChooseButton();

	Replayer();
	~Replayer();
};