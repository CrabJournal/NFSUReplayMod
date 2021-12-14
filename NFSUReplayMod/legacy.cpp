if ((GetAsyncKeyState(54) & 1)) {
	Car** cars = (*pCurrentWorld)->Cars;
	vector3* pl_pos = &(cars[0]->worldobject->object->position);
	float offs1 = 0, offs2 = 0;
	for (int i = 1; i <= 12; i++) {
		vector3* bot_pos;
		const float diff = 4;
		__try {
			float offs;
			if (cars[i]->worldobject->Vtable == (void*)0x6BA714) {
				bot_pos = ((vector3*)(cars[i]->worldobject) + 1);
				offs1 += diff;
				offs = offs1;
			}
			else {
				bot_pos = &(cars[i]->worldobject->object->position);
				offs2 -= diff;
				offs = offs2;
			}
			*bot_pos = *pl_pos;
			bot_pos->y += offs;
		}
		__except (Handler(GetExceptionCode(), GetExceptionInformation())) {}
	}
}

void GetNameForReplay(char* buf) {
	WIN32_FIND_DATAA ffd;
	char tbuf[256];
	tbuf[0] = '\0';
	strcpy(buf, filesdir "/");
	HANDLE hFind = FindFirstFileA(filesdir, &ffd);
	if (hFind == INVALID_HANDLE_VALUE) {
		strcat(buf, "0.rpl");
	}
	else {
		bool fnd = false;
		do {
			if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				strcpy(tbuf, ffd.cFileName);
			}
		} while (FindNextFileA(hFind, &ffd) != 0);
		FindClose(hFind);
		hFind = NULL;
		if (tbuf[0] == '\0') {
			strcat(buf, "0.rpl");
		}
		else {
			tbuf[0]++;
			strcat(buf, tbuf);
		}
	}
}

void Replayer::SwitchRec() {
	if (_Proc == &Replayer::Rec) {
		Stop();
		SaveCurrentReplayAsync();
	}
	else {
		if (_Proc != &Replayer::Ret) {
			Stop();
		}
		StartRec();
	}
}
void Replayer::SwitchPlay() {
	if (_Proc == &Replayer::Play) {
		Stop();
	}
	else {
		if (_Proc != &Replayer::Ret) {
			Stop();
			SaveCurrentReplayAsync();
			StartPlayNoReload();
			return;
		}
		StartPlay();
	}
}

#define CopySameNameFields(a, b, field) ((a).field = (b).field)
#define CopySameNameFields2(a, b, field) CopySameNameFields(*a, *b, field)
void FillRaceHeader(RaceHeader *raceHeader) {
	RaceOptions* raceOptions = cFrontendDatabase_GetOptions(FEDatabase, *CurrentRaceMode);
	raceHeader->raceMode = *CurrentRaceMode;
	CopySameNameFields2(raceHeader, raceOptions, SkipFETrackNumber);
	CopySameNameFields2(raceHeader, raceOptions, isBackwardRaceTrack);
	CopySameNameFields2(raceHeader, raceOptions, lapsNumber);
	CopySameNameFields2(raceHeader, raceOptions, oponentsSkill);
	CopySameNameFields2(raceHeader, raceOptions, Traffic);
	CopySameNameFields2(raceHeader, raceOptions, chatchUp);
}
void FillRaceOptions(const RaceHeader *raceHeader) {
	RaceOptions* raceOptions = cFrontendDatabase_GetOptions(FEDatabase, *CurrentRaceMode);
	*CurrentRaceMode = raceHeader->raceMode;
	CopySameNameFields2(raceOptions, raceHeader, SkipFETrackNumber);
	CopySameNameFields2(raceOptions, raceHeader, isBackwardRaceTrack);
	CopySameNameFields2(raceOptions, raceHeader, lapsNumber);
	CopySameNameFields2(raceOptions, raceHeader, oponentsSkill);
	CopySameNameFields2(raceOptions, raceHeader, Traffic);
	CopySameNameFields2(raceOptions, raceHeader, chatchUp);
}

void Matrix4x4To3x3(float Matrix4x4[4][4], float Matrix3x3[3][3]) {
	memcpy(&(Matrix3x3[0][0]), &(Matrix4x4[0][0]), 12);
	memcpy(&(Matrix3x3[1][0]), &(Matrix4x4[1][0]), 12);
	memcpy(&(Matrix3x3[2][0]), &(Matrix4x4[2][0]), 12);
}
void Matrix3x3To4x4(float Matrix4x4[4][4], float Matrix3x3[3][3]) {
	memcpy(&(Matrix4x4[0][0]), &(Matrix3x3[0][0]), 12);
	memcpy(&(Matrix4x4[1][0]), &(Matrix3x3[1][0]), 12);
	memcpy(&(Matrix4x4[2][0]), &(Matrix3x3[2][0]), 12);
}
if (GetPlayerCarBody() == nullptr) {
	return;
}

