#include "stdafx.h"
#include "Replayer.h"

void MatrixAlignedToUnaligned(float MatrixAligned[3][4], float MatrixUnaligned[3][3]) {
	memcpy(&(MatrixUnaligned[0][0]), &(MatrixAligned[0][0]), 12);
	memcpy(&(MatrixUnaligned[1][0]), &(MatrixAligned[1][0]), 12);
	memcpy(&(MatrixUnaligned[2][0]), &(MatrixAligned[2][0]), 12);
}
void MatrixUnalignedToAligned(float MatrixAligned[3][4], float MatrixUnaligned[3][3]) {
	memcpy(&(MatrixAligned[0][0]), &(MatrixUnaligned[0][0]), 12);
	memcpy(&(MatrixAligned[1][0]), &(MatrixUnaligned[1][0]), 12);
	memcpy(&(MatrixAligned[2][0]), &(MatrixUnaligned[2][0]), 12);
}

char const filesdir[] = "replays/*";
char const filesdir_[] = "replays/";

const int msgbufsize = 256;
char msgbuf[msgbufsize] = "";

char currentrplfilename[256] = "";

HANDLE HFind = NULL;
WIN32_FIND_DATAA FFD;

void GetNameForReplay(char* buf) {
	WIN32_FIND_DATAA ffd;
	char tbuf[256];
	tbuf[0] = '\0';
	strcpy(buf, filesdir_);
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
bool GetNextFILE() {
	while (FindNextFileA(HFind, &FFD) != 0) {
		if (!(FFD.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			return true;
		}
	}
	return false;
}
void Replayer::ChooseButton() {
	D3DInitFont(*ppD3D9Device);
	char buf[256];
	if (HFind != NULL) {
		if (GetNextFILE()) {
			strcpy(buf, FFD.cFileName);
		}
		else {
			FindClose(HFind);
			HFind = NULL;
			goto l_else;
		}
	}
	else {
		l_else:
		HFind = FindFirstFileA(filesdir, &FFD);
		if (HFind == INVALID_HANDLE_VALUE) {
			HFind = NULL;
			return;
		}
		if (FFD.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			if (!GetNextFILE()) {
				strcpy(msgbuf, "There's no replays");
				currentrplfilename[0] = '\0';
				FindClose(HFind);
				HFind = NULL;
				return;
			}
		}
		strcpy(buf, FFD.cFileName);
	}
	sprintf_s(msgbuf, "Press Play button to play '%s'", buf);
	strcpy(currentrplfilename, filesdir_);
	strcat(currentrplfilename, buf);
}

size_t statesnum;
OVERLAPPED ovlp, ovlp2;
HANDLE HFile;
void* tosave;

void CALLBACK Saved(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped) { }
bool Replayer::SaveCurrentReplayAsync() { // it's not Async yet
	statesnum = states.size();
	if (statesnum == 0) {
		return false;
	}
	GetNameForReplay(currentrplfilename);
	HANDLE HFile = CreateFileA(currentrplfilename, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
	if (HFile == INVALID_HANDLE_VALUE) {
		return false;
	}
	tosave = &(states[0]);
	if (WriteFileEx(HFile, &statesnum, sizeof(statesnum), &ovlp, Saved) == false) {
		CloseHandle(HFile);
		return false;
	}
	sprintf_s(msgbuf, msgbufsize, "Saved as '%s'", currentrplfilename);
	ovlp2.Offset = sizeof(statesnum);
	bool t = WriteFileEx(HFile, &(states[0]), statesnum * sizeof(state), &ovlp2, Saved);
	CloseHandle(HFile);
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
	DWORD stnum;
	DWORD bytesreaded;
	if (ReadFile(hFile, &stnum, sizeof(stnum), &bytesreaded, NULL) == false) {
		CloseHandle(hFile);
		return false;
	}
	states.resize(stnum);
	bool t = ReadFile(hFile, &(states[0]), stnum * sizeof(state), &bytesreaded, NULL);
	CloseHandle(hFile);
	return t;
}

void Replayer::Ret(Object3D* obj) {};

void Replayer::PushState(Object3D* obj) {
	state t;
	t.position = obj->position;
	int size = states.size();
	sprintf_s(msgbuf, msgbufsize,	"Recording...  Frame %d, vector mem: %.2fKB, XYZ %.3f, %.3f, %.3f", size, 
		((double)states.capacity())*sizeof(state)/1024.0, (double)t.position.x, (double)t.position.y, (double)t.position.z);
	AlignedRotationMatrixToRotation(obj->RotationMatrixAligned, &(t.rotation));
	//MatrixAlignedToUnaligned(obj->RotationMatrixAligned, t.matrix);
	states.push_back(t);
}
void Replayer::Play(Object3D* obj) {
	if (reader_current_pos >= states.size()) {
		// memset(&(obj->speed), 0, 12);
		Stop();
		return;
	}
	state *t = &(states[reader_current_pos]);
	sprintf_s(msgbuf, msgbufsize,	"Playing...  Frame %d, XYZ %.3f, %.3f, %.3f", reader_current_pos, 
		(double)t->position.x, (double)t->position.y, (double)t->position.z);
	const float mul = 1.0 / 0.014;
	if (reader_current_pos) {
		obj->speed = (t->position - states[reader_current_pos - 1].position) * (mul);
	}
	//obj->speed = { 0, 0, 0 };
	obj->position = t->position;
	RotationToAlignedRotationMatrix(obj->RotationMatrixAligned, &(t->rotation));
	//MatrixUnalignedToAligned(obj->RotationMatrixAligned, t->matrix);
	reader_current_pos++;
}

void Replayer::Proc(Object3D* obj) {
	(this->*_Proc)(obj);
}

void Replayer::Init() {
	msg = msgbuf;
	_Proc = &Replayer::Ret;
}
void Replayer::Destroy() {
	states.clear();
	states.shrink_to_fit();
}

void Replayer::StartRec(Object3D* obj) {
	_Proc = &Replayer::PushState;
	// initial_pos = obj->position;
	D3DInitFont(*ppD3D9Device);
	states.resize(0);
}
void Replayer::SwitchRec(Object3D* obj) {
	if (_Proc == &Replayer::PushState) {
		Stop();
		SaveCurrentReplayAsync();
	}
	else {
		if (_Proc != &Replayer::Ret) {
			Stop();
		}
		StartRec(obj);
	}
}
void Replayer::StartPlay(Object3D* obj) {
	if (HFind != NULL) {
		FindClose(HFind);
		HFind = NULL;
	}
	if (LoadCurrentReplay() == false) {
		return;
	}
	_Proc = &Replayer::Play;
	D3DInitFont(*ppD3D9Device);
	//obj->position = initial_pos;
	reader_current_pos = 0;
}
void Replayer::SwitchPlay(Object3D* obj) {
	if (_Proc == &Replayer::Play) {
		Stop();
	}
	else {
		if (_Proc != &Replayer::Ret) {
			Stop();
			SaveCurrentReplayAsync();
			reader_current_pos = 0;
			_Proc = &Replayer::Play;
			return;
		}
		StartPlay(obj);
	}
}
void Replayer::Stop() {
	msgbuf[0] = '\0';
	_Proc = &Replayer::Ret;
	reader_current_pos = 0;
}

Replayer::Replayer() {}
Replayer::~Replayer() {}