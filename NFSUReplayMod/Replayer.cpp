#include "stdafx.h"
#include "Replayer.h"

inline const char* R_strend(const char *str) {
	while (*str) { str++; }
	return str;
}
inline size_t R_strlen(const char *str) {
	return R_strend(str) - str;
}
// unlike std C function, returns lenght of resulted string including \0 (because dst_size may be <= 0)
size_t R_strcpy_s(char* dst, const char* src, int dst_size) {
	char t;
	int i = 0;
	if (dst_size <= i) { return i; }
	do {
		t = src[i];
		dst[i] = t;
		i++;
	} while (t && i < dst_size - 1);
	dst[i - 1] = 0;
	return i;
}
// unlike std C function, returns lenght of resulted string including \0
// nothing will be copied if str1_size <= strlen(str1) for some reason
size_t R_strcat_s(char* str1, const char* str2, size_t str1_size) {
	char* str1end = (char*)R_strend(str1);
	int str1len = str1end - str1;
	return str1len + R_strcpy_s(str1end, str2, str1_size - str1len);
}

constexpr int MinOfConsts(int a, int b) {
	return (a < b)? a : b;
}
#define _strcpy(DST, SRC) (R_strcpy_s(DST, SRC, _countof(DST)))
#define _strcat(DST, SRC) (R_strcat_s(DST, SRC, _countof(DST)))

// CRT bypass
int atexit(void(__cdecl *func)(void)) { return 0; }
extern "C" int __fltused;

// ============== game relative code ==================================

inline Car* GetGameCar(int index) {
	return (*pCurrentWorld)->Cars[index];
}

inline Car* GetPlayerCar() {
	return GetGameCar(0);
}

inline RigidBody* GetPlayerCarBody() {
	return GetPlayerCar()->mover->GetBody();
}

void FillMellowGameCar(Car* game_car, const ReplayMellowCarState* repl_car) {
	RigidBody* body = game_car->mover->body;
	RotationToRotationMatrix(body->RotationMatrix4x4, &(repl_car->rotation));
	__m128 pos = repl_car->position;
	body->position = pos;
	game_car->state.somePos2 = pos;
	//NFS_memcpy(&game_car->state.somePos2, &repl_car->position, 12);
	R_memcpy(game_car->state.Matrix4x4_0x50, body->RotationMatrix4x4, sizeof(float) * 3 * 4);
}
void SetNosState(Car* game_car, bool state) {
	if (state) {
		if (game_car->state.NosTimer == 0) {
			game_car->state.Nos1 = game_car->state.Nos2;
			game_car->state.NosTimer = *WorldTimer;
		}
	}
	else {
		if (game_car->state.NosTimer != 0) {
			CarState *state = &game_car->state;
			__asm {
				mov esi, state
				call CarState_CalcNos
			}
		}
	}
}
//const WORD steercoef = 180;
void FillPhysicsGameCar(Car* game_car, const ReplayPhysicsCarState* repl_car) {
	FillMellowGameCar(game_car, repl_car);
	//R_memset(&game_car->state.mbAceleration, 0, 12);
	//R_memset(&game_car->mover->body->mbAceleration1, 0, 28);

	RealDriver* drv = game_car->Driver;
	if (game_car->DriverType == 2) {
		drv = (RealDriver*)(((BYTE*)drv) + 0x250);
	}
	if (drv->Vtable == (void*)0x6C8AD0) {
		static const float div255 = 1.0f / 255.0f; // loosing a bit precision
		static const float div127 = 1.0f / 127.0f;
		drv->GearCode_RW = repl_car->GearCode;
		drv->Gas_RW = ((float)(repl_car->Gas)) * div255;
		drv->Steering_RW = ((float)(repl_car->Steering)) * div127;
		//drv->actual_steering = (WORD)repl_car->Steering * steercoef;
		drv->Break_RW = ((float)(repl_car->Break)) * div255;
		drv->E_Break_RW = ((float)(repl_car->E_Break)) * div255;
	}
	SetNosState(game_car, repl_car->NosOn);
}
void FillReplayCarMellow(const Car* game_car, ReplayMellowCarState* repl_car) {
	register const RigidBody* body = game_car->mover->body;
	repl_car->position = body->position;
	RotationMatrixToRotation(body->RotationMatrix4x4, &repl_car->rotation);
}
void FillReplayCarPhysics(const Car* game_car, ReplayPhysicsCarState* repl_car) {
	FillReplayCarMellow(game_car, repl_car);
	register const RealDriver* drv = game_car->Driver;
	if (game_car->DriverType == 2) {
		drv = (RealDriver*)(((BYTE*)drv) + 0x250);
	}
	repl_car->NosOn = (DWORD)(game_car->state.NosTimer);
	repl_car->GearCode = drv->GearCode_RW;
	repl_car->Gas = drv->Gas_R;
	repl_car->Steering = (drv->Steering_RW) * 127.0f;
	//WORD t = drv->actual_steering / steercoef;
	//if (t > 180) { t = 180; }
	//repl_car->Steering = t;
	
	repl_car->Break = drv->Break_R;
	repl_car->E_Break = drv->E_Brake_R;
}