/*const int n = 5;
int nums[n] = { 0 };
for (int i = 1; i < *NumberOfCars; i++) {
	Car *pCar = (*pCurrentWorld)->Cars[i];
	CarMovmentMode cmm = pCar->MovmentMode;
	nums[cmm]++;
	Car_SetMovementMode_C(CarMovmentMode::Null, pCar);
	if (cmm == CarMovmentMode::Traffic) {
		TrafficMover* mover = (TrafficMover*)((*pCurrentWorld)->Cars[i]->mover);
		mover->speed = 0;
	}
}
for (int i = 0, offset = 0; i < n; i++) {
	static const char* const typeNames[n] = { "Null", "Physics", "Traffic", "Mellow", "Belt" };
	sprintf_s(replayer.msg + offset, 260 - offset, "%s=%d\n", typeNames[i], nums[i]);
	offset = strlen(replayer.msg);
}
if (GetAsyncKeyState(51) & 1) {
	vector3 poss[n];
	const float step = 6;
	static vector3 toAdd[n] = { {step, 0, step}, {-step, 0, 0}, {0, step, step}, {0, -step, 0}, {step, step, 0} };
	Car* plCar = (*pCurrentWorld)->Cars[0];
	RigidBody *plBody = plCar->mover->body;
	poss[0] = plBody->position;
	float (*plcarmatrix)[4] = plBody->RotationMatrix4x4;
	float plangle = (float)(plCar->state.angle1_R) / 10430.378;
	for (int i = 1; i < n; i++) {
		poss[i] = poss[0];
	}
	for (int i = 1; i < *NumberOfCars; i++) {
		Car *pCar = (*pCurrentWorld)->Cars[i];
		CarMovmentMode cmm = pCar->MovmentMode;
		vector3* Carpos;
		float *carmatrix;
		//if (cmm != CarMovmentMode::Traffic) { continue; }
		//Car_SetMovementMode_C(CarMovmentMode::Null, pCar);
		SaveM128InV3(poss[cmm] + toAdd[cmm], poss[cmm]);
		////if (cmm == CarMovmentMode::Traffic) {
		//	TrafficMover* mover = (TrafficMover*)(pCar->mover);
		//
		//	mover->vector2_Pos[0] = poss[cmm].x;
		//	mover->vector2_Pos[1] = poss[cmm].y;
		//	//Carpos = &mover->vector2_Pos;
		//	mover->angle = plangle;
		//}
		//else {
			Carpos = &(pCar->state.somePos);
			RigidBody* body = pCar->mover->GetBody();
			if (body != 0) {
				body->velocity = zero_m128();
			}
			//carmatrix = pCar->collision->GetRotationMatrix4x4();
			//memcpy(carmatrix, plcarmatrix, sizeof(float) * 4 * 4);
			*Carpos = poss[cmm];
		//}
	}
}*/
void FillGameTrafficCarState(Car* game_car, const ReplayPhysicsCarState* repl_car) {
	memcpy(((TrafficMover*)(game_car->mover))->vector2_Pos, &repl_car->position, 12);
	game_car->state.angle1_R = repl_car->position.z * AngleConvert;
	_asm {
		mov eax, [game_car]
		lea ecx, [eax + 540h]
		push ecx
		lea ecx, [eax + 60h]
		push ecx
		lea ecx, [eax + 90h]
		push ecx
		push eax
		movzx eax, [eax + 3B4h]
		mov ecx, Car_UpdateState
		call ecx
	}
}
int tr_cars;
for (tr_cars = 0; i < total_car_num; i++) {
	Car* tcar = GetGameCar(i);
	state = &Carstates[current_state];
	if (state->CarNum == i) {
		CarMovmentMode rcmm = (CarMovmentMode)state->CarMovmMode;
		Car_SetMovementMode_C(rcmm, tcar);
		if (rcmm == CarMovmentMode::Mellow) {
			RestoreMellowCar(i);
		}
		else {
			FillGameTrafficCarState(tcar, state);
			current_state++;
		}
		tr_cars++;
	}
	else {
		Car_SetMovementMode_C(CarMovmentMode::Null, tcar);
	}
	else {
	Car_SetMovementMode_C(CarMovmentMode::Null, tcar);
	}
}
