#pragma once

#include <vector>
#include <windows.h>
#include "R_math.h"
#include "FromGame.h"
#include "d3d9Stuff.h"

struct state {
	vector3 position;
	vector3 rotation;
	//float matrix[3][3];
};

class Replayer {

	const int MemAllocStepInStates = 3600; // 10 min
	int mem_allocated_in_states;
	int reader_current_pos;
	vector3 initial_pos;
	std::vector<state> states;
	bool SaveCurrentReplayAsync();
	bool LoadCurrentReplay();
	void Ret(Object3D* obj);
	void PushState(Object3D* obj);
	void Play(Object3D* obj);
	void (Replayer::*_Proc)(Object3D*);

public:
	const char *msg;
	void Proc(Object3D* obj);
	void Init();
	void Destroy();
	void StartRec(Object3D* obj);
	void SwitchRec(Object3D* obj);
	void StartPlay(Object3D* obj);
	void SwitchPlay(Object3D* obj);
	void Stop();
	void ChooseButton();

	Replayer();
	~Replayer();
};