void GameCarCalcVelocity(Car* game_car, ReplayMellowCarState& curr, ReplayMellowCarState& prev, float repTimeElapsed) {
	__m128 velocity = (curr.position - prev.position) / repTimeElapsed;
	RigidBody *co = game_car->mover->body;
	vector3a* v = &(co->velocity);
	/*co->magic3 = 1;
	co->magic3_inversed = 1;*/
	*v = velocity * co->magic3;
	game_car->state.speed_mps_R = abs(v);
	//v->align = 0;

	/*vector3a v2 = velocity;
	float scalar_velocity = sqrt(v2.x * v2.x + v2.y * v2.y + v2.z * v2.z);
	game_car->state.speed_mps_R = scalar_velocity;*/
	
	//co->velocity_multiplied.align = 0;
}

// =====================================================================
typedef DWORD SIGN_TYPE;
enum Version : WORD {
	v0_9,

	LastVersionP1
};
// NURP
const SIGN_TYPE file_signature = 0xF156'E40A;
const Version CurrentVersion = (Version)(LastVersionP1 - 1);
struct FileHeader {
	SIGN_TYPE sign;
	Version version;
	Version minVersionRequired;
};
const FileHeader currentVersionFileHeader = { file_signature, CurrentVersion, v0_9 };
struct ReplayHeader {
	FileHeader fileheader;

	DWORD statestotalsize;
	DWORD statesNum;
	DWORD frames;
	DWORD seed;
	R_RaceType racetype;
	float averageframetime;
	union Race {
		struct UndergroundRace {
			DWORD difficulty;
			DWORD UgRaceNumber;
		}ug;
		struct QuickRace {
			DWORD raceMode;
			RaceOptions options;
		}QR;
	}race;
};
ReplayHeader _replayHeader;

void ProcRaceHeader(ReplayHeader* raceHeader) {
	*Seed = raceHeader->seed;
	switch (raceHeader->racetype) {
	case QuickRace: {
		*CurrentRaceMode = raceHeader->race.QR.raceMode;
		RaceOptions* ro = cFrontendDatabase_GetOptions(FEDatabase, raceHeader->race.QR.raceMode);
		R_memcpy(ro, &raceHeader->race.QR.options, sizeof(RaceOptions));
		RaceStarter_StartRace();
		break;
	}
	case Underground: {
		*UndergroundRaceDifficulty = raceHeader->race.ug.difficulty;
		int raceNum = raceHeader->race.ug.UgRaceNumber;
		_asm {
			mov esi, CareerManager
			mov edi, raceNum
			mov eax, FECarierManager_InitFERaceEvent
			call eax
			mov ebx, [pFERaceEvent]
			mov ebx, [ebx]
			mov eax, RaceStarter_StartUndergroundModeRaceR
			call eax
		}
		break;
	}
	default:
		break;
	}
}

// ============== WinAPI files stuff ===================================

#define filesdir "replays"

char msgbuf[msgbufsize] = "";

char currentrplfilename[_MAX_PATH] = "";

HANDLE HFind = NULL;
WIN32_FIND_DATAA FFD;

void GetNewReplayNamePath(char buf[MAX_PATH]) {
	CreateDirectoryA(filesdir, NULL);
	SYSTEMTIME time;
	GetLocalTime(&time);
	_sprintf_s(buf, MAX_PATH, filesdir "/%d.%02d.%02d %d.%d.%d.nurp", time.wHour, time.wMinute, time.wSecond, time.wDay, time.wMonth, time.wYear);
}
bool GetNextFILE() {
	while (FindNextFileA(HFind, &FFD) != 0) {
		if (!(FFD.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			return true;
		}
	}
	return false;
}
void Replayer::ChooseButton() {
	D3DReinitFont(*ppD3D9Device);
	char filenamebuf[MAX_PATH];
	if (HFind != NULL) {
		if (GetNextFILE()) {
			_strcpy(filenamebuf, FFD.cFileName);
		}
		else {
			FindClose(HFind);
			HFind = NULL;
			goto l_else;
		}
	}
	else {
	l_else:
		HFind = FindFirstFileA(filesdir "/*", &FFD);
		if (HFind == INVALID_HANDLE_VALUE) {
			HFind = NULL;
			CreateDirectoryA(filesdir, NULL);
			return;
		}
		if (FFD.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			if (!GetNextFILE()) {
				_strcpy(msgbuf, "There's no replays");
				currentrplfilename[0] = '\0';
				FindClose(HFind);
				HFind = NULL;
				return;
			}
		}
		_strcpy(filenamebuf, FFD.cFileName);
	}
	char keynamebuf[32];
	GetKeyNameTextA(MapVirtualKeyA(settings.buttons[Buttons::button_play], MAPVK_VK_TO_VSC) << 16, keynamebuf, _countof(keynamebuf));
	_sprintf_s(msgbuf, msgbufsize, "Press %s to play '%s'", keynamebuf, filenamebuf);
	_strcpy(currentrplfilename, filesdir "/");
	_strcat(currentrplfilename, filenamebuf);
}

//#define ASYNC

#ifdef ASYNC
const DWORD FlagAtributes = FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED;
OVERLAPPED ovlp, ovlp2;
HANDLE HFile;
void* tosave;
void CALLBACK Saved(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped) { MessageBoxA(0, "Saved!", "mess", 0); }
#else
const DWORD FlagAtributes = FILE_ATTRIBUTE_NORMAL;
#endif

inline bool R_WriteFile(HANDLE hFile, void* addr, size_t size) {
	DWORD nb;
#ifdef ASYNC
	return WriteFileEx(hFile, addr, size, &ovlp, Saved);
#else
	return WriteFile(hFile, addr, size, &nb, 0);
#endif
}

bool Replayer::SaveCurrentReplay() {
	if (_replayHeader.frames == 0) {
		return false;
	}
	_replayHeader.fileheader = currentVersionFileHeader;
	_replayHeader.averageframetime = average_frametime;
	GetNewReplayNamePath(currentrplfilename);
	HANDLE HFile = CreateFileA(currentrplfilename, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS,
		FlagAtributes, NULL);
	if (HFile == INVALID_HANDLE_VALUE) {
		return false;
	}
	if (R_WriteFile(HFile, &_replayHeader, sizeof(_replayHeader)) == false) {
		CloseHandle(HFile);
		return false;
	}
	_sprintf_s(msgbuf, msgbufsize, "Saved as '%s'", currentrplfilename);
	// ovlp2.Offset = sizeof(_raceHeader);
	bool t = R_WriteFile(HFile, data.GetMemPtr(0), _replayHeader.statestotalsize);
	CloseHandle(HFile);
	_replayHeader.frames = 0;
	return t;
}
bool Replayer::LoadCurrentReplay() {
	if (currentrplfilename[0] == '\0') {
		return false;
	}
	HANDLE hFile = CreateFileA(currentrplfilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		return false;
	}
	DWORD bytesreaded;
	if (ReadFile(hFile, &_replayHeader, sizeof(_replayHeader), &bytesreaded, NULL) == false) {
		CloseHandle(hFile);
		return false;
	}
	if (_replayHeader.fileheader.sign != file_signature) {
		_strcpy(msgbuf, "It's not a replay");
		CloseHandle(hFile);
		return false;
	}
	if (_replayHeader.fileheader.minVersionRequired > CurrentVersion) {
		_strcpy(msgbuf, "Unsupported replay version");
		CloseHandle(hFile);
		return false;
	}
	data.resize(_replayHeader.statestotalsize);
	BOOL t = ReadFile(hFile, data.GetMemPtr(0), _replayHeader.statestotalsize, &bytesreaded, NULL);
	CloseHandle(hFile);
	return t;
}

// ====================================================================

void Replayer::Ret() {};

void Replayer::FirstFrameRec() {
	_Proc = &Replayer::Rec;
	Rec();
}
void Replayer::FirstFramePlay() {
	_Proc = &Replayer::Play;
	Play();
}

void Replayer::Rec(){
	Car* plcar = GetPlayerCar();
	if (plcar == 0 || plcar->MovmentMode != CarMovmentMode::Physics) {
		return;
	}
	RigidBody* plbody = plcar->mover->body;

	if (settings.flags[Flags::Flag_ShowInfoInRace]) {
		_sprintf_s(msgbuf, msgbufsize, 
			"Recording...  Frame %d, vector mem: %.2fKB, XYZ " "%.3f %.3f %.3f",
			R_dtoi(current_frame),
			(double)data.capacity() / 1024.0, 
			plbody->position.x, plbody->position.y, plbody->position.z);
			/*plbody->RotationMatrix4x4[0][0], plbody->RotationMatrix4x4[0][1], plbody->RotationMatrix4x4[0][1],
			plbody->RotationMatrix4x4[1][0], plbody->RotationMatrix4x4[1][1], plbody->RotationMatrix4x4[1][1], 
			plbody->RotationMatrix4x4[2][0], plbody->RotationMatrix4x4[2][1], plbody->RotationMatrix4x4[2][1]);*/
	}
	ReplayPhysicsCarState* RCarState = (ReplayPhysicsCarState*)data.ReservePlace(sizeof(ReplayPhysicsCarState));
	FillReplayCarPhysics(GetPlayerCar(), RCarState);
	int i;
	for (i = 1; i < racing_car_num; i++) {
		PushMellow(i);
	}
	for ( ; i < total_car_num; i++) {
		PushMellow(i);
	}
	current_frame++;
	average_frametime = average_frametime - (average_frametime - *RealTimeElapsed) / current_frame;
}

void Replayer::PushMellow(int CarIndex) {
	ReplayMellowCarState* state = (ReplayMellowCarState*)data.ReservePlace(sizeof(ReplayMellowCarState));
	FillReplayCarMellow(GetGameCar(CarIndex), state);
}

ReplayMellowCarState* Replayer::GetPrevReplayCarState(ReplayMellowCarState* state) {
	return (ReplayMellowCarState*)( (BYTE*)state - sizeof(ReplayPhysicsCarState) - (sizeof(ReplayMellowCarState) * (total_car_num - 1)) );
}
#define frametime  _replayHeader.averageframetime
void Replayer::RestoreMellowCar(int CarIndex, Frame* frame, Frame* i_frame) {
	Car* car = GetGameCar(CarIndex);
	FillMellowGameCar(car, &frame->cars[CarIndex - 1]);
	if (current_frame >= 1.0) {
		GameCarCalcVelocity(car, i_frame->cars[CarIndex - 1], *(GetPrevReplayCarState(&i_frame->cars[CarIndex - 1])), frametime);
	}
}
void _declspec(naked) _fastcall Car_SetMovementMode_C(CarMovmentMode MovmentMode, Car* _this) {
	_asm {
		push esi
		mov esi, edx
		mov eax, Car_SetMovementMode
		call eax
		pop esi
		ret
	}
}
#define GetSizeofFrame() (sizeof(ReplayPhysicsCarState) + sizeof(ReplayMellowCarState)* (total_car_num - 1))

inline __m128 _vectorcall Interpolate(__m128 v1, __m128 v2, __m128 t) {
	return v1 + ((v2 - v1) * t);
}
void MellowCarInterpolate(ReplayMellowCarState &s1, ReplayMellowCarState &s2, ReplayMellowCarState &dst, 
	float subframe, __m128 subframe128) {
	SaveM128InV3(Interpolate(s1.position, s2.position, subframe128), dst.position);
	//dst.position = s1.position;

	// Linear interpolation isn't correct for sin(x), but probably it have enough approximation if dx is very small
	__m128 coss1 = SetCosSignFromSrcSSE(s1.rotation, GetSinCosSSE(s1.rotation));
	__m128 coss2 = SetCosSignFromSrcSSE(s2.rotation, GetSinCosSSE(s2.rotation));

	__m128 signs = (Interpolate(coss1, coss2, subframe128) >> 31);
	SaveM128InV3(((Interpolate(s1.rotation, s2.rotation, subframe128) >> 1) << 1 ) | signs, dst.rotation);

	// dst.rotation = s1.rotation;
}
Frame* Replayer::GenerateFrame(Frame* in) {
	size_t reader_offset = data.GetReaderOffset();
	Frame* pCurrentFrameVal = (Frame*)data.GetMemPtr(reader_offset);
	int i_current_frame = current_frame;
	framecounttype fi_current_frame = i_current_frame;
	if (current_frame == fi_current_frame) {
		return pCurrentFrameVal;
	}
	Frame* pNextFrameVal = (Frame*)data.GetMemPtr(reader_offset + GetSizeofFrame());
	R_memcpy(&in->player_car, &pCurrentFrameVal->player_car, sizeof(ReplayPhysicsCarState));
	float subframe = current_frame - fi_current_frame;
	__m128 subframe128 = _mm_load_ps1(&subframe);
	MellowCarInterpolate(pCurrentFrameVal->player_car, pNextFrameVal->player_car, in->player_car, subframe, subframe128);
	for (int i = 0; i < total_car_num - 1; i++) {
		MellowCarInterpolate(pCurrentFrameVal->cars[i], pNextFrameVal->cars[i], in->cars[i], subframe, subframe128);
	}
	return in;
}
void Replayer::Play() {

	Car* plcar = GetPlayerCar();
	if (plcar == 0 || plcar->MovmentMode != CarMovmentMode::Physics) {
		return;
	}
	if (current_frame > (framecounttype)(_replayHeader.frames - 1)) {
		Stop();
		_strcpy(msgbuf, "Replay finished");
		// free controls
		RealDriver* drv = plcar->Driver;
		drv->Break_RW = 0;
		drv->E_Break_RW = 0;
		drv->Gas_RW = 0;
		drv->Steering_RW = 0;
		SetNosState(plcar, 0);
		return;
	}
	// or check 0x426998
	*(DWORD*)0x73B234 = 0x3F223600; // 1.0074121f
	
	if (current_frame < 0.0) {
		current_frame = 0.0;
		SetPlayingSpeed(0);
		data.SetReaderOffset(0);
	}

	Frame interpolated_frame;
	Frame* frame = GenerateFrame(&interpolated_frame);
	Frame* i_frame = (Frame*)data.GetMemPtr(data.GetReaderOffset());
	ReplayPhysicsCarState* state = &frame->player_car;
	if (settings.flags[Flags::Flag_ShowInfoOnReplaying]) {
		_sprintf_s(msgbuf, msgbufsize, "Playing... x%g  Frame %.2f, XYZ %.3f, %.3f, %.3f", game_speed, current_frame,
				(double)state->position.x, (double)state->position.y, (double)state->position.z);
			//state->rotation.x, state->rotation.y, state->rotation.z);
	}
	FillPhysicsGameCar(plcar, state);
	if (current_frame >= 1.0) {
		GameCarCalcVelocity(plcar, i_frame->player_car, *(GetPrevReplayCarState(&i_frame->player_car)), frametime);
	}

	int i;
	for (i = 1; i < racing_car_num; i++) {
		RestoreMellowCar(i, frame, i_frame);
	}
	for (; i < total_car_num; i++) {
		RestoreMellowCar(i, frame, i_frame);
	}
	int i_old_frame = R_dtoi(current_frame);
	current_frame += game_speed;
	data.SetReaderOffset(data.GetReaderOffset() + GetSizeofFrame() * (R_dtoi(current_frame) - i_old_frame));
}
void Replayer::StartPlayNoReload() {
	_Proc = &Replayer::Play;
	clear_mgsbuf();
	D3DReinitFont(*ppD3D9Device);
	ResetReplayState();
}

void Replayer::Proc() {
	(this->*_Proc)();
}
void Replayer::Init() {
	DWORD oldp;
	VirtualProtect(Tweak_GameSpeed, 4, PAGE_READWRITE, &oldp);
	_Backup = _Proc = &Replayer::Ret;
	D3DXInit();
}
void Replayer::Release() {
	D3DXDeInit();
}

void FillRaceHeader(ReplayHeader *raceHeader, R_RaceType type, int ugracenumber) {
	raceHeader->seed = *Seed;
	raceHeader->racetype = type;
	switch (type) {
	case QuickRace: {
		raceHeader->race.QR.raceMode = *CurrentRaceMode;
		RaceOptions* raceOptions = cFrontendDatabase_GetOptions(FEDatabase, *CurrentRaceMode);
		R_memcpy(&raceHeader->race.QR.options, raceOptions, sizeof(*raceOptions));
		break;
	}
	case Underground: {
		raceHeader->race.ug.UgRaceNumber = ugracenumber;
		raceHeader->race.ug.difficulty = *UndergroundRaceDifficulty;
		break;
	}
	default:
		break;
	}
}

void Replayer::StartRec(R_RaceType type, int ugracenumber) {
	_Backup = _Proc = &Replayer::FirstFrameRec;
	average_frametime = 1.0 / 60;
	FillRaceHeader(&_replayHeader, type, ugracenumber);

	D3DReinitFont(*ppD3D9Device);
	clear_mgsbuf();
	ResetReplayState();
	data.resize(0);
}
void Replayer::StartPlay() {
	if (HFind != NULL) {
		FindClose(HFind);
		HFind = NULL;
	}
	if (LoadCurrentReplay() == false) {
		return;
	}
	clear_mgsbuf();
	ProcRaceHeader(&_replayHeader);
	_Backup = _Proc = &Replayer::FirstFramePlay;
	ResetReplayState();
}
void Replayer::Stop() {
	clear_mgsbuf();
	_Proc = &Replayer::Ret;
	ResetReplayState();
}

void Replayer::PrepareToSave() {
	int i_current_frame = R_dtoi(current_frame);
	_replayHeader.statesNum = i_current_frame * total_car_num;
	_replayHeader.frames = i_current_frame;
	_replayHeader.statestotalsize = data.size();
	char keynamebuf[32];
	GetKeyNameTextA(MapVirtualKeyA(settings.buttons[Buttons::button_save], MAPVK_VK_TO_VSC) << 16,
		keynamebuf, _countof(keynamebuf));
	_sprintf_s(msgbuf, msgbufsize, "Press %s to save replay", keynamebuf);
}

void Replayer::ResetCurrentMode() {
	clear_mgsbuf();
	if (_Proc == &Replayer::Play || _Backup == &Replayer::FirstFramePlay) {
		ResetReplayState();
		_Proc = &Replayer::FirstFramePlay;
	} else if (_Proc == &Replayer::Rec || _Backup == &Replayer::FirstFrameRec || _Backup == &Replayer::Rec) {
		if (_Proc == &Replayer::Rec) {
			PrepareToSave();
		}
		ResetReplayState();
		data.resize(0);
		_Proc = &Replayer::FirstFrameRec;
	}
}
void Replayer::PauseCurrentMode() {
	_Backup = _Proc;
	_Proc = &Replayer::Ret;
}
void Replayer::ContinueCurrentMode() {
	_Proc = _Backup;
}

void Replayer::ResetReplayState() {
	SetPlayingSpeed(1.0f);
	current_frame = 0.0;
	data.SetReaderOffset(0);
}

void Replayer::Save() {
	if (_Backup == &Replayer::FirstFrameRec) {
		SaveCurrentReplay();
	}
}
void Replayer::SetPlayingSpeed(float plsp) {
	game_speed = plsp;
	*Tweak_GameSpeed = R_fabs(plsp);
	current_frame = R_dtoi(current_frame);
}
float Replayer::ProcPlayingSpeed(float plsp) {
	if (plsp < 0) {
		plsp /= 2;
		if (plsp > -min_abs_playing_speed) {
			plsp = 0;
		}
	}
	else if (plsp > 0) {
		plsp *= 2;
		plsp = FitInto(plsp, max_abs_playing_speed);
	}
	else {
		plsp = min_abs_playing_speed;
	}
	return plsp;
}
void Replayer::IncreasePlayingSpeed() {
	SetPlayingSpeed(ProcPlayingSpeed(game_speed));
}
void Replayer::DecreasePlayingSpeed() {
	float new_sp = -ProcPlayingSpeed(-game_speed);
	SetPlayingSpeed( new_sp == -0.0f? 0.0f : new_sp );
}

Replayer::Replayer() {}
Replayer::~Replayer() {